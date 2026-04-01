// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef UTIL__THEME_COLORS_H_
#define UTIL__THEME_COLORS_H_

#include <QApplication>
#include <QColor>
#include <QPalette>
#include <QWidget>

namespace fm
{

inline QColor GetThemeTextColor(const QWidget* widget)
{
    return QApplication::palette(widget).color(QPalette::Text);
}

inline QColor GetValidationErrorColor(const QWidget* widget)
{
    const auto palette = QApplication::palette(widget);
    const bool darkBase = palette.color(QPalette::Base).lightness() < 128;
    return darkBase ? QColor(255, 128, 128) : QColor(176, 0, 32);
}

} // namespace fm

#endif // UTIL__THEME_COLORS_H_
