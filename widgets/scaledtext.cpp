// Copyright 2010-2014 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
// $Id$
#include "scaledtext.h"
#include <QtGui>
#include <algorithm>

ScaledText::ScaledText(QWidget* pParent)
	: QWidget(pParent)
	, m_Text()
	, m_TextColor(Qt::color1)  // normal foreground
	, m_BGColor(Qt::transparent)  // normal background (transparent)
	, m_Alignment(Qt::AlignCenter)
	, m_pLayout(0)
	, m_timerId(-1)
	, m_timerDelay(0)
	, m_textSize(eSize_normal)
	, m_isVisible(true)
	, m_rotate(false)
{
	SetText("ScaledText");
}

void ScaledText::SetText(const QString& text,
						 ETextSize size,
						 bool rotate)
{
	set_size(size);

	if (eSize_uppercase == m_textSize)
		m_Text = text.toUpper();
	else
		m_Text = text;

	m_rotate = rotate;

	update_text_metrics();

	Redraw();
}

void ScaledText::SetFont(const QFont& font)
{
	this->setFont(font);
	update_text_metrics();

	Redraw();
}

void ScaledText::SetFontAndColor(const QFont& font, const QColor& textColor,
								 const QColor& bgColor)
{
	SetFont(font);
	SetColor(textColor, bgColor);
}


void ScaledText::SetColor(const QColor& textColor, const QColor& bgColor)
{
	m_TextColor = textColor;
	m_BGColor = bgColor;

	Redraw();
}

void ScaledText::SetColor(const QColor& textColor)
{
	SetColor(textColor, m_BGColor);
}

const QColor& ScaledText::GetColor() const
{
	return m_TextColor;
}

const QColor& ScaledText::GetBgColor() const
{
	return m_BGColor;
}

void ScaledText::set_size(ETextSize size)
{
	Q_ASSERT(size < eSize_MAX && size >= 0);
	m_textSize = size;
}

ScaledText::ETextSize ScaledText::GetSize() const
{
	return m_textSize;
}

void ScaledText::SetBlinking(bool blink, int delay)
{
	if (blink && -1 != m_timerId && delay == m_timerDelay)
		return;

	// kill old timer
	if (-1 != m_timerId)
		killTimer(m_timerId);

	m_timerId = -1;
	m_timerDelay = delay;
	m_isVisible = true;

	if (blink)
		m_timerId = startTimer(delay);
}

bool ScaledText::IsBlinking() const
{
	return (m_timerId != -1);
}

void ScaledText::Redraw()
{
	update();
}

void ScaledText::paintEvent(QPaintEvent* event)
{
	QPainter painter(this);
	painter.save();

	// erase background
	painter.fillRect(event->rect(), QBrush(m_BGColor));

	if (m_isVisible)
	{
		painter.setRenderHint(QPainter::TextAntialiasing);

		if (0 != m_pLayout)
		{
			painter.setPen(m_TextColor);

			// move coordinate system to the center
			painter.translate(width() / 2.0, height() / 2.0);

			QTextLine line = m_pLayout->lineAt(0);
			line.setLeadingIncluded(false);

			const QRectF rect = line.naturalTextRect();
			Q_ASSERT(rect == m_pLayout->boundingRect());

			qreal w = m_rotate ? rect.height() : rect.width();
			qreal h = m_rotate ? rect.width() : rect.height();

			qreal adjust_y(0);

			if (!m_rotate)
			{
				if (eSize_full == m_textSize)
				{
					h -= line.descent() * 2 + 0.5;
				}
				else if (eSize_uppercase == m_textSize)
				{
					h -= line.descent() + 2;
					adjust_y = -line.descent() / 2.5;
				}
				else
				{
					//adjust_y = line.descent()/2.0;
				}
			}

			const qreal zoom = std::min<qreal>(width() / w, height() / h);

			if (m_rotate)
				painter.rotate(-60.0);

			QPointF center = rect.center();

			if (Qt::AlignLeft == m_Alignment)
			{
				center.setX(width() / 2.0 / zoom);
			}
			else if (Qt::AlignRight == m_Alignment)
			{
				center.setX(-width() / 2.0 / zoom + center.x() * 2);
			}

			center.setY(center.y() + adjust_y);

			painter.scale(zoom, zoom);
			line.draw(&painter, -center);
		}
	}

	painter.restore();
	painter.end();
}

void ScaledText::timerEvent(QTimerEvent* /*event*/)
{
	m_isVisible = !m_isVisible;

	Redraw();
}

void ScaledText::update_text_metrics()
{
	const QFont& currentFont = this->font();

	delete m_pLayout;
	m_pLayout = new QTextLayout(m_Text, currentFont);
	m_pLayout->setTextOption(QTextOption(m_Alignment));
	m_pLayout->setCacheEnabled(true);
	m_pLayout->beginLayout();
	m_pLayout->createLine();
	m_pLayout->endLayout();

	Redraw();
}
