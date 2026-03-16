// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef CLUBPARSER_H
#define CLUBPARSER_H

#include "Club.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <stdexcept>

namespace ClubParser
{

namespace Tags
{
static const char* Address = "Address";
static const char* LogoFile = "LogoFile";
} // namespace Tags

static Ipponboard::ClubList ParseIniFile(QString filePath)
{
    QSettings settings(filePath, QSettings::IniFormat);

    Ipponboard::ClubList clubs;

    for (auto const& group : settings.childGroups())
    {
        settings.beginGroup(group);
        Ipponboard::Club club;
        club.name = group;

        if (settings.contains(Tags::Address))
        {
            club.address = settings.value(Tags::Address).toString();
        }

        if (settings.contains(Tags::LogoFile))
        {
            club.logoFile = settings.value(Tags::LogoFile).toString();
        }

        settings.endGroup();

        clubs.push_back(club);
    }

    return clubs;
}

static void ToIniFile(QString filePath, Ipponboard::ClubList const& clubs)
{
    QSettings settings(filePath, QSettings::IniFormat);

    settings.clear();

    for (auto const& club : clubs)
    {
        settings.beginGroup(club.name);
        settings.setValue(Tags::Address, club.address);
        settings.setValue(Tags::LogoFile, club.logoFile);
        settings.endGroup();
    }
}

} // namespace ClubParser

#endif // CLUBPARSER_H
