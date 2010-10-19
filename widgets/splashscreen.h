#ifndef SPLASHSCREEN_H
#define SPLASHSCREEN_H

#include <QDialog>
#include <QDate>

namespace Ui {
	class SplashScreen;
}

class SplashScreen : public QDialog {
	Q_OBJECT
public:
	struct data
	{
		QDate date;
		QString text;
		QString info;
	};

	SplashScreen(data const& data, QWidget *parent = 0);
	~SplashScreen();

protected:
	void changeEvent(QEvent *e);

private:
	Ui::SplashScreen *ui;

private slots:
	void on_pushButton_pressed();
};

#endif // SPLASHSCREEN_H
