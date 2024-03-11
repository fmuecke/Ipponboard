// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/debug.h"
#include "ClubManager.h"
#include "ClubParser.h"
#include "../util/path_helpers.h"
#include "../util/json.hpp"
#include "../util/qt_helpers.hpp"
#include <QFile>
#include <QMessageBox>
#include <algorithm>
#include <iostream>
#include <fstream>


using namespace Ipponboard;
const char* const ClubManager::str_legacy_filename_club_definitions = "clubs.json";
const char* const ClubManager::str_clubs_settingsFile = "clubs.config";

//---------------------------------------------------------
ClubManager::ClubManager()
	: m_Clubs()
//---------------------------------------------------------
{
    TRACE(2, "ClubManager::ClubManager()");
    LoadClubs();
	SortClubs();
}

//---------------------------------------------------------
ClubManager::~ClubManager()
//---------------------------------------------------------
{
    TRACE(2, "ClubManager::~ClubManager()");
    SaveClubs();
}

//---------------------------------------------------------
bool ClubManager::GetClub(int index, Ipponboard::Club& club) const
//---------------------------------------------------------
{
    TRACE(2, "ClubManager::GetClub()");
    try
	{
		club = m_Clubs.at(index);
	}
	catch (std::out_of_range&)
	{
		return false;
	}

	return true;
}

//---------------------------------------------------------
QString ClubManager::GetLogo(const QString& clubName) const
//---------------------------------------------------------
{
    TRACE(2, "ClubManager::GetLogo(%s)", clubName.toUtf8().data());
    for (unsigned i(0); i < m_Clubs.size(); ++i)
	{
		if (clubName == m_Clubs.at(i).name)
			return m_Clubs.at(i).logoFile;
	}

	return QString();
}

//---------------------------------------------------------
QString ClubManager::GetAddress(const QString& clubName) const
//---------------------------------------------------------
{
    TRACE(2, "ClubManager::GetAddress(%s)", clubName.toUtf8().data());
    for (unsigned i(0); i < m_Clubs.size(); ++i)
	{
		if (clubName == m_Clubs.at(i).name)
			return m_Clubs.at(i).address;
	}

	return QString();
}

//---------------------------------------------------------
void ClubManager::AddClub(const Ipponboard::Club& club)
//---------------------------------------------------------
{
    TRACE(2, "ClubManager::AddClub()");
    m_Clubs.push_back(club);
}

//---------------------------------------------------------
void ClubManager::UpdateClub(unsigned int index, const Ipponboard::Club& club)
//---------------------------------------------------------
{
    TRACE(2, "ClubManager::UpdateClub()");
    m_Clubs.at(index) = club;
}

//---------------------------------------------------------
void ClubManager::RemoveClub(unsigned int index)
//---------------------------------------------------------
{
    TRACE(2, "ClubManager::RemoveClub()");
    Q_ASSERT(index < m_Clubs.size() && index >= 0);

	m_Clubs.erase(m_Clubs.begin() + index);
}

//---------------------------------------------------------
void ClubManager::SortClubs()
//---------------------------------------------------------
{
    TRACE(2, "ClubManager::SortClubs()");
    std::sort(m_Clubs.begin(), m_Clubs.end());
}

//---------------------------------------------------------
void ClubManager::LoadClubs()
//---------------------------------------------------------
{
    TRACE(2, "ClubManager::LoadClubs()");
    auto config = fm::GetSettingsFilePath(str_clubs_settingsFile);

	if (QFile::exists(config.c_str()))
	{
		m_Clubs = ClubParser::ParseIniFile(config.c_str());
	}
	else
	{
		// load legacy settings for conversion
		try
		{
			auto legacyConfig = fm::GetSettingsFilePath(str_legacy_filename_club_definitions);
			m_Clubs = ClubParser::ParseJsonFile(legacyConfig.c_str());
		}
		catch (std::exception const& e)
		{
			QMessageBox::critical(0, QString("Internal error"), QString::fromStdString(e.what()));
		}
	}
}

//---------------------------------------------------------
void ClubManager::SaveClubs()
//---------------------------------------------------------
{
    TRACE(2, "ClubManager::SaveClubs()");
    if (m_Clubs.empty())
	{
		return;
	}

	// make an archive
	auto filePath = fm::GetSettingsFilePath(str_clubs_settingsFile);

	try
	{
		ClubParser::ToIniFile(filePath.c_str(), m_Clubs);
	}
	catch (std::exception const& e)
	{
		QMessageBox::critical(0, QString("Internal error"), QString::fromStdString(e.what()));
	}

}
