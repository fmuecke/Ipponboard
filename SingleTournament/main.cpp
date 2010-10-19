#include <QtGui/QApplication>
#include <QTranslator>
#include <QMessageBox>
#include "../base/mainwindow.h"
#include "../widgets/splashscreen.h"
#include "../base/versioninfo.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator translator;
	translator.load("Ipponboard_de");
	a.installTranslator(&translator);

	QCoreApplication::setApplicationVersion("0.4.2");
	QCoreApplication::setOrganizationName("Florian Mücke");
	QCoreApplication::setOrganizationDomain("ipponboard.origo.ethz.ch");
	QCoreApplication::setApplicationName("Ipponboard");

	QString text = QString(
		"<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN" "http://www.w3.org/TR/REC-html40/strict.dtd\">"
		"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">"
		"p, li { white-space: pre-wrap; }"
		"</style></head>"
		"<body style=\" font-family:'MS Shell Dlg 2'; font-size:10pt; font-weight:400; font-style:normal;\" bgcolor=\"#f7f7de\">"
		"<h2><span style=\"font-family:Cuprum,Helvetica,Arial;color:#336699\">%1</span><small> v%2</small></h2>"
		).arg( QCoreApplication::applicationName(),
			   QCoreApplication::applicationVersion() );

	text += QCoreApplication::tr(
		"<p>This version is provided for testing purposes and can be used without any fee. "
		"It will stop working after <b>March 31st 2010</b>.</p>"
		"<p>However, the unmodified version may be copied and distributed freely.</p>"
		"<p>If you have improvements regardings the design (view, handling) or the controlling - please tell us! "
		"We would like to hear from you!</p>"
		"<p>If you'd like to help us improving the application please fill out the enclosed online survey (<em>Menu&rarr;Survey</em>) - it just takes a few secs!</p>"
		"<p>The most recent version can be found on the following pages:"
		"<ul>"
		"<li><a href=\"http://flo.mueckeimnetz.de/ipponboard/\">http://flo.mueckeimnetz.de/ipponboard/</a></li>"
		"<li><a href=\"http://ipponboard.origo.ethz.ch\">http://ipponboard.origo.ethz.ch</a></li>"
		"</p>"
		"<p><br/><em>Thank you very much!</em></p>" );

	text += "</body></html>";

	SplashScreen::data splashData;
	splashData.text = text;
	splashData.info = QCoreApplication::applicationName()
					  + " v"
					  + QCoreApplication::applicationVersion()
					  + "\n"
					  + "Build: " + VersionInfo::Date;
	splashData.date = QDate(2010, 3, 31);
	SplashScreen splash(splashData);

	if( QDialog::Accepted != splash.exec() )
	return 0;

	const int days_left = splashData.date.daysTo(QDate::currentDate());
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
