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

static Ipponboard::ClubList ParseJsonFile(QString filePath)
{
    Ipponboard::ClubList clubs;

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        throw std::runtime_error(
            QStringLiteral("Unable to open club configuration '%1'").arg(filePath).toStdString());
    }

    QJsonParseError parseError{};
    const auto document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        throw std::runtime_error(QStringLiteral("Invalid club JSON '%1': %2")
                                     .arg(filePath, parseError.errorString())
                                     .toStdString());
    }

    if (!document.isArray())
    {
        throw std::runtime_error(
            QStringLiteral("Expected JSON array in '%1'").arg(filePath).toStdString());
    }

    const auto jsonClubs = document.array();

    for (auto const& jsonClub : jsonClubs)
    {
        const auto clubObject = jsonClub.toObject();
        Ipponboard::Club club;
        club.name = clubObject.value(QStringLiteral("name")).toString();
        club.address = clubObject.value(QStringLiteral("address")).toString();
        club.logoFile = clubObject.value(QStringLiteral("image")).toString();

        clubs.push_back(club);
    }

    return clubs;
}

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
