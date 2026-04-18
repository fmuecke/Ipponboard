// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "FighterManager.h"

#include "../util/SimpleCsvFile.hpp"

#include <QObject> // needed for tr()
#include <QStringList>
#include <algorithm>

using namespace Ipponboard;

char const* const FighterManager::str_FIRSTNAME = "@FIRSTNAME";
char const* const FighterManager::str_LASTNAME = "@LASTNAME";
char const* const FighterManager::str_CLUB = "@CLUB";
char const* const FighterManager::str_WEIGHT = "@WEIGHT";
char const* const FighterManager::str_CATEGORY = "@CATEGORY";

const std::array<char const* const, 5> FighterManager::Specifiers = {
    str_FIRSTNAME, str_LASTNAME, str_CLUB, str_WEIGHT, str_CATEGORY
};

QString FighterManager::DefaultExportFormat()
{
    QString ret;

    for (char const* const s : Specifiers)
    {
        if (!ret.isEmpty())
        {
            ret.append(';');
        }

        ret.append(s);
    }

    return ret;
}

FighterManager::FighterManager() : m_athletes() {}

QString FighterManager::GetSpecifierDescription()
{
    QString retVal;

    for (char const* const s : Specifiers)
    {
        if (!retVal.isEmpty())
        {
            retVal.append(", ");
        }

        retVal.append(s);
    }

    return retVal;
}

//bool Ipponboard::FighterManager::IsValidSpecifier(const QString& str)
//{
//    for(char const * const s : Specifiers)
//    {
//        if (str.compare(s) == 0)
//            return true;
//    }

//    return false;
//}

// A Satisfying format string must have at least tags for
// firstname and lastname
bool Ipponboard::FighterManager::IsFormatSatisfying(const QString& formatStr)
{
    return formatStr.contains(str_FIRSTNAME) && formatStr.contains(str_LASTNAME);
}

// Format string must be at least "satisfying" to be processed
bool Ipponboard::FighterManager::DetermineSeparator(const QString& str, QString& sep)
{
    if (!IsFormatSatisfying(str))
    {
        return false;
    }

    sep.clear();
    QString workStr = str.trimmed();

    auto beginPos = workStr.indexOf(str_FIRSTNAME);
    auto endPos = beginPos + QString(str_FIRSTNAME).length();

    // use second tag if is last tag in format specifier
    if (endPos == workStr.length())
    {
        beginPos = workStr.indexOf(str_LASTNAME);
        endPos = beginPos + QString(str_LASTNAME).length();
    }

    auto nextPos = workStr.indexOf("@", endPos);

    if (-1 == nextPos)
    {
        return false;
    }

    sep = workStr.mid(endPos, nextPos - endPos);

    return true;
}

bool FighterManager::ImportFighters(QString const& fileName, QString const& formatStr,
                                    QString& errorMsg)
{
    errorMsg.clear();

    QString sep;

    if (!DetermineSeparator(formatStr, sep))
    {
        errorMsg = QObject::tr("Format specifier has invalid separator: %1").arg(formatStr);
        return false;
    }

    // determine tag positions
    const QStringList tags = formatStr.split(sep);
    const int firstNamePos = tags.indexOf(str_FIRSTNAME);
    const int lastNamePos = tags.indexOf(str_LASTNAME);
    const int clubPos = tags.indexOf(str_CLUB);
    const int weightPos = tags.indexOf(str_WEIGHT);
    const int categoryPos = tags.indexOf(str_CATEGORY);

    if (-1 == firstNamePos || -1 == lastNamePos)
    {
        errorMsg = QObject::tr("Format specifier does not contain firstname and lastname: %1")
                       .arg(formatStr);
        return false;
    }

    std::vector<QStringList> data;

    if (!fm::SimpleCsvFile::ReadItems(fileName, sep, data, errorMsg))
    {
        return false;
    }

    //NO, don't do it: m_athletes.clear();

    const size_t oldCount = m_athletes.size();

    for (QStringList const& line : data)
    {
        QString firstName = line[firstNamePos];
        QString lastName = line[lastNamePos];
        QString club = -1 != clubPos ? line[clubPos] : "";
        QString weight = -1 != weightPos ? line[weightPos] : "";
        QString category = -1 != categoryPos ? line[categoryPos] : "";

        Ipponboard::Athlete athlete(firstName, lastName);
        athlete.club = club;
        athlete.weight = weight;
        athlete.category = category;

        m_athletes.insert(athlete);
    }

    errorMsg =
        QObject::tr("Imported %1 new athletes.").arg(QString::number(m_athletes.size() - oldCount));

    return true;
}

bool FighterManager::ExportFighters(QString const& fileName, QString const& formatStr,
                                    QString& errorMsg)
{
    errorMsg.clear();

    QString sep;

    if (!DetermineSeparator(formatStr, sep))
    {
        errorMsg = QObject::tr("Format specifier has invalid separator: %1").arg(formatStr);

        return false;
    }

    if (!IsFormatSatisfying(formatStr))
    {
        errorMsg = QObject::tr("Format specifier does not meet criteria: %1").arg(formatStr);

        return false;
    }

    // determine tag positions
    const QStringList tags = formatStr.split(sep);
    const int firstNamePos = tags.indexOf(str_FIRSTNAME);
    const int lastNamePos = tags.indexOf(str_LASTNAME);
    const int clubPos = tags.indexOf(str_CLUB);
    const int weightPos = tags.indexOf(str_WEIGHT);
    const int categoryPos = tags.indexOf(str_CATEGORY);

    QStringList data;

    for (Ipponboard::Athlete const& f : m_athletes)
    {
        QString line;

        for (int i = 0; i < static_cast<int>(tags.size()); ++i)
        {
            if (!line.isEmpty())
            {
                line.append(sep);
            }

            if (i == firstNamePos)
            {
                line.append(f.first_name);
            }
            else if (i == lastNamePos)
            {
                line.append(f.last_name);
            }
            else if (i == clubPos)
            {
                line.append(f.club);
            }
            else if (i == weightPos)
            {
                line.append(f.weight);
            }
            else if (i == categoryPos)
            {
                line.append(f.category);
            }
            else
            {
                // empty tag
            }
        }

        data.push_back(line);
    }

    if (!fm::SimpleCsvFile::WriteData(fileName, data, errorMsg))
    {
        return false;
    }

    errorMsg =
        QObject::tr("Successfully exported %1 fighters.").arg(QString::number(m_athletes.size()));

    return true;
}

bool FighterManager::AddFighter(Athlete f) { return m_athletes.insert(f).second; }

bool FighterManager::RemoveFighter(Athlete f)
{
    auto iter = std::find(begin(m_athletes), end(m_athletes), f);

    if (iter == end(m_athletes))
    {
        return false;
    }

    m_athletes.erase(iter);

    return true;
}

QStringList FighterManager::GetClubFighterNames(const QString& club) const
{
    QStringList ret;
    std::for_each(begin(m_athletes),
                  end(m_athletes),
                  [&](Ipponboard::Athlete const& f)
                  {
                      if (f.club == club)
                      {
                          ret.append(QString("%1 %2").arg(f.first_name, f.last_name));
                      }
                  });

    return ret;
}
