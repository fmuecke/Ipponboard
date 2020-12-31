// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <string>
#include <QString>

namespace fm
{
    namespace qt
    {
        static std::string to_utf8_str(QString const& qStr)
        {
            auto byteArray = qStr.toUtf8();
            std::string result(byteArray.constData(), byteArray.length());
            return result;
        }

        static QString from_utf8_str(std::string const& str)
        {
            return QString::fromUtf8(str.c_str(), str.length());
        }

    }
}
