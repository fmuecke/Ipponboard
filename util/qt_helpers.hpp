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
