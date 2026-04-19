// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#include "../core/Athlete.h"

#include <QString>
#include <QStringList>
#include <array>
#include <set>

namespace Ipponboard
{

class AthleteManager
{
  public:
    AthleteManager();
    AthleteManager(const AthleteManager&) = delete;
    AthleteManager& operator=(const AthleteManager&) = delete;

    static char const* const str_FIRSTNAME;
    static char const* const str_LASTNAME;
    static char const* const str_CLUB;
    static char const* const str_WEIGHT;
    static char const* const str_CATEGORY;

    static const std::array<char const* const, 5> Specifiers;

    static QString GetSpecifierDescription();
    //static bool IsValidSpecifier(QString const& str);
    static bool IsFormatSatisfying(QString const& formatStr);
    static bool DetermineSeparator(QString const& str, QString& sep);

    static QString DefaultExportFormat();

    bool ImportAthletes(QString const& fileName, QString const& formatStr, QString& errorMsg);
    bool ExportAthletes(QString const& fileName, QString const& formatStr, QString& errorMsg);

    bool AddAthlete(Ipponboard::Athlete f);
    bool RemoveAthlete(Ipponboard::Athlete f);
    QStringList GetClubAthleteNames(QString const& filter) const;

    //private:
    std::set<Ipponboard::Athlete> m_athletes; //TODO: encapsulate
  private:
};

} // namespace Ipponboard
