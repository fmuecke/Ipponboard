// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
#include "FighterManager.h"
#include "../util/SimpleCsvFile.hpp"

#include <QObject>  // needed for tr()
#include <boost/foreach.hpp>
#include <algorithm>


using namespace Ipponboard;

char const * const FighterManager::str_FIRSTNAME = "@FIRSTNAME";
char const * const FighterManager::str_LASTNAME = "@LASTNAME";
char const * const FighterManager::str_CLUB = "@CLUB";
char const * const FighterManager::str_WEIGHT = "@WEIGHT";
char const * const FighterManager::str_CATEGORY = "@CATEGORY";

const std::array<char const * const, 5> FighterManager::Specifiers =
{
    str_FIRSTNAME,
    str_LASTNAME,
    str_CLUB,
    str_WEIGHT,
    str_CATEGORY
};

FighterManager::FighterManager()
    : m_fighters()
    , m_exportFormat()
{
    BOOST_FOREACH(char const * const s, Specifiers)
    {
        if (!m_exportFormat.isEmpty())
        {
            m_exportFormat.append(';');
        }

        m_exportFormat.append(s);
    }
}

QString FighterManager::GetSpecifiererDescription()
{
    QString retVal;

    //TODO: use for_each
    BOOST_FOREACH(char const * const s, Specifiers)
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
//    BOOST_FOREACH(char const * const s, Specifiers)
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

    sep = workStr.mid(endPos, nextPos-endPos);

    return true;
}

bool FighterManager::ImportFighters(QString const& fileName, QString &errorMsg)
{
    errorMsg.clear();

    QString sep;
    if (!DetermineSeparator(GetExportFormat(), sep))
    {
        errorMsg = QObject::tr("Format specifier hat invalid separator: %1").arg(GetExportFormat());
        return false;
    }

    // determine tag positions
    const QStringList tags = GetExportFormat().split(sep);
    const int firstNamePos = tags.indexOf(str_FIRSTNAME);
    const int lastNamePos = tags.indexOf(str_LASTNAME);
    const int clubPos = tags.indexOf(str_CLUB);
    const int weightPos = tags.indexOf(str_WEIGHT);
    const int categoryPos = tags.indexOf(str_CATEGORY);

    if (-1 == firstNamePos || -1 == lastNamePos)
    {
        errorMsg = QObject::tr("Format specifier does not contain firstname and lastname: %1").arg(GetExportFormat());
        return false;

    }

    std::vector<QStringList> data;
    if (!fmu::SimpleCsvFile::ReadItems(fileName, sep, data, errorMsg))
    {
       return false;
    }

    //NO, don't do it: m_fighters.clear();

    const size_t oldCount = m_fighters.size();
    BOOST_FOREACH(QStringList const& line, data)
    {
        QString firstName = line[firstNamePos];
        QString lastName = line[lastNamePos];
        QString club = -1 != clubPos ? line[clubPos] : club;
        QString weight = -1 != weightPos ? line[weightPos] : weight;
        QString category = -1 != categoryPos ? line[categoryPos] : category;

        Ipponboard::Fighter fighter(firstName, lastName, club, weight, category);
        m_fighters.push_back(fighter);
    }

    errorMsg = QObject::tr("Successfully imported %1 fighters.").arg(QString::number(m_fighters.size()-oldCount));

    return true;
}

bool FighterManager::ExportFighters(QString const& fileName, QString& errorMsg)
{
    errorMsg.clear();

    QString sep;
    if (!DetermineSeparator(GetExportFormat(), sep))
    {
        errorMsg = QObject::tr("Format specifier hat invalid separator: %1")
                .arg(GetExportFormat());

        return false;
    }

    if (!IsFormatSatisfying(GetExportFormat()))
    {
        errorMsg = QObject::tr("Format specifier does not meet criteria: %1")
                .arg(GetExportFormat());

        return false;
    }

    // determine tag positions
    const QStringList tags = GetExportFormat().split(sep);
    const int firstNamePos = tags.indexOf(str_FIRSTNAME);
    const int lastNamePos = tags.indexOf(str_LASTNAME);
    const int clubPos = tags.indexOf(str_CLUB);
    const int weightPos = tags.indexOf(str_WEIGHT);
    const int categoryPos = tags.indexOf(str_CATEGORY);

    QStringList data;
    BOOST_FOREACH(Ipponboard::Fighter const& f, m_fighters)
    {
        QString line;
        for (int i=0; i < static_cast<int>(tags.size()); ++i)
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
                line.append(f.weight_class);
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

    if (!fmu::SimpleCsvFile::WriteData(fileName, data, errorMsg))
    {
        return false;
    }

    errorMsg = QObject::tr("Successfully exported %1 fighters.").arg(QString::number(m_fighters.size()));
    return true;
}
