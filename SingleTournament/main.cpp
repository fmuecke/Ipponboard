#include <QtGui/QApplication>
#include <QTranslator>
#include <QMessageBox>
#include "../base/mainwindow.h"
#include "../widgets/splashscreen.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator translator;
	translator.load("Ipponboard_de");
	a.installTranslator(&translator);

	QCoreApplication::setApplicationVersion("0.4.1");
	QCoreApplication::setOrganizationName("Florian Mücke");
	QCoreApplication::setOrganizationDomain("ipponboard.origo.ethz.ch");
	QCoreApplication::setApplicationName("Ipponboard");

	QString text = a.tr(
		"%1 v%2\n\n"
		"Diese Version wird im Rahmen einer Testphase zur Verfügung gestellt "
		"und ist uneingeschränkt bis zum 31. März 2010 lauffähig. "
		"Sie darf in unmodifizierter Version unentgeltlich kopiert und "
		"verbreitet werden."
		"\n\n"
		"Falls Sie Verbesserungsvorschläge bzgl. der Anzeige (Darstellung, "
		"Bedienung) oder der Steuerung haben, oder aber finden, dass es noch "
		"irgendwo hakt, dann teilen Sie uns dies bitte mit - wir freuen uns, "
		"wenn wir unsere Software noch besser machen können."
		"\n\n"
		"Um uns detailliert weiterzuhelfen, können Sie einfach den "
		"beiligenden Fragebogen ausfüllen (geht auch online!)."
		"\n\n"
		"Die neueste Version gibt es aktuell immer unter "
		"http://ipponboard.origo.ethz.ch."
		"\n"
		"\n"
		"Vielen Dank!").arg(
				QCoreApplication::applicationName(),
				QCoreApplication::applicationVersion());

	SplashScreen::data splashData;
	splashData.text = text;
	splashData.date = QDate(2010, 3, 31);
	SplashScreen splash(splashData);
	splash.exec();

	const int days_left = splashData.date.daysTo(QDate::currentDate());
	if( days_left <= 0 )
	{
		QMessageBox::warning(0, a.tr("Warning"), a.tr(
			"This version is no longer valid!\n\n"
			"You need to visit the project webpage for an update."));

		return 0;
	}
	if( days_left < 30 )
	{
		QMessageBox::warning(0, a.tr("Warning"), a.tr(
			"This version will stop to work in less than 30 days!\n\n"
			"Please visit the project webpage - there should be a newer one available."));
	}
	MainWindow w;
	w.show();

	return a.exec();
}
