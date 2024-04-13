// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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

	ui->textBrowser_eula->setHtml(data.text);
	ui->label_info->setText(data.info);
	ui->commandLinkButton_donate->setText(DonationManager::GetDonationLabel());

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
