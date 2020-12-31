// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "countdown.h"
#include "ui_countdown.h"

Countdown::Countdown(int secs, QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::Countdown)
	, m_seconds(secs)
{
	ui->setupUi(this);

	ui->scaledText->SetColor(QColor(0, 0, 0), QColor(0xF7, 0xF7, 0xDE));
	ui->scaledText->SetText(QString::number(m_seconds));
	setWindowFlags(Qt::Dialog);

	timer.start(1000, this);
}

Countdown::~Countdown()
{
	delete ui;
}

void Countdown::changeEvent(QEvent* e)
{
	QWidget::changeEvent(e);

	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;

	default:
		break;
	}
}

void Countdown::timerEvent(QTimerEvent* event)
{
	if (event->timerId() == timer.timerId())
	{
		--m_seconds;

		if (m_seconds <= 0)
			accept();

		ui->scaledText->SetText(QString::number(m_seconds));
		update();
	}
	else
	{
		QWidget::timerEvent(event);
	}
}
