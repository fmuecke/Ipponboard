#include <QtGui/QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include "../base/mainwindow.h"
#include "../widgets/splashscreen.h"
#include "../base/versioninfo.h"
#include "../util/helpers.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setApplicationVersion(VersionInfo::VersionStr);
	QCoreApplication::setOrganizationName("Florian Mücke");
	QCoreApplication::setOrganizationDomain("ipponboard.origo.ethz.ch");
	QCoreApplication::setApplicationName("Ipponboard");

	// read language code
	const QString ini(
			QString::fromStdString(FMU::GetSettingsFilePath(str_ini_name)));

	QSettings settings(ini, QSettings::IniFormat, &a);
	settings.beginGroup(str_tag_Main);
	const QString langStr = settings.value(str_tag_Language,"en").toString();
	settings.endGroup();

	QTranslator translator;
	if( langStr != QString("en") )
	{
		const QString& langPath =
				QCoreApplication::applicationDirPath();// + Qtring("/lang");

		const QString langFile =
			QCoreApplication::applicationName() + QString("_") + langStr;
		if( translator.load(langFile, langPath) )
		{
			a.installTranslator(&translator);
		}
		else
		{
			QMessageBox::critical(nullptr,
								  QCoreApplication::applicationName(),
								  "Unable to read language file: " + langFile +
								  "\nThe default language is being used.");
		}
	}


	QFile f(langStr == "de" ? ":/text/text/License_de.html" : ":/text/text/License_en.html");
	f.open(QIODevice::ReadOnly | QIODevice::Text);
	const QByteArray data = f.readAll();
	f.close();
	const QString text = QTextCodec::codecForHtml(data)->toUnicode(data);

	SplashScreen::data splashData;
	splashData.text = text;
	splashData.info = QCoreApplication::applicationName()
					  + " v"
					  + QCoreApplication::applicationVersion()
					  + "\n"
					  + "Build: " + VersionInfo::Date;
	splashData.date = QDate(2011, 6, 30);
	SplashScreen splash(splashData);

	if( QDialog::Accepted != splash.exec() )
	return 0;

	const int days_left = QDate::currentDate().daysTo(splashData.date);
	if( days_left <= 0 )
	{
		QMessageBox::warning(0,
			QCoreApplication::tr("Warning"),
			QCoreApplication::tr(
				"This version is no longer valid!\n\n"
				"You need to visit the project webpage for an update."));

		return 0;
	}
	if( days_left < 30 )
	{
		QMessageBox::warning(0,
			QCoreApplication::tr("Warning"),
			QCoreApplication::tr(
				"This version will stop to work in less than 30 days!\n\n"
				"Please visit the project webpage - there should be a newer one available."));
	}

	MainWindow w;
	w.show();

	return a.exec();
}
