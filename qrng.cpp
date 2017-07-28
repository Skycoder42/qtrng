#include "qrng.h"
#if defined(Q_OS_UNIX)
#include <QFile>
#elif defined(Q_OS_WIN)
#include <qt_windows.h>
#include <QDebug>
#endif

#ifdef Q_OS_WIN
static QString formatWinError(DWORD winError)
{
	wchar_t *buffer = NULL;
	auto num = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
							  NULL,
							  winError,
							  0,
							  (LPWSTR)&buffer,
							  0,
							  NULL);
	if(buffer) {
		QString res = QString::fromWCharArray(buffer, num);
		LocalFree(buffer);
		return res;
	} else
		return QString();
}
#endif

QRng::QRng(QObject *parent) :
	QObject(parent),
	_securityLevel(NormalSecurity)
{}

void QRng::generateRandom(void *data, const int size)
{
#if defined(Q_OS_UNIX)
	QFile randomFile;
	switch (_securityLevel) {
	case QRng::NormalSecurity:
		randomFile.setFileName(QStringLiteral("/dev/urandom"));
		break;
	case QRng::HighSecurity:
		randomFile.setFileName(QStringLiteral("/dev/random"));
		break;
	default:
		Q_UNREACHABLE();
		break;
	}

	if(!randomFile.open(QIODevice::ReadOnly | QIODevice::Unbuffered)) {
		throw QRngException(QStringLiteral("Failed to read from %1 with error: %2")
							.arg(randomFile.fileName())
							.arg(randomFile.errorString())
							.toUtf8());
	}
	auto rLen = size;
	while (rLen > 0) {
		auto read = randomFile.read((char*)data, rLen);
		if(read == -1) {
			throw QRngException(QStringLiteral("Failed to read from %1 with error: %2")
								.arg(randomFile.fileName())
								.arg(randomFile.errorString())
								.toUtf8());
		}
		rLen -= read;
	}
	randomFile.close();
#elif defined(Q_OS_WINRT)
	BCRYPT_ALG_HANDLE *phAlgorithm = NULL;

	if (!::BCryptOpenAlgorithmProvider(phAlgorithm, BCRYPT_AES_GMAC_ALGORITHM, NULL, BCRYPT_PROV_DISPATCH)) {
		throw QRngException(QStringLiteral("Failed to load RNG with error: %1")
							.arg(formatWinError(GetLastError()))
							.toUtf8());
	}

	if (!::BCryptGenRandom(phAlgorithm, (PUCHAR)data, (ULONG)size, 0)) {
		::BCryptCloseAlgorithmProvider(phAlgorithm, 0);
		throw QRngException(QStringLiteral("Failed to load RNG with error: %1")
							.arg(formatWinError(GetLastError()))
							.toUtf8());
	}

	if (!::BCryptCloseAlgorithmProvider(phAlgorithm, 0)) {
		qWarning() << "Failed to release RNG with error:"
				   << formatWinError(GetLastError());
	}
#elif defined(Q_OS_WIN)
	HCRYPTPROV hProvider = NULL;

	if (!::CryptAcquireContext(&hProvider, NULL, NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT)) {
		throw QRngException(QStringLiteral("Failed to load RNG with error: %1")
							.arg(formatWinError(GetLastError()))
							.toUtf8());
	}

	if (!::CryptGenRandom(hProvider, (DWORD)size, (BYTE*)data))
	{
		::CryptReleaseContext(hProvider, 0);
		throw QRngException(QStringLiteral("Failed to load RNG with error: %1")
							.arg(formatWinError(GetLastError()))
							.toUtf8());
	}

	if (!::CryptReleaseContext(hProvider, 0)) {
		qWarning() << "Failed to release RNG with error:"
				   << formatWinError(GetLastError());
	}
#else
#error "Unsupported OS"
#endif
}

QByteArray QRng::generateRandom(int size)
{
	QByteArray data(size, Qt::Uninitialized);
	generateRandom(data.data(), data.size());
	return data;
}

QRng::SecurityLevel QRng::securityLevel() const
{
	return _securityLevel;
}

int QRng::currentEntropy(bool asBytes) const
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MAC)
	switch (_securityLevel) {
	case QRng::NormalSecurity:
		return UnlimitedEntropy;
	case QRng::HighSecurity:
	{
		QFile entropyFile(QStringLiteral("/proc/sys/kernel/random/entropy_avail"));
		if(!entropyFile.open(QIODevice::ReadOnly)) {
			throw QRngException(QStringLiteral("Failed to read entropy with error: %2")
								.arg(entropyFile.errorString())
								.toUtf8());
		}
		auto entropy = entropyFile.readLine().trimmed().toInt();
		entropyFile.close();
		if(asBytes)
			return entropy / 8;
		else
			return entropy;
	}
	default:
		Q_UNREACHABLE();
		return UnlimitedEntropy;
	}
#else
	Q_UNUSED(asBytes);
	return UnlimitedEntropy;
#endif
}

void QRng::setSecurityLevel(QRng::SecurityLevel securityLevel)
{
	if (_securityLevel == securityLevel)
		return;

	_securityLevel = securityLevel;
	emit securityLevelChanged(securityLevel);
}



QRngException::QRngException(const QByteArray &error) :
	_error(error)
{}

const char *QRngException::what() const noexcept
{
	return _error.constData();
}

void QRngException::raise() const
{
	throw *this;
}

QException *QRngException::clone() const
{
	return new QRngException(_error);
}
