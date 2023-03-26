// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "MainWindow.h"
#include "MainWindowTeam.h"
#include "../core/TournamentMode.h"
#include "SplashScreen.h"
#include "versioninfo.h"
#include "UpdateChecker.h"
#include "../util/path_helpers.h"

#include <QtGui/QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include <QLocale>
#include <QtextCodec>



void LangNotFound(const QString& fileName)
{
	QMessageBox::critical(nullptr,
						  QCoreApplication::applicationName(),
						  "Unable to read language file: " + fileName +
						  "\nThe default language is being used.");
}

void SetTranslation(QApplication& app, QTranslator& translator, QString const& langStr)
{
	UNREFERENCED_PARAMETER(app);

	if (langStr == QString("en"))
	{
		return; // default
	}

	if (langStr == QString("de") || langStr == QString("nl"))
	{
		const QString& langPath =
			QCoreApplication::applicationDirPath() + QString("/lang");

		if (translator.load(langStr, langPath))
		{
			app.installTranslator(&translator);
		}
		else
		{
			LangNotFound(langStr);
		}
	}
}

int ShowSplashScreen()
{
	auto t1 = QCoreApplication::tr("the Judo score board and timer");
	auto t2 = QCoreApplication::tr("Judo is part of our lives.");
	auto t3 = QCoreApplication::tr("Ipponboard is developed with passion by judoka for judoka to provide the best possible experience.");
	auto t4 = QCoreApplication::tr(
				"<p>As of December 2020 Ipponboard is open source.<br/>It can be used without restrictions and free of charge.</p>"
				"<p>You can contribute to its development by"
				"<ul>"
				"<li>giving feedback, wishes, and suggestions</li>"
				"<li>participating in the development</li>"
				"<li>providing donations for a coffee or a pizza</li>"
				"</ul></p>"
				"<p>See project site on <a href=\"https://github.com/fmuecke/Ipponboard\">GitHub</a> for further information.</p>");
	auto t5 = QCoreApplication::tr("Thank you!");

	auto text = QString("<html><body><p><big><span style=\"color:#336699;font-weight:bold\">Ipponboard</span> - %1</big></p>"
						"<blockquote><p><em><b>%2</b><br/>%3</em></p>"
						"%4</blockquote>"
						"<p><em>%5</em></p>"
						"</body></html>").arg(t1, t2, t3, t4, t5);

	SplashScreen::Data splashData;
	splashData.text = text;
	splashData.info = QCoreApplication::applicationName()
					  + " v" + QCoreApplication::applicationVersion()
					  + "\n"
					  + "Build: " + VersionInfo::Date;
	SplashScreen splash(splashData);
	//splash.SetImageStyleSheet("image: url(:/res/images/logo_team.png);");
	//splash.resize(480, 410);

	auto dlgResult = splash.exec();

	if (QDialog::Rejected == dlgResult)
	{
		return 0;
	}

	return dlgResult;
}

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setApplicationVersion(VersionInfo::VersionStr);
	QCoreApplication::setOrganizationName("Florian Mücke");
	QCoreApplication::setOrganizationDomain("ipponboard.koe-judo.de");
	QCoreApplication::setApplicationName("Ipponboard");

	// read language code
	QString langStr = QLocale::system().name();
	langStr.truncate(langStr.lastIndexOf('_'));

	const QString ini(QString::fromStdString(fm::GetSettingsFilePath("Ipponboard.ini")));

	QSettings settings(ini, QSettings::IniFormat, &a);
	settings.beginGroup(str_tag_Main);

	if (settings.contains(str_tag_Language))
	{
		langStr = settings.value(str_tag_Language).toString();
	}

	settings.endGroup();

	QTranslator translator;  // Note: this object needs to remain in scope.
	SetTranslation(a, translator, langStr);

	if (UpdateChecker::CheckForNewerVersion())
	{
		return 0;
	}

	auto dlgResult = ShowSplashScreen();

	if (dlgResult == 0)
	{
		return 0;
	}

	std::unique_ptr<MainWindowBase> pMainWnd = nullptr;

	if (dlgResult == QDialog::Accepted + 1)
	{
		pMainWnd = std::make_unique<MainWindowTeam>();
	}
	else
	{
		pMainWnd = std::make_unique<MainWindow>();
	}

	try
	{
		pMainWnd->Init();
	}
	catch (std::exception const&)
	{
		return 0;
	}

	pMainWnd->show();

	return a.exec();
}
