// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/debug.h"
#include "ScaledImage.h"
#include <QPainter>


ScaledImage::ScaledImage(QWidget* pParent)
	: QWidget(pParent)
	, m_Size(0, 0)
	, m_BGColor(Qt::transparent)    // normal background (transparent)
{
    TRACE(6, "ScaledImage::ScaledImage()");
    m_Image.load("off.png");
}

void ScaledImage::UpdateImage(const QString& fileName)
{
    TRACE(6, "ScaledImage::UpdateImage(fileName=%s)", fileName.toUtf8().data());
    m_Image.load(fileName);
	update();
}

void ScaledImage::SetBgColor(const QColor& color)
{
    TRACE(6, "ScaledImage::SetBgColor()");
    m_BGColor = color;
	update();
}

void ScaledImage::paintEvent(QPaintEvent* event)
{
    TRACE(6, "ScaledImage::paintEvent()");
    QPainter painter(this);
	painter.fillRect(event->rect(), QBrush(m_BGColor));
	QPoint centerPoint(0, 0);

	// Scale new image
	QImage scaledImage = m_Image.scaled(
							 m_Size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

	// Calculate image center position into screen
	centerPoint.setX((m_Size.width() - scaledImage.width()) / 2);
	centerPoint.setY((m_Size.height() - scaledImage.height()) / 2);

	// Draw image
	painter.drawImage(centerPoint, scaledImage);
}

void ScaledImage::resizeEvent(QResizeEvent* pEvent)
{
    TRACE(6, "ScaledImage::resizeEvent()");
    m_Size = pEvent->size();

	// Call base class impl
	QWidget::resizeEvent(pEvent);
}
