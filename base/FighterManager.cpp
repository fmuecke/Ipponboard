// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
#include "FighterManager.h"
#include "../util/SimpleCsvFile.hpp"

#include <QObject>  // needed for tr()
#include <QStringList>
#include <algorithm>

using namespace Ipponboard;

char const* const FighterManager::str_firstname = "@firstname";
char const* const FighterManager::str_lastname = "@lastname";
char const* const FighterManager::str_club = "@club";
char const* const FighterManager::str_weight = "@weight";
char const* const FighterManager::str_category = "@category";
char const* const FighterManager::str_nation = "@nation";
char const* const FighterManager::str_year = "@year";

const std::array<char const* const, 7> FighterManager::Specifiers =
{
    str_lastname,
    str_firstname,
    str_club,
    str_weight,
    str_category,
    str_nation,
    str_year
};

QString FighterManager::DefaultExportFormat()
{
	QString ret;

    for (auto const& s :  Specifiers)
    {
		if (!ret.isEmpty())
		{
			ret.append(';');
		}

		ret.append(s);
	}

	return ret;
}


FighterManager::FighterManager()
	: m_fighters()
{
}

QString FighterManager::GetSpecifiererDescription()
{
	QString retVal;

    for (auto const& s :  Specifiers)
	{
		if (!retVal.isEmpty())
		{
			retVal.append(", ");
		}

		retVal.append(s);
	}

	return retVal;
}

// A satisfying format string must have at least tags for
// firstname and lastname
bool Ipponboard::FighterManager::IsFormatSatisfying(const QString& formatStr)
{
    return formatStr.contains(str_firstname) && formatStr.contains(str_lastname);
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

    auto beginPos = workStr.indexOf(str_firstname);
    auto endPos = beginPos + QString(str_firstname).length();

	// use second tag if is last tag in format specifier
	if (endPos == workStr.length())
	{
        beginPos = workStr.indexOf(str_lastname);
        endPos = beginPos + QString(str_lastname).length();
	}

	auto nextPos = workStr.indexOf("@", endPos);

	if (-1 == nextPos)
	{
		return false;
	}

	sep = workStr.mid(endPos, nextPos - endPos);

	return true;
}

bool FighterManager::ImportFighters(
    QString const& fileName,
    QString const& formatStr,
    QString const& encoding,
    QString& errorMsg)
{
    QString sep;

    if (!DetermineSeparator(formatStr, sep))
    {
        errorMsg = QObject::tr("Format specifier has invalid separator: %1")
                   .arg(formatStr);
        return false;
    }

    return ImportFighters(fileName, formatStr, sep, encoding, errorMsg);

}

bool FighterManager::ImportFighters(
    QString const& fileName,
    QString const& formatStr,
    QString const& separator,
    QString const& encoding,
    QString& errorMsg)
{
	errorMsg.clear();

	// determine tag positions
    const QStringList tags = formatStr.split(separator);
    const int firstnamePos = tags.indexOf(str_firstname);
    const int lastnamePos = tags.indexOf(str_lastname);
    const int clubPos = tags.indexOf(str_club);
    const int weightPos = tags.indexOf(str_weight);
    const int categoryPos = tags.indexOf(str_category);
    const int nationPos = tags.indexOf(str_nation);
    const int yearPos = tags.indexOf(str_year);

    if (-1 == firstnamePos || -1 == lastnamePos)
	{
		errorMsg = QObject::tr("Format specifier does not contain required fields: firstname, lastname");
		return false;
	}

	int minItemsPerLine = 2;

	std::vector<QStringList> data;

    if (!fmu::SimpleCsvFile::ReadItems(fileName, separator, data, minItemsPerLine, encoding, errorMsg))
	{
		return false;
	}

	//NO, don't do it: m_fighters.clear();

	const size_t oldCount = m_fighters.size();
    for (QStringList const & line : data)
	{
        Ipponboard::Fighter fighter(line[firstnamePos], line[lastnamePos]);
        fighter.club = -1 != clubPos ? line[clubPos] : QString();
        fighter.weight = -1 != weightPos ? line[weightPos] : QString();
        fighter.category = -1 != categoryPos ? line[categoryPos] : QString();
        fighter.nation = -1 != nationPos ? line[nationPos] : QString();
        fighter.year = -1 != yearPos ? line[yearPos] : QString();

		m_fighters.insert(fighter);
	}

	errorMsg = QObject::tr("Imported %1 new fighters.").arg(QString::number(m_fighters.size() - oldCount));

	return true;
}

bool FighterManager::ExportFighters(
	QString const& fileName,
	QString const& formatStr,
	QString& errorMsg)
{
	errorMsg.clear();

    QString separator;

    if (!DetermineSeparator(formatStr, separator))
	{
		errorMsg = QObject::tr("Format specifier has invalid separator: %1")
				   .arg(formatStr);

		return false;
	}

	if (!IsFormatSatisfying(formatStr))
	{
		errorMsg = QObject::tr("Format specifier does not meet criteria: %1")
				   .arg(formatStr);

		return false;
	}

	// determine tag positions
    const QStringList tags = formatStr.split(separator);
    const int firstNamePos = tags.indexOf(str_firstname);
    const int lastNamePos = tags.indexOf(str_lastname);
    const int clubPos = tags.indexOf(str_club);
    const int weightPos = tags.indexOf(str_weight);
    const int categoryPos = tags.indexOf(str_category);
    const int nationPos = tags.indexOf(str_nation);
    const int yearPos = tags.indexOf(str_year);

	QStringList data;
    for (Ipponboard::Fighter const & f : m_fighters)
	{
		QString line;

		for (int i = 0; i < static_cast<int>(tags.size()); ++i)
		{
			if (!line.isEmpty())
			{
                line.append(separator);
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
            else if (i == nationPos)
            {
                line.append(f.nation);
            }
            else if (i == yearPos)
            {
                line.append(f.year);
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

	errorMsg = QObject::tr("Successfully exported %1 fighters.")
			   .arg(QString::number(m_fighters.size()));

	return true;
}

bool FighterManager::AddFighter(Fighter f)
{
	return m_fighters.insert(f).second;
}

bool FighterManager::RemoveFighter(Fighter f)
{
	auto iter = std::find(begin(m_fighters), end(m_fighters), f);

	if (iter == end(m_fighters))
	{
		return false;
	}

	m_fighters.erase(iter);

	return true;
}

QStringList FighterManager::GetClubFighterNames(const QString& club) const
{
	QStringList ret;
	std::for_each(begin(m_fighters), end(m_fighters),
				  [&](Ipponboard::Fighter const & f)
	{
		if (f.club == club)
		{
			ret.append(QString("%1 %2").arg(f.first_name, f.last_name));
		}
	});

    return ret;
}
