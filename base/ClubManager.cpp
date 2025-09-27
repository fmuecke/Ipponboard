// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ClubManager.h"

#include "../util/path_helpers.h"
#include "ClubParser.h"

#include <QFile>
#include <QMessageBox>
#include <algorithm>

using namespace Ipponboard;
const char* const ClubManager::str_legacy_filename_club_definitions =
    "clubs.json"; // used up to version 1.10
const char* const ClubManager::str_clubs_settingsFile = "clubs.config";

//---------------------------------------------------------
ClubManager::ClubManager() : m_Clubs()
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

    if (QFile::exists(config))
    {
        m_Clubs = ClubParser::ParseIniFile(config);
    }
    else
    {
        // load legacy settings for conversion
        qWarning() << QString("%1 missing. Trying to load legacy %2")
                          .arg(str_clubs_settingsFile)
                          .arg(str_legacy_filename_club_definitions);
        try
        {
            auto legacyConfig = fm::GetSettingsFilePath(str_legacy_filename_club_definitions);
            m_Clubs = ClubParser::ParseJsonFile(legacyConfig);
        }
        catch (std::exception const& e)
        {
            QMessageBox::critical(
                nullptr, QString("Internal error"), QString::fromStdString(e.what()));
        }
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
        ClubParser::ToIniFile(filePath, m_Clubs);
    }
    catch (std::exception const& e)
    {
        QMessageBox::critical(nullptr, QString("Internal error"), QString::fromStdString(e.what()));
    }
}
