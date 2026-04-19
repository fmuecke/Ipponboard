// Copyright 2018 Florian Muecke.
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "../core/CompetitionMode.h"
#include "../core/Contest.h"

#include <QColor>
#include <QJsonDocument>
#include <QString>
#include <vector>

namespace Ipponboard::CompetitionSerialization
{
inline constexpr const char* CompetitionSaveFileVersion = "3";
inline constexpr const char* AutoSaveFilename = "Ipponboard-autosave.json";

enum class ReadSaveFileStatus
{
    Success,
    FileNotFound,
    OpenError,
    ParseError
};

struct CompetitionSaveData
{
    QString fileVersion{ QString::fromLatin1(CompetitionSaveFileVersion) };
    QString host;
    QString date;
    QString location;
    QString home;
    QString guest;
    int currentRound{ 0 };
    int currentContest{ 0 };
    QRgb infoTextFg{ 0 };
    QRgb infoTextBg{ 0 };
    QRgb firstFg{ 0 };
    QRgb firstBg{ 0 };
    QRgb secondFg{ 0 };
    QRgb secondBg{ 0 };
    CompetitionMode mode;
    std::vector<std::vector<Contest>> rounds;
};

QJsonDocument ToJson(const CompetitionSaveData& data);

int CreateFromJson(const QJsonDocument& doc, const QString& expectedVersion,
                   bool loadWithIncompatibleVersion, CompetitionSaveData& data);

[[nodiscard]] ReadSaveFileStatus ReadSaveFile(const QString& filePath, QJsonDocument& document,
                                              QString* errorMessage = nullptr);

} // namespace Ipponboard::CompetitionSerialization
