#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <qrng.h>

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
	Q_OBJECT

public:
	explicit Widget(QWidget *parent = 0);
	~Widget();

private slots:
	void on_generateButton_clicked();

	void on_entropyButton_clicked();

private:
	Ui::Widget *ui;
	QRng *random;
};

#endif // WIDGET_H
