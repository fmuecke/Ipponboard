#ifndef WIDGETS__COUNTDOWN_H_
#define WIDGETS__COUNTDOWN_H_
// Copyright 2010-2012 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
// $Id: countdown.h 215 2011-11-01 12:28:21Z fmuecke $

#include <QDialog>
#include <QBasicTimer>

namespace Ui
{
class Countdown;
}

class Countdown : public QDialog
{
	Q_OBJECT

public:
	explicit Countdown(int secs, QWidget* parent = nullptr);
	virtual ~Countdown();

protected:
	void changeEvent(QEvent* e);
	void timerEvent(QTimerEvent * e);

private:
	Ui::Countdown* ui;
	int m_seconds;
	QBasicTimer timer;

private slots:
};

#endif  // WIDGETS__COUNTDOWN_H_
