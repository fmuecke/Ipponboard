// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "FighterManager.h"
#include "../util/SimpleCsvFile.hpp"

#include <QObject>  // needed for tr()
#include <QStringList>
#include <algorithm>

using namespace Ipponboard;

char const* const FighterManager::str_FIRSTNAME = "@FIRSTNAME";
char const* const FighterManager::str_LASTNAME = "@LASTNAME";
char const* const FighterManager::str_CLUB = "@CLUB";
char const* const FighterManager::str_WEIGHT = "@WEIGHT";
char const* const FighterManager::str_CATEGORY = "@CATEGORY";

const std::array<char const* const, 5> FighterManager::Specifiers =
{
	str_FIRSTNAME,
	str_LASTNAME,
	str_CLUB,
	str_WEIGHT,
	str_CATEGORY
};

FighterManager::FighterManager()
	: m_fighters()
{
}

QString FighterManager::GetCsvHeaderFormat()
{
	QString retVal;

	for (auto const s : Specifiers)
	{
		if (!retVal.isEmpty())
		{
			retVal.append(Separator);
		}

		retVal.append(s);
	}

	return retVal;
}

bool FighterManager::LoadFighters(QString const& csvFile, QString& errorMsg)
{
	errorMsg.clear();

	m_fighters.clear();

	auto addResult = AddFighters(csvFile, errorMsg);

	if (m_fighters.empty())
	{
		Ipponboard::Fighter fighter(QObject::tr("FIRST NAME"), QObject::tr("LAST NAME"));
		m_fighters.insert(fighter);
	}

	return addResult;
}

bool FighterManager::AddFighters(QString const& csvFile, QString& errorMsg)
{
	errorMsg.clear();

	std::vector<QStringList> data;
	if (!fm::SimpleCsvFile::ReadItems(csvFile, &Separator, data, errorMsg))
	{
		return false;
	}

	if (data.empty() || data[0].join(&Separator) != GetCsvHeaderFormat())
	{
		errorMsg = QObject::tr("No fighters were added: file %1 does not have the proper header:\n\n%2").arg(csvFile, GetCsvHeaderFormat());
		return false;
	}

	const size_t oldCount = m_fighters.size();

	for (unsigned int i=1; i<data.size(); ++i)
	{
		auto line = data[i];
		if (line.count() != Specifiers.size())
		{
			errorMsg = QObject::tr("Line %1 of file %2 does not have the right number of fields").arg(i).arg(csvFile);
			return false;
		}

		Ipponboard::Fighter fighter(line[0], line[1]);
		fighter.club = line[2];
		fighter.weight = line[3];
		fighter.category = line[4];

		m_fighters.insert(fighter);
	}

	errorMsg = QObject::tr("Imported %1 new fighters.").arg(QString::number(m_fighters.size() - oldCount));

	return true;
}

bool FighterManager::SaveFighters(QString const& csvFile, QString& errorMsg)
{
	errorMsg.clear();

	QStringList data;
	data.push_back(GetCsvHeaderFormat());

	for (Ipponboard::Fighter const & f : m_fighters)
	{
		data.push_back(QString("%1;%2;%3;%4;%5").arg(f.first_name, f.last_name, f.club, f.weight, f.category));
	}

	if (!fm::SimpleCsvFile::WriteData(csvFile, data, errorMsg))
	{
		return false;
	}

	qDebug("Successfully saved %i fighters to %s.", m_fighters.size(), csvFile);
	return true;
}

Ipponboard::Fighter FighterManager::AddNewFighter()
{
	auto id = rand();
	Ipponboard::Fighter f(QString("first_name_%1").arg(id), QString("last_name_%1").arg(id));

	AddFighter(f);

	return f;
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
	auto filteredFighters = Filter(m_fighters, str_CLUB, club);

	QStringList ret;
	for (auto const& f : filteredFighters)
	{
		ret.append(QString("%1 %2").arg(f.first_name, f.last_name));
	};

	return ret;
}

FighterManager::FighterList FighterManager::Filter(FighterManager::FighterList const& fighters, QString const& specifier, QString const& value)
{
	FighterManager::FighterList resultSet;
	QString Fighter::*pMember = nullptr;

	if (specifier == FighterManager::str_CATEGORY)
	{
		pMember = &Fighter::category;
	}
	else if (specifier == FighterManager::str_CLUB)
	{
		pMember = &Fighter::club;
	}
	else if (specifier == FighterManager::str_FIRSTNAME)
	{
		pMember = &Fighter::first_name;
	}
	else if (specifier == FighterManager::str_WEIGHT)
	{
		pMember = &Fighter::weight;
	}
	else if (specifier == FighterManager::str_LASTNAME)
	{
		pMember = &Fighter::last_name;
	}
	else
	{
		// unknown specifier
		return resultSet; //TODO: maybe throw exception
	}

	for (auto const& f : fighters)
	{
		if ((f.*pMember) == value || (f.*pMember).isEmpty())
		{
			resultSet.insert(f);
		}
	}

	return resultSet;
}
