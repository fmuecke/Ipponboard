// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__PCH_H_
#define BASE__PCH_H_

#include "pch_core.h"

#if defined __cplusplus

#if defined(QT_WIDGETS_LIB)
#pragma warning(disable : 4127) // conditional expression is constant
#include <QApplication>
#include <QComboBox>
#include <QDesktopServices>
#include <QDialog>
#include <QFileDialog>
#include <QGuiApplication>
#include <QInputDialog>
#include <QLabel>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#if defined(QT_MULTIMEDIA_LIB)
#include <QSoundEffect>
#endif
#include <QTableWidgetItem>
#include <QWidget>
#pragma warning(default : 4127)
#endif

#endif // __cplusplus

#endif // BASE__PCH_H_
