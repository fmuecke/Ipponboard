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
const char* const ClubManager::str_clubs_settingsFile = "clubs.toml";

//---------------------------------------------------------
ClubManager::ClubManager()
	: m_Clubs()
//---------------------------------------------------------
{
	LoadClubs_();
	SortClubs();
}

//---------------------------------------------------------
ClubManager::~ClubManager()
//---------------------------------------------------------
{
	SaveClubs_();
}

//---------------------------------------------------------
bool ClubManager::GetClub(int index, Ipponboard::Club& club) const
//---------------------------------------------------------
{
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
	m_Clubs.push_back(club);
}

//---------------------------------------------------------
void ClubManager::UpdateClub(unsigned int index, const Ipponboard::Club& club)
//---------------------------------------------------------
{
	m_Clubs.at(index) = club;
}

//---------------------------------------------------------
void ClubManager::RemoveClub(unsigned int index)
//---------------------------------------------------------
{
	Q_ASSERT(index < m_Clubs.size() && index >= 0);

	m_Clubs.erase(m_Clubs.begin() + index);
}

//---------------------------------------------------------
void ClubManager::SortClubs()
//---------------------------------------------------------
{
	std::sort(m_Clubs.begin(), m_Clubs.end());
}

//---------------------------------------------------------
void ClubManager::LoadClubs_()
//---------------------------------------------------------
{
	auto config = fm::GetSettingsFilePath(str_clubs_settingsFile);

	try
	{
		if (QFile::exists(config.c_str()))
		{
			m_Clubs = ClubParser::ParseTomlFile(config.c_str());
		}
		else
		{
		// load legacy settings for conversion
			auto legacyConfig = fm::GetSettingsFilePath(str_legacy_filename_club_definitions);
			m_Clubs = ClubParser::ParseJsonFile(legacyConfig.c_str());
		}
	}
	catch (std::exception const& e)
	{
		QMessageBox::critical(
					0,
					"Unable to load clubs",
					QString("Error reading clubs from file '%1'.\n\n%2").arg(config.c_str(), e.what()));
	}
}

//---------------------------------------------------------
void ClubManager::SaveClubs_()
//---------------------------------------------------------
{
	if (m_Clubs.empty())
	{
		return;
	}

	// make an archive
	auto filePath = fm::GetSettingsFilePath(str_clubs_settingsFile);
	try
	{
		ClubParser::ToTomlFile(filePath.c_str(), m_Clubs);
	}
	catch (std::exception const& e)
	{
		QMessageBox::critical(
					0,
					"Unable to save clubs",
					QString("Error saving clubs to file '%1'.\n\n%2").arg(filePath.c_str(), e.what()));
	}
}
