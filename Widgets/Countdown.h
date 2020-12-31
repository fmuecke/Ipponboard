// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef WIDGETS__COUNTDOWN_H_
#define WIDGETS__COUNTDOWN_H_

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
	explicit Countdown(int secs, QWidget* parent = 0);
	virtual ~Countdown();

protected:
	void changeEvent(QEvent* e);
	void timerEvent(QTimerEvent* e);

private:
	Ui::Countdown* ui;
	int m_seconds;
	QBasicTimer timer;

private slots:
};

#endif  // WIDGETS__COUNTDOWN_H_
