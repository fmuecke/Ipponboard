// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "FightCategoryParser.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QSettings>
#include <stdexcept>

namespace Tags
{
static const char* const Weights = "Weights";
static const char* const RoundTime = "RoundTimeInSecs";
static const char* const GoldenScoreTime = "GoldenScoreTimeInSecs";
} // namespace Tags

FightCategoryParser::FightCategoryParser() {}

void FightCategoryParser::ToIniFile(const QString& file,
                                    Ipponboard::FightCategoryList const& categories)
{
    QSettings settings(file, QSettings::IniFormat);
    settings.clear();

    for (auto const& cat : categories)
    {
        settings.beginGroup(cat.ToString());
        {
            settings.setValue(Tags::RoundTime, cat.GetRoundTime());
            settings.setValue(Tags::GoldenScoreTime, cat.GetGoldenScoreTime());
            settings.setValue(Tags::Weights, cat.GetWeights());
        }
        settings.endGroup();
    }
}

Ipponboard::FightCategoryList FightCategoryParser::ParseIniFile(QString const& file)
{
    QSettings settings(file, QSettings::IniFormat);

    Ipponboard::FightCategoryList categories;

    for (auto const& group : settings.childGroups())
    {
        Ipponboard::FightCategory cat(group);
        settings.beginGroup(group);
        {
            if (settings.contains(Tags::RoundTime))
            {
                auto secs = settings.value(Tags::RoundTime).toInt();
                cat.SetRoundTime(secs);
            }

            if (settings.contains(Tags::GoldenScoreTime))
            {
                auto secs = settings.value(Tags::GoldenScoreTime).toInt();
                cat.SetGoldenScoreTime(secs);
            }

            if (settings.contains(Tags::Weights))
            {
                auto val = settings.value(Tags::Weights).toString();
                cat.SetWeights(val);
            }
        }
        settings.endGroup();

        categories.push_back(cat);
    }

    return categories;
}

Ipponboard::FightCategoryList ParseJson(const QJsonArray& json)
{
    Ipponboard::FightCategoryList categories;

    for (const auto& jsonCat : json)
    {
        const auto object = jsonCat.toObject();
        Ipponboard::FightCategory cat(object.value(QStringLiteral("name")).toString());
        cat.SetRoundTime(object.value(QStringLiteral("round_time_secs")).toInt());
        cat.SetGoldenScoreTime(object.value(QStringLiteral("golden_score_time_secs")).toInt());
        cat.SetWeights(object.value(QStringLiteral("weights")).toString());

        categories.push_back(cat);
    }

    return categories;
}

// May throw exception!
Ipponboard::FightCategoryList FightCategoryParser::ParseJsonFile(const QString& file)
{
    QFile legacyFile(file);
    if (!legacyFile.open(QIODevice::ReadOnly))
    {
        throw std::runtime_error(
            QStringLiteral("Unable to open legacy fight categories '%1'").arg(file).toStdString());
    }

    QJsonParseError parseError{};
    const auto document = QJsonDocument::fromJson(legacyFile.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError)
    {
        throw std::runtime_error(QStringLiteral("Invalid fight categories JSON '%1': %2")
                                     .arg(file, parseError.errorString())
                                     .toStdString());
    }

    if (!document.isArray())
    {
        throw std::runtime_error(
            QStringLiteral("Expected fight category array in '%1'").arg(file).toStdString());
    }

    return ParseJson(document.array());
}
