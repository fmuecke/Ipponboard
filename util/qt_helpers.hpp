// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FM_QT_HELPERS_H
#define FM_QT_HELPERS_H

#include <string>
#include <QString>

namespace fm
{
    namespace qt
    {
        inline std::string to_utf8_str(QString const& qStr)
        {
#if QT_VERSION >= 0x050000
            return qStr.toStdString();
#else
            auto byteArray = qStr.toUtf8();
            std::string result(byteArray.constData(), byteArray.length());
            return result;
#endif
        }

        inline QString from_utf8_str(std::string const& str)
        {
#if QT_VERSION >= 0x050000
            return QString::fromStdString(str);
#else
            return QString::fromUtf8(str.c_str(), str.length());
#endif
        }

    }
}

#endif // FM_QT_HELPERS_H