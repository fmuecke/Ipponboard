// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "scaledtext.h"
#include <QtGui>
#include <QtDebug>
#include <algorithm>

ScaledText::ScaledText(QWidget* pParent)
	: QWidget(pParent)
	, m_Text()
	, m_TextColor(Qt::color1)  // normal foreground
	, m_BGColor(Qt::transparent)  // normal background (transparent)
	, m_Alignment(Qt::AlignCenter)
    , m_pLayout(nullptr)
	, m_timerId(-1)
	, m_timerDelay(0)
	, m_textSize(eSize_normal)
	, m_isVisible(true)
    , m_isRotated(false)
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

    m_isRotated = rotate;

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

        if (m_pLayout)
		{
			painter.setPen(m_TextColor);

			// move coordinate system to the center
			painter.translate(width() / 2.0, height() / 2.0);

			QTextLine line = m_pLayout->lineAt(0);
			line.setLeadingIncluded(false);

            const QRectF textRect = line.naturalTextRect();
            Q_ASSERT(textRect == m_pLayout->boundingRect());

            auto w = textRect.width();
            auto h = textRect.height();
            qreal adjust_y{0};

            if (!m_isRotated)
			{
                /*
                    |    <-- Ascent
                    |    <-- Ascent
                    |    <-- Ascent
                    |    <-- Ascent
                    |--- <-- Baseline
                    |    <-- Descent
                    |    <-- Descent

                    height = ascent + descent
                    example: height=19, ascent=15, descent=4
                */

                if (eSize_full == m_textSize || eSize_uppercase == m_textSize)
				{
                    //qDebug() << "rect.width:" << textRect.width() << " rect.height:" << textRect.height() << "descent:" << line.descent() << "ascent:" << line.ascent();

                    // remove descent space
                    h = line.ascent();     // h => 15

                    // make extra ascent space a little smaller (almost never used)
                    h -= line.descent() / 2.0;  // h => 13

                    // calc vertical adjustment
                    adjust_y -= line.descent() / line.ascent();  // 4/15 => 0,266
                }
				else
				{
                    adjust_y = line.descent() / line.ascent();
				}
            }

            // consider that italic text is a little wider than normal text
            const qreal italicFactor = this->fontInfo().italic() ? 1.03 : 1.0;
            qreal zoomFactor{0};

            if (m_isRotated)
            {
                painter.rotate(-60.0);
                zoomFactor = std::min<qreal>(width() / h, height() / italicFactor / w);
            }
            else
            {
                zoomFactor = std::min<qreal>(width() / italicFactor / w, height() / h);
            }

            QPointF center = textRect.center();

            if (Qt::AlignLeft == m_Alignment)
			{
                center.setX(width() / 2.0 / italicFactor / zoomFactor);
			}
			else if (Qt::AlignRight == m_Alignment)
			{
                center.setX(-width() / 2.0 / italicFactor / zoomFactor + center.x() * 2);
			}

            center.setY(center.y() + adjust_y);

            painter.scale(zoomFactor, zoomFactor);
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
