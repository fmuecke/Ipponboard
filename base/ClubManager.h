// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__CLUBMANAGER_H_
#define BASE__CLUBMANAGER_H_

#include "Club.h"

// forwards
class QListWidgetItem;

namespace Ipponboard
{

class ClubManager
{
    //Q_OBJECT
  public:
    static const char* const str_legacy_filename_club_definitions; // used prior to version 1.10
    static const char* const str_clubs_settingsFile;

    ClubManager();
    virtual ~ClubManager();

    bool GetClub(int index, Ipponboard::Club& club) const;
    QString GetLogo(const QString& clubName) const;
    QString GetAddress(const QString& clubName) const;

    // overwrites an existing club!
    void AddClub(const Ipponboard::Club& club);
    void UpdateClub(unsigned int index, const Ipponboard::Club& club);
    void RemoveClub(unsigned int index);
    void SortClubs();
    int ClubCount() const { return m_Clubs.size(); }

  private:
    void LoadClubs_();
    void SaveClubs_();

    ClubList m_Clubs;
};

} // namespace Ipponboard

#endif // BASE__CLUBMANAGER_H_
