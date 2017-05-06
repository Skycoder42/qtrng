#include "qrng.h"
#ifdef Q_OS_UNIX
#include <QFile>
#endif

QRng::QRng(QObject *parent) :
	QObject(parent),
	_securityLevel(NormalSecurity)
{}

void QRng::generateRandom(void *data, const int size)
{
#ifdef Q_OS_UNIX
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
#ifdef Q_OS_UNIX
	switch (_securityLevel) {
	case QRng::NormalSecurity:
		return -1;
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
		return -1;
	}
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
