#include "MainWindow.h"
#include "../Widgets/Countdown.h"
#include "../Widgets/SplashScreen.h"
#include "../base/versioninfo.h"
#include "../util/path_helpers.h"

#include <QtGui/QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QSettings>
#include <QFile>

int DelayUser()
{
	Countdown dlg(25);
	return dlg.exec();
}

void LangNotFound(const QString& fileName)
{
	QMessageBox::critical(nullptr,
						  QCoreApplication::applicationName(),
						  "Unable to read language file: " + fileName +
						  "\nThe default language is being used.");
}

int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	QCoreApplication::setApplicationVersion(VersionInfo::VersionStr);
	QCoreApplication::setOrganizationName("Florian Mücke");
	QCoreApplication::setOrganizationDomain("ipponboard.info");
	QCoreApplication::setApplicationName("Ipponboard (Basic Edition)");

	MainWindow mainWnd;
	mainWnd.setWindowTitle(QCoreApplication::applicationName() + " v" +
						   QCoreApplication::applicationVersion());

	// read language code
	QString langStr = QLocale::system().name();
	langStr.truncate(langStr.lastIndexOf('_'));

	const QString ini(QString::fromStdString(
						  fmu::GetSettingsFilePath(mainWnd.GetConfigFileName().toAscii())));
	QSettings settings(ini, QSettings::IniFormat, &a);

	settings.beginGroup(str_tag_Main);

	if (settings.contains(str_tag_Language))
		langStr = settings.value(str_tag_Language).toString();

	settings.endGroup();

	QTranslator translator;  // Note: this object needs to stay.
	QTranslator coreTranslator; // Note: this object needs to stay.

	if (langStr == QString("de"))
	{
		const QString& langPath =
			QCoreApplication::applicationDirPath() + QString("/lang");

		if (translator.load("Ipponboard_de", langPath))
			a.installTranslator(&translator);
		else
			LangNotFound("Ipponboard_de");

		if (coreTranslator.load("core_de", langPath))
			a.installTranslator(&coreTranslator);
		else
			LangNotFound("core_de");
	}

	QFile f(langStr == "de" ? ":/text/text/License_de.html" : ":/text/text/License_en.html");
	f.open(QIODevice::ReadOnly | QIODevice::Text);
	const QByteArray data = f.readAll();
	f.close();
	const QString text = QTextCodec::codecForHtml(data)->toUnicode(data);

	SplashScreen::Data splashData;
	splashData.text = text;
	splashData.info = QCoreApplication::applicationName()
					  + " v" + QCoreApplication::applicationVersion()
					  + "\n"
					  + "Build: " + VersionInfo::Date;
	splashData.date = QDate(2012, 7, 30);
	SplashScreen splash(splashData);

	if (QDialog::Accepted != splash.exec())
		return 0;

	//const int days_left = QDate::currentDate().daysTo(splashData.date);

	//if (days_left <= 0)
	//{
	//    QMessageBox::critical(0,
	//                          QCoreApplication::tr("Warning"),
	//                          QCoreApplication::tr(
	//                              "This version is no longer valid!\n\n"
	//                              "You need to visit the project homepage for a (free) update."));
	//
	//    if (QDialog::Accepted != DelayUser())
	//        return 0;
	//}
	//else if (days_left < 30)
	//{
	//    QMessageBox::warning(0,
	//                         QCoreApplication::tr("Warning"),
	//                         QCoreApplication::tr(
	//                             "This version will stop to work in less than 30 days!\n\n"
	//                             "Please visit the project homepage - there should be a newer version available."));
	//}

	mainWnd.Init();
	mainWnd.show();

	return a.exec();
}
