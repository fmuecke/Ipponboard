// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ScaledImage.h"

#include <QDebug>
#include <QFile>
#include <QPainter>

namespace
{
QString normalizeResourcePath(const QString& fileName)
{
    if (fileName.startsWith(QStringLiteral(":/")))
    {
        return fileName;
    }

    return fileName;
}
} // namespace

ScaledImage::ScaledImage(QWidget* pParent)
    : QWidget(pParent), m_Size(0, 0), m_BGColor(Qt::transparent) // normal background (transparent)
{
    UpdateImage(QStringLiteral(":/images/off.png"));
}

void ScaledImage::UpdateImage(const QString& fileName)
{
    const QString normalized = normalizeResourcePath(fileName);
    QImage newImage;

    if (!newImage.load(normalized))
    {
        qWarning() << "ScaledImage: failed to load" << normalized;
        return;
    }

    m_Image = std::move(newImage);
    update();
}

void ScaledImage::SetBgColor(const QColor& color)
{
    m_BGColor = color;
    update();
}

void ScaledImage::paintEvent(QPaintEvent* event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), QBrush(m_BGColor));
    QPoint centerPoint(0, 0);

    // Scale new image
    QImage scaledImage = m_Image.scaled(m_Size, Qt::KeepAspectRatio, Qt::SmoothTransformation);

    // Calculate image center position into screen
    centerPoint.setX((m_Size.width() - scaledImage.width()) / 2);
    centerPoint.setY((m_Size.height() - scaledImage.height()) / 2);

    // Draw image
    painter.drawImage(centerPoint, scaledImage);
}

void ScaledImage::resizeEvent(QResizeEvent* pEvent)
{
    m_Size = pEvent->size();

    // Call base class impl
    QWidget::resizeEvent(pEvent);
}
