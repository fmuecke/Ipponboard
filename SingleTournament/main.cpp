#include <QtGui/QApplication>
#include <QTranslator>
#include <QSettings>
#include <QMessageBox>
#include "../base/mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QCoreApplication::setApplicationName(QLatin1String("Ipponboard"));
	QCoreApplication::setApplicationVersion("0.5");
	QCoreApplication::setOrganizationName(QLatin1String("Florian Mücke"));
	QCoreApplication::setOrganizationDomain("ipponboard.origo.ethz.ch");

	// load translation file(s)
	const QSettings settings(//QSettings::NativeFormat,
							 QSettings::UserScope,
							 QCoreApplication::applicationName(),
							 QCoreApplication::applicationName());
	QLocale locale;
	QString langStr = ( locale.language() == QLocale::German ) ?
						QLatin1String("de") : QLatin1String("en");
	langStr = settings.value("Language", langStr).toString();

	if( langStr != QLatin1String("en") )
	{
		const QString& langPath =
				QCoreApplication::applicationDirPath() + QLatin1String("/lang");

		QTranslator translator;
		langStr = QCoreApplication::applicationName() + QLatin1String("_") + langStr;
		if( translator.load(langStr, langPath) )
		{
			a.installTranslator(&translator);
		}
		else
		{
			QMessageBox::critical(nullptr,
								  QCoreApplication::applicationName(),
								  "Unable to read language file: " + langStr +
								  "\nThe default language is being used.");
		}
	}

	// show main window
	MainWindow w;
	w.show();

	return a.exec();
}
