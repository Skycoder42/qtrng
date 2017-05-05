#include "widget.h"
#include "ui_widget.h"

Widget::Widget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Widget),
	random(new QRng(this))
{
	ui->setupUi(this);
	on_entropyButton_clicked();
}

Widget::~Widget()
{
	delete ui;
}

void Widget::on_generateButton_clicked()
{
	random->setSecurityLevel((QRng::SecurityLevel)ui->securityLevelComboBox->currentIndex());
	auto data = random->generateRandom(ui->generateSpinBox->value());
	switch (ui->encodingComboBox->currentIndex()) {
	case 0:	//base64
		ui->textBrowser->setPlainText(QString::fromLatin1(data.toBase64()));
		break;
	case 1:	//hex
		ui->textBrowser->setPlainText(QString::fromLatin1(data.toHex()));
		break;
	case 2:	//raw
		ui->textBrowser->setPlainText(QString::fromLatin1(data));
		break;
	default:
		Q_UNREACHABLE();
		break;
	}
	on_entropyButton_clicked();
}

void Widget::on_entropyButton_clicked()
{
	auto e = random->currentEntropy();
	if(e == QRng::UnlimitedEntropy)
		ui->entropyEdit->setText(tr("Unlimited"));
	else
		ui->entropyEdit->setText(tr("%L1 bit").arg(e));
}
