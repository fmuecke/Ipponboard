// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ContestCategoryParser.h"

#include <QFile>
#include <QSettings>
#include <stdexcept>

namespace Tags
{
static const char* const Weights = "Weights";
static const char* const RoundTime = "RoundTimeInSecs";
static const char* const GoldenScoreTime = "GoldenScoreTimeInSecs";
} // namespace Tags

ContestCategoryParser::ContestCategoryParser() {}

void ContestCategoryParser::ToIniFile(const QString& file,
                                      Ipponboard::CategoryList const& categories)
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

Ipponboard::CategoryList ContestCategoryParser::ParseIniFile(QString const& file)
{
    QSettings settings(file, QSettings::IniFormat);

    Ipponboard::CategoryList categories;

    for (auto const& group : settings.childGroups())
    {
        Ipponboard::ContestCategory cat(group);
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