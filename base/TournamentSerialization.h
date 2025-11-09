// Copyright 2018 Florian Muecke.
// SPDX-License-Identifier: BSD-2-Clause

#pragma once

#include "../core/Fight.h"
#include "../core/TournamentMode.h"

#include <QColor>
#include <QString>
#include <QJsonDocument>
#include <QString>
#include <vector>

namespace Ipponboard::TournamentSerialization
{
inline constexpr const char* TournamentSaveFileVersion = "1.0";
inline constexpr const char* AutoSaveFilename = "Ipponboard-autosave.json";

enum class ReadSaveFileStatus
{
	Success,
	FileNotFound,
	OpenError,
	ParseError
};

struct TournamentSaveData
{
	QString fileVersion { QString::fromLatin1(TournamentSaveFileVersion) };
	QString host;
	QString date;
	QString location;
	QString home;
	QString guest;
	int currentRound { 0 };
	int currentFight { 0 };
	QRgb infoTextFg { 0 };
	QRgb infoTextBg { 0 };
	QRgb firstFg { 0 };
	QRgb firstBg { 0 };
	QRgb secondFg { 0 };
	QRgb secondBg { 0 };
	TournamentMode mode;
	std::vector<std::vector<Fight>> rounds;
};

QJsonDocument ToJson(const TournamentSaveData& data);

int CreateFromJson(const QJsonDocument& doc,
						const QString& expectedVersion,
						bool loadWithIncompatibleVersion,
						TournamentSaveData& data);

[[nodiscard]] ReadSaveFileStatus ReadSaveFile(
	const QString& filePath,
	QJsonDocument& document,
	QString* errorMessage = nullptr);

}

