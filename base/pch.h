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
#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <set>
#include <map>
#include <string>
#include <algorithm>
#include <functional>
#include <utility>
#include <memory>

//
// boost
//
#include <boost/utility.hpp>
#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS		// needed for msm
#define BOOST_MPL_LIMIT_VECTOR_SIZE	50			// needed for msm
#define BOOST_MPL_LIMIT_MAP_SIZE		50			// needed for msm
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>


//
// QT
//
#pragma warning (disable: 4127)  // conditional expression is constant
#include <QObject>
#include <QString>
#include <QApplication>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QSoundEffect>
#include <QFileInfo>
#include <QtGui/QApplication>
#include <QTranslator>
#include <QComboBox>
#include <QDesktopWidget>
#include <QFontDialog>
#include <QColorDialog>
#include <QInputDialog>
#include <QSettings>
#include <QTimer>
//#include <QSplashScreen>
#include <QPainter>
#include <QtGui>
#include <QStringList>
#include <QList>
#include <QVector>
#include <QSize>
#include <QTime>
#include <QFontDatabase>
#include <QMenu>
#include <QDialog>
#include <QMainWindow>
#include <QTableWidgetItem>
#include <QtGui/QWidget>
#include <QImage>
#include <QResizeEvent>
#include <QFont>
#include <QAbstractItemModel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QFile>
#include <QDir>
#pragma warning (default: 4127)

//
// custom
//
//#include "../util/qstring_serialization.h"


#endif // __cplusplus

#endif  // BASE__PCH_H_
