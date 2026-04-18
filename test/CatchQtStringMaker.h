#pragma once

#include <QString>
#include <catch2/catch_tostring.hpp>

namespace Catch
{
template<>
struct StringMaker<QString>
{
    static std::string convert(const QString& value)
    {
        return value.toUtf8().toStdString();
    }
};
} // namespace Catch
