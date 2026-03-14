// Copyright 2024 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__PCH_CORE_H_
#define BASE__PCH_CORE_H_

#include "pch_std.h"

#if defined __cplusplus

#if defined(QT_CORE_LIB)
#pragma warning(disable : 4127) // conditional expression is constant
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QTime>
#include <QTimer>
#include <QTranslator>
#include <QUrl>
#pragma warning(default : 4127)
#endif

#endif // __cplusplus

#endif // BASE__PCH_CORE_H_
