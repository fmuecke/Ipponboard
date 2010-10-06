#include <QtGui/QApplication>
#include "gamepaddemo.h"


int main(int argc, char *argv[])
{
	QApplication a(argc, argv);

	GamepadDemo w;
	w.show();
	return a.exec();
}
