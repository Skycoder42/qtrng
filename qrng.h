#ifndef QRNG_H
#define QRNG_H

#include <QObject>
#include <QByteArray>
#include <type_traits>

//TODO exceptions!
class QRng : public QObject
{
	Q_OBJECT

	Q_PROPERTY(SecurityLevel securityLevel READ securityLevel WRITE setSecurityLevel NOTIFY securityLevelChanged)

public:
	enum SecurityLevel {
		NormalSecurity,
		HighSecurity
	};
	Q_ENUM(SecurityLevel)

	const static int UnlimitedEntropy = -1;

	QRng(QObject *parent = nullptr);

	void generateRandom(void *data, const int size);
	QByteArray generateRandom(int size);
	template <typename T>
	T generateRandom() const;

	SecurityLevel securityLevel() const;
	int currentEntropy(bool asBytes = false) const;

public slots:
	void setSecurityLevel(SecurityLevel securityLevel);

signals:
	void securityLevelChanged(SecurityLevel securityLevel);

private:
	SecurityLevel _securityLevel;
};

template<typename T>
T QRng::generateRandom() const
{
	static_assert(std::is_integral<T>::value, "T must be an integral type!");
	T value;
	generateRandom(&value, (int)sizeof(T));
	return value;
}

#endif // QRNG_H
