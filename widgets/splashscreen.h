#ifndef WIDGETS__SPLASHSCREEN_H_
#define WIDGETS__SPLASHSCREEN_H_
// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
// $Id$

#include <QDialog>
#include <QDate>

namespace Ui
{
class SplashScreen;
}

class SplashScreen : public QDialog
{
	Q_OBJECT

public:
	struct Data
	{
		QDate date;
		QString text;
		QString info;
	};

	SplashScreen(Data const& data, QWidget* parent = 0);
	~SplashScreen();

	void SetImageStyleSheet(QString const& text);

protected:
	void changeEvent(QEvent* e);

private:
	Ui::SplashScreen* ui;

private slots:
	void on_pushButton_Cancel_pressed();
	void on_pushButton_pressed();
};

#endif  // WIDGETS__SPLASHSCREEN_H_
