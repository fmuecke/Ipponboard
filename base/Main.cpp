#include "MainWindow.h"
#include "MainWindowTeam.h"
#include "../core/TournamentMode.h"
#include "../Widgets/SplashScreen.h"
#include "../base/versioninfo.h"
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

bool CheckForNeverVersion()
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

	query.setQuery("Ipponboard/Version/text()");
	if (query.isValid())
	{
		QString version;
		query.evaluateTo(&version);
		version = version.trimmed();

		QStringList onlineVer = version.split('.');
		QStringList buildVer = QString(VersionInfo::VersionStr).split('.');
		
		while (onlineVer.length() < 3) onlineVer.append("0");
		while (buildVer.length() < 3) buildVer.append("0");

		if (onlineVer[0].toInt() > buildVer[0].toInt() ||
			onlineVer[1].toInt() > buildVer[2].toInt() || 
			onlineVer[2].toInt() > buildVer[2].toInt())
		{
			auto result = QMessageBox::information(
				nullptr,
				QCoreApplication::tr("New version available"),
				QCoreApplication::tr("There is a newer version of Ipponboard available: %1\nDo you want to download it or visit the project homepage?").arg(version),
				QCoreApplication::tr("Download"), 
				QCoreApplication::tr("Visit Homepage"), 
				QCoreApplication::tr("Cancel"),
				0, 2);

			if (result == 0)
			{
				query.setQuery("Ipponboard/DownloadUrl/text()");
				if (query.isValid())
				{
					QString url;
					query.evaluateTo(&url);
					url = url.trimmed();
					return QDesktopServices::openUrl(QUrl(url));
				}
			}
			else if (result == 1)
			{
				query.setQuery("Ipponboard/InfoUrl/text()");
				if (query.isValid())
				{
					QString url;
					query.evaluateTo(&url);
					url = url.trimmed();
					if (!QDesktopServices::openUrl(QUrl(url)))
					{
						return QDesktopServices::openUrl(QUrl("http://www.ipponboard.info/"));
					}

					return true;
				}
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

void SetTranslation(QApplication& app, QTranslator& translator, QTranslator& coreTranslator, QString const& langStr)
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

        if (coreTranslator.load("core_" + langStr, langPath))
        {
            app.installTranslator(&coreTranslator);
        }
        else
        {
            LangNotFound("core_" + langStr);
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
    QTranslator coreTranslator; // Note: this object needs to remain in scope

    SetTranslation(a, translator, coreTranslator, langStr);

	if (CheckForNeverVersion())
	{
		return 0;
	}

    auto eulaText1 = QCoreApplication::tr("This version can be used without any fee. The unmodified version may and shall be copied and distributed freely.");
    auto eulaText2 = QCoreApplication::tr("Please consider to support the development and future maintainance of Ipponbord by a small donation.");
    auto eulaText3 = QCoreApplication::tr("To donate just get in touch with me or use the donate link on the Ipponboard project homepage.");
    auto eulaText4 = QCoreApplication::tr("If you have improvements regardings the design (view, handling) or the controlling - please let me know! "\
                                          "I would love hearing from you! Please leave your comments in the online survey <em>Menu&rarr;About&rarr;Feedback</em> - "\
                                          "it just takes a few seconds!");
    auto eulaText5 = QCoreApplication::tr("The most recent version can be found on the homepage:");
    auto eulaText6 = QCoreApplication::tr("Thank you very much!");

    const QString text = QString(
                "<html><body><h2><span style=\"color:#336699\">Ipponboard</span> <small>v%7</small></h2>"\
                "<!--<p><strong>%1</strong></p>-->"\
                "<p><span style=\"color:blue\"><em>%2</em></span></p>"\
                "<p>%3</p>"\
                "<p>%4</p>"\
                "<p>%5 <a href=\"http://www.ipponboard.info\">www.ipponboard.info</a></p>"\
                "<p><br/><em>%6</em></p>"\
				"</body></html>").arg(eulaText1, eulaText2, eulaText3, eulaText4, eulaText5, eulaText6, VersionInfo::VersionStrShort);

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
