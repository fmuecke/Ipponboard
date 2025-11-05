// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef WIDGETS__SCALEDIMAGE_H_
#define WIDGETS__SCALEDIMAGE_H_

#include <QImage>
#include <QResizeEvent>
#include <QWidget>


class ScaledImage : public QWidget
{
    Q_OBJECT

  public:
    explicit ScaledImage(QWidget* parent = 0);

    void UpdateImage(const QString& fileName);
    void SetBgColor(const QColor& color);
    void paintEvent(QPaintEvent* pEvent);
    void resizeEvent(QResizeEvent* pEvent);

  private: //Data
    QImage m_Image;
    QSize m_Size;
    QColor m_BGColor;
};

#endif // WIDGETS__SCALEDIMAGE_H_
