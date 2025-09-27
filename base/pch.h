// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__PCH_H_
#define BASE__PCH_H_

/* Add C includes here */

#if defined __cplusplus
/* Add C++ includes here */

//
// STL
//
#include <algorithm>
#include <array>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

//
// boost
//
#include "../core/MsmIncludes.h"

#include <boost/utility.hpp>

//
// QT
//
#pragma warning(disable : 4127) // conditional expression is constant
#include <QApplication>
#include <QColorDialog>
#include <QComboBox>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QFileInfo>
#include <QFontDialog>
#include <QInputDialog>
#include <QLabel>
#include <QMessageBox>
#include <QObject>
#include <QPushButton>
#include <QSettings>
#include <QSoundEffect>
#include <QString>
#include <QTimer>
#include <QTranslator>
#include <QtGui/QApplication>
//#include <QSplashScreen>
#include <QAbstractItemModel>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFont>
#include <QFontDatabase>
#include <QImage>
#include <QLineEdit>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QSize>
#include <QStringList>
#include <QTableWidgetItem>
#include <QTime>
#include <QVector>
#include <QtGui/QWidget>
#include <QtGui>
#pragma warning(default : 4127)

//
// custom
//
//#include "../util/qstring_serialization.h"

#endif // __cplusplus

#endif // BASE__PCH_H_
