#include "ClubManager.h"
#include <QMessageBox>

#include <iostream>
#include <fstream>
#include "../util/path_helpers.h"
#include "../util/json.hpp"
#include "../util/qt_helpers.hpp"
#include <algorithm>


using namespace Ipponboard;
const char* const ClubManager::str_filename_club_definitions = "clubs\\clubs.json";

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
	// open the archive

	//m_Clubs.push_back(Club("TSV Königsbrunn", "..\\base\\emblems\\koenigsbrunn.png"));
	//m_Clubs.push_back(Club("TV Lenggries", "..\\base\\emblems\\tv_lenggries.png"));

	const std::string filePath(
		fm::GetSettingsFilePath(str_filename_club_definitions));

    try
    {
        auto const& jsonClubs = fm::Json::ReadFile(filePath.c_str());

        for (auto const& jsonClub : jsonClubs)
        {
            Club club;
            club.name = fm::qt::from_utf8_str(jsonClub["name"].asString());
            club.address = fm::qt::from_utf8_str(jsonClub["address"].asString());
            club.logoFile = fm::qt::from_utf8_str(jsonClub["image"].asString());

            m_Clubs.push_back(club);
        }
    }
    catch (fm::Json::Exception const& e)
    {
        QMessageBox::critical(0, QString("Error"), QString::fromStdString(e.what()));
    }
    catch(std::exception const& e)
    {
        QMessageBox::critical(0, QString("Internal error"), QString::fromStdString(e.what()));
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
	const std::string filePath(
		fm::GetSettingsFilePath(str_filename_club_definitions));

    fm::Json::Value jsonClubs;

    for (Club const& club : m_Clubs)
    {
        fm::Json::Value jsonClub;
        jsonClub["name"] = fm::qt::to_utf8_str(club.name);
        jsonClub["address"] = fm::qt::to_utf8_str(club.address);
        jsonClub["image"] = fm::qt::to_utf8_str(club.logoFile);

        jsonClubs.append(jsonClub);
    }

    try
    {
        fm::Json::WriteFile(filePath.c_str(), jsonClubs);
    }
    catch(fm::Json::Exception const& e)
    {
        QMessageBox::critical(0, QString("Error"), QString::fromStdString(e.what()));
	}
    catch(std::exception const& e)
    {
        QMessageBox::critical(0, QString("Internal error"), QString::fromStdString(e.what()));
    }

}
