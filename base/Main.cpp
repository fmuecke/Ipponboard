#include "MainWindow.h"
#include "MainWindowTeam.h"
#include "../core/TournamentMode.h"
#include "SplashScreen.h"
#include "versioninfo.h"
#include "../util/path_helpers.h"

#include <QtGui/QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include <QLocale>
#include <QtextCodec>
#include <QXmlQuery>
#include <QUrl>
#include <QDesktopServices>

QString GetXmlValue(QXmlQuery query, QString xPath)
{
    QString value;
    query.setQuery(xPath);
    if (query.isValid())
    {
        query.evaluateTo(&value);
        return value.trimmed();
    }

    return value;
}


bool CheckForNewerVersion()
{
	QXmlQuery query;
	try
	{
		query.setFocus(QUrl("http://ipponboard.koe-judo.de/files/current_version.xml"));
	}
	catch (...)
	{
		return false;
	}

    auto version = GetXmlValue(query, "Ipponboard/Version/text()");
    if (!version.isEmpty())
	{
		QStringList onlineVer = version.split('.');
		QStringList buildVer = QString(VersionInfo::VersionStr).split('.');
		
		while (onlineVer.length() < 3) onlineVer.append("0");
		while (buildVer.length() < 3) buildVer.append("0");

		bool isOldVersion = false;
		if ((onlineVer[0].toInt() > buildVer[0].toInt()))
		{
			isOldVersion = true;
		}
		else if (onlineVer[0].toInt() == buildVer[0].toInt())
		{
			if (onlineVer[1].toInt() > buildVer[1].toInt())
			{
				isOldVersion = true;
			}
			else if ((onlineVer[1].toInt() == buildVer[1].toInt()))
			{
				if ((onlineVer[2].toInt() > buildVer[2].toInt()))
				{
					isOldVersion = true;
				}
			}
		}

		if (isOldVersion)
		{
            auto lang = QCoreApplication::tr("en");
            lang = lang == "de" ? "de" : "en";
            auto changes = GetXmlValue(query, "Ipponboard/Changes[@lang='" + lang + "']/text()");
            if (!changes.isEmpty())
            {
                changes = QString(": <em>%1</em>").arg(changes);
            }

            QString msg = QString("<p>%1 %2</p>")
                    .arg(
                        QCoreApplication::tr("Version %1 available").arg(
                            QString("<b>%1</b>").arg(version)))
                    .arg(changes);

            auto downloadUrl = GetXmlValue(query, "Ipponboard/DownloadUrl/text()");
            auto infoUrl = GetXmlValue(query, "Ipponboard/InfoUrl/text()");

            msg += QString("<p>%1</p>")
                    .arg(QCoreApplication::tr("Do you want to download it or visit the project homepage?"));

            auto result = QMessageBox::information(
                        nullptr,
                        QCoreApplication::tr("Ipponboard - New Version Available"),
                        msg,
                        QCoreApplication::tr("Download"),
                        QCoreApplication::tr("Visit Homepage"),
                        QCoreApplication::tr("Cancel"),
                        0, 2);

			if (result == 0)
			{
                return QDesktopServices::openUrl(QUrl(downloadUrl));
            }
			else if (result == 1)
			{
                if (!QDesktopServices::openUrl(QUrl(infoUrl)))
                {
                    return QDesktopServices::openUrl(QUrl("http://www.ipponboard.info/"));
                }

                return true;
            }
		}
	}

	return false;
}

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

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setApplicationVersion(VersionInfo::VersionStr);
	QCoreApplication::setOrganizationName("Florian Mücke");
	QCoreApplication::setOrganizationDomain("ipponboard.info");
	QCoreApplication::setApplicationName("Ipponboard");

	// read language code
	QString langStr = QLocale::system().name();
	langStr.truncate(langStr.lastIndexOf('_'));

	const QString ini(
		QString::fromStdString(
			fm::GetSettingsFilePath("Ipponboard.ini")));

	QSettings settings(ini, QSettings::IniFormat, &a);
	settings.beginGroup(str_tag_Main);

	if (settings.contains(str_tag_Language))
    {
		langStr = settings.value(str_tag_Language).toString();
    }

	settings.endGroup();

    QTranslator translator;  // Note: this object needs to remain in scope.

    SetTranslation(a, translator, langStr);

    if (CheckForNewerVersion())
	{
		return 0;
	}

    auto t1 = QCoreApplication::tr("the score board for judoka by judoka");
    auto t10 = QCoreApplication::tr("Judo is part of our lives.");
    auto t2 = QCoreApplication::tr("Therefore, Ipponboard is not just a simple display program, but developed by judoka for judoka. It is not only helpful for the people at the time table, but also for the trainers, the audience and the fighters themselves.");
    auto t3 = QCoreApplication::tr("Ipponboard is not only clearly readable but also revolutionary easy and intuitive to use. Therefore, it is appreciated by clubs and organizations around the world and has been used for many years at major championships.");
    auto t4 = QCoreApplication::tr("This version can be used without restriction. Copying in unchanged form is permitted.");
    auto t5 = QCoreApplication::tr("If you like Ipponboard, please support its development by:");
    auto t6 = QCoreApplication::tr("feedback");
    auto t7 = QCoreApplication::tr("wishes and suggestions");
    auto t8 = QCoreApplication::tr("or by giving appreciative donations");
    auto t9 = QCoreApplication::tr("Thank you very much!");

    auto text = QString("<html><body><p><big><span style=\"color:#336699;font-weight:bold\">Ipponboard</span> - %1</big></p>"
                        "<blockquote><p><em><b>%10</b> %2</em></p>"
                        "<p><em>%3</em></p></blockquote>"
                        "<p>%4</p>"
                        "<p>%5<ul>"
                        "<li>%6</li>"
                        "<li>%7</li>"
                        "<li>%8</li></ul></p>"
                        "<p><em>%9</em></p>"
                        "</body></html>").arg(t1, t2, t3, t4, t5, t6, t7, t8, t9).arg(t10);

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
