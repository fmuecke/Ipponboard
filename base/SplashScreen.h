// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef WIDGETS__SPLASHSCREEN_H_
#define WIDGETS__SPLASHSCREEN_H_

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
	void on_commandLinkButton_startSingleVersion_pressed();
	void on_commandLinkButton_startTeamVersion_pressed();
	void on_commandLinkButton_donate_pressed();
};

#endif // WIDGETS__SPLASHSCREEN_H_
