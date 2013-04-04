// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
// $Id: splashscreen.cpp 215 2011-11-01 12:28:21Z fmuecke $
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
