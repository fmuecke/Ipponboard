#include "splashscreen.h"
#include "ui_splashscreen.h"

SplashScreen::SplashScreen(data const& data, QWidget *parent) :
	QDialog(parent),
    ui(new Ui::SplashScreen)
{
    ui->setupUi(this);

//	const int days_left = data.date.daysTo(QDate::currentDate());
//	ui->label_valid->setText(
//			"- " + QString::number( days_left ) +
//			" " + tr("days left") + " -");

	ui->textEdit->setText(data.text);
	//ui->textEdit->setHtml(data.text);
	setWindowFlags(Qt::SplashScreen);
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
