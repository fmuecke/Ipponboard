// Copyright 2010-2014 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
// $Id$
#include "DonationManager.h"
#include "splashscreen.h"
#include "ui_splashscreen.h"
#include "versioninfo.h"

#include <algorithm>
using namespace std;

SplashScreen::SplashScreen(Data const& data, QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::SplashScreen)
{
	ui->setupUi(this);

//	const int days_left = data.date.daysTo(QDate::currentDate());
//	ui->label_valid->setText(
//			"- " + QString::number( days_left ) +
//			" " + tr("days left") + " -");

	ui->textEdit_eula->setHtml(data.text);
	ui->label_info->setText(data.info);
	//ui->labelCopyright->setText(QString("© %1 Florian Mücke").arg(VersionInfo::CopyrightYear));
	ui->commandLinkButton_donate->setText(DonationManager::GetDonationLabel());

	/*std::vector<QCommandLinkButton*> buttons =
	{
	    ui->commandLinkButton_donate,
	    ui->commandLinkButton_startSingleVersion,
	    ui->commandLinkButton_startTeamVersion
	};

	buttons.erase(
	        remove_if(
	              begin(buttons),
	              end(buttons),
	              [](QCommandLinkButton* b) { return !b->isEnabled(); }),
	        end(buttons));

	auto r = qrand() % buttons.size();
	buttons[r]->setFocus();
	buttons[r]->setDefault(true);

	//ui->commandLinkButton_donate->setFocus();
	*/

	setWindowFlags(Qt::Window);
}

SplashScreen::~SplashScreen()
{
	delete ui;
}

void SplashScreen::SetImageStyleSheet(QString const& /*text*/)
{
	//"image: url(:/res/images/logo.png);"
	//ui->widget_image->setStyleSheet(text);
}

void SplashScreen::changeEvent(QEvent* e)
{
	QWidget::changeEvent(e);

	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;

	default:
		break;
	}
}

void SplashScreen::on_commandLinkButton_startSingleVersion_pressed()
{
	accept();
}

void SplashScreen::on_commandLinkButton_startTeamVersion_pressed()
{
	done(QDialog::Accepted + 1);
}

void SplashScreen::on_commandLinkButton_donate_pressed()
{
	DonationManager::OpenUrl();
}

//void SplashScreen::on_commandLinkButton_cancel_pressed()
//{
//	reject();
//}
