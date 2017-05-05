#include "qrng.h"
#include <QDebug>
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
		qWarning() << "Failed to read from" << randomFile.fileName()
				   << "with error" << randomFile.errorString();
		return;//TODO throw
	}
	auto rLen = size;
	while (rLen > 0) {
		auto read = randomFile.read((char*)data, rLen);
		if(read == -1) {
			qWarning() << "Failed to read from" << randomFile.fileName()
					   << "with error" << randomFile.errorString();
			return;//TODO throw
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
			qWarning() << "Failed to read entropy with error" << entropyFile.errorString();
			return 0;//TODO throw
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
