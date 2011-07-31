#include "splashscreen.h"
#include "ui_splashscreen.h"

SplashScreen::SplashScreen(Data const& data, QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SplashScreen)
{
	ui->setupUi(this);

//	const int days_left = data.date.daysTo(QDate::currentDate());
//	ui->label_valid->setText(
//			"- " + QString::number( days_left ) +
//			" " + tr("days left") + " -");

	ui->textEdit->setHtml(data.text);
	ui->label_info->setText(data.info);
	setWindowFlags(Qt::Window);
}

SplashScreen::~SplashScreen()
{
	delete ui;
}

void SplashScreen::changeEvent(QEvent *e)
{
	QWidget::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void SplashScreen::on_pushButton_pressed()
{
	accept();
}

void SplashScreen::on_pushButton_Cancel_pressed()
{
	reject();
}
