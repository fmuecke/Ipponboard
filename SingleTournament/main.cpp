#include <QtGui/QApplication>
#include <QTranslator>
#include "../base/mainwindow.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	QTranslator translator;
	translator.load("Ipponboard_de");
	a.installTranslator(&translator);

	MainWindow w;
	w.show();

	return a.exec();
}
