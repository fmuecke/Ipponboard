// Copyright 2018 Florian Muecke.
// SPDX-License-Identifier: BSD-2-Clause

#include "TournamentSerialization.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QtGlobal>

#include <utility>

using namespace Ipponboard;

namespace Ipponboard::TournamentSerialization
{
	constexpr auto str_Weight = "Weight";
	constexpr auto str_SecondsElapsed = "SecondsElapsed";
	constexpr auto str_RoundTimeSeconds = "RoundTimeSeconds";
	constexpr auto str_IsGoldenScore = "IsGoldenScore";
	constexpr auto str_IsSaved = "IsSaved";
	constexpr auto str_Name = "Name";
	constexpr auto str_Club = "Club";
	constexpr auto str_Ippon = "Ippon";
	constexpr auto str_Wazaari = "Wazaari";
	constexpr auto str_Yuko = "Yuko";
	constexpr auto str_Shido = "Shido";
	constexpr auto str_Hansokumake = "Hansokumake";
	constexpr auto str_FirstFighter = "FirstFighter";
	constexpr auto str_SecondFighter = "SecondFighter";

	constexpr auto str_TournamentMode = "TournamentMode";
	constexpr auto str_Rounds = "Rounds";
	constexpr auto str_ID = "ID";
	constexpr auto str_FileVersion = "FileVersion";
	constexpr auto str_Host = "Host";
	constexpr auto str_Date = "Date";
	constexpr auto str_Location = "Location";
	constexpr auto str_Home = "Home";
	constexpr auto str_Guest = "Guest";
	constexpr auto str_CurrentRound = "CurrentRound";
	constexpr auto str_CurrentFight = "CurrentFight";
	constexpr auto str_FgColorInfoText = "FgColorInfoText";
	constexpr auto str_BgColorInfoText = "BgColorInfoText";
	constexpr auto str_FgColorFirst = "FgColorFirst";
	constexpr auto str_BgColorFirst = "BgColorFirst";
	constexpr auto str_FgColorSecond = "FgColorSecond";
	constexpr auto str_BgColorSecond = "BgColorSecond";

QJsonDocument ToJson(const TournamentSaveData& data)
{
	QJsonObject saveObject;
	saveObject.insert(str_FileVersion, data.fileVersion);
	saveObject.insert(str_Host, data.host);
	saveObject.insert(str_Date, data.date);
	saveObject.insert(str_Location, data.location);
	saveObject.insert(str_Home, data.home);
	saveObject.insert(str_Guest, data.guest);
	saveObject.insert(str_CurrentRound, data.currentRound);
	saveObject.insert(str_CurrentFight, data.currentFight);
	saveObject.insert(str_FgColorInfoText, static_cast<int>(data.infoTextFg));
	saveObject.insert(str_BgColorInfoText, static_cast<int>(data.infoTextBg));
	saveObject.insert(str_FgColorFirst, static_cast<int>(data.firstFg));
	saveObject.insert(str_BgColorFirst, static_cast<int>(data.firstBg));
	saveObject.insert(str_FgColorSecond, static_cast<int>(data.secondFg));
	saveObject.insert(str_BgColorSecond, static_cast<int>(data.secondBg));

	const auto& mode = data.mode;
	QJsonObject modeObject;
	modeObject.insert(str_ID, mode.id);
	modeObject.insert(mode.str_Title, mode.title);
	modeObject.insert(mode.str_SubTitle, mode.subTitle);
	modeObject.insert(mode.str_Template, mode.listTemplate);
	modeObject.insert(mode.str_Weights, mode.weights);
	modeObject.insert(mode.str_WeightsAreDoubled, mode.weightsAreDoubled);
	modeObject.insert(mode.str_Rounds, mode.nRounds);
	modeObject.insert(mode.str_FightTimeInSeconds, mode.fightTimeInSeconds);
	modeObject.insert(mode.str_FightTimeOverrides, mode.GetFightTimeOverridesString());
	modeObject.insert(mode.str_Rules, mode.rules);
	modeObject.insert(mode.str_Options, mode.options);
	saveObject.insert(str_TournamentMode, modeObject);

	QJsonArray tournamentArray;
	for (const auto& round : data.rounds)
	{
		QJsonArray roundArray;
		for (const auto& fight : round)
		{
			QJsonObject fightObject;

			fightObject.insert(str_Weight, fight.weight);
			fightObject.insert(str_SecondsElapsed, fight.GetSecondsElapsed());
			fightObject.insert(str_RoundTimeSeconds, fight.GetRoundSeconds());
			fightObject.insert(str_IsGoldenScore, fight.IsGoldenScore());
			fightObject.insert(str_IsSaved, fight.is_saved);

			QJsonObject firstFighter;
			firstFighter.insert(str_Name, fight.GetFighter(FighterEnum::First).name);
			firstFighter.insert(str_Club, fight.GetFighter(FighterEnum::First).club);
			firstFighter.insert(str_Ippon, fight.GetScore1().Value(Score::Point::Ippon));
			firstFighter.insert(str_Wazaari, fight.GetScore1().Value(Score::Point::Wazaari));
			firstFighter.insert(str_Yuko, fight.GetScore1().Value(Score::Point::Yuko));
			firstFighter.insert(str_Shido, fight.GetScore1().Value(Score::Point::Shido));
			firstFighter.insert(str_Hansokumake, fight.GetScore1().Value(Score::Point::Hansokumake));
			fightObject.insert(str_FirstFighter, firstFighter);

			QJsonObject secondFighter;
			secondFighter.insert(str_Name, fight.GetFighter(FighterEnum::Second).name);
			secondFighter.insert(str_Club, fight.GetFighter(FighterEnum::Second).club);
			secondFighter.insert(str_Ippon, fight.GetScore2().Value(Score::Point::Ippon));
			secondFighter.insert(str_Wazaari, fight.GetScore2().Value(Score::Point::Wazaari));
			secondFighter.insert(str_Yuko, fight.GetScore2().Value(Score::Point::Yuko));
			secondFighter.insert(str_Shido, fight.GetScore2().Value(Score::Point::Shido));
			secondFighter.insert(str_Hansokumake, fight.GetScore2().Value(Score::Point::Hansokumake));
			fightObject.insert(str_SecondFighter, secondFighter);

			roundArray.append(fightObject);
		}
		tournamentArray.append(roundArray);
	}
	saveObject.insert(str_Rounds, tournamentArray);

	return QJsonDocument(saveObject);
}

int CreateFromJson(const QJsonDocument& doc,
							const QString& expectedVersion,
							bool loadWithIncompatibleVersion,
							TournamentSaveData& data)
{
	const QJsonObject saveObject = doc.object();

	const auto fileVersion = saveObject[str_FileVersion].toString();
	if (fileVersion != expectedVersion && !loadWithIncompatibleVersion)
	{
		return 1;
	}

	TournamentSaveData parsed;
	parsed.fileVersion = fileVersion;
	parsed.host = saveObject[str_Host].toString();
	parsed.date = saveObject[str_Date].toString();
	parsed.location = saveObject[str_Location].toString();
	parsed.home = saveObject[str_Home].toString();
	parsed.guest = saveObject[str_Guest].toString();
	parsed.currentRound = qBound(0, saveObject[str_CurrentRound].toInt(), 100);
	parsed.currentFight = qBound(0, saveObject[str_CurrentFight].toInt(), 100);
	parsed.infoTextFg = static_cast<QRgb>(saveObject[str_FgColorInfoText].toInt());
	parsed.infoTextBg = static_cast<QRgb>(saveObject[str_BgColorInfoText].toInt());
	parsed.firstFg = static_cast<QRgb>(saveObject[str_FgColorFirst].toInt());
	parsed.firstBg = static_cast<QRgb>(saveObject[str_BgColorFirst].toInt());
	parsed.secondFg = static_cast<QRgb>(saveObject[str_FgColorSecond].toInt());
	parsed.secondBg = static_cast<QRgb>(saveObject[str_BgColorSecond].toInt());

	const QJsonObject modeObject = saveObject[str_TournamentMode].toObject();
	TournamentMode mode;
	mode.id = modeObject[str_ID].toString();
	mode.title = modeObject[mode.str_Title].toString();
	mode.subTitle = modeObject[mode.str_SubTitle].toString();
	mode.listTemplate = modeObject[mode.str_Template].toString();
	mode.weights = modeObject[mode.str_Weights].toString();
	mode.weightsAreDoubled = modeObject[mode.str_WeightsAreDoubled].toBool();
	mode.nRounds = qBound(0, modeObject[mode.str_Rounds].toInt(), 10);
	mode.fightTimeInSeconds = qBound(0, modeObject[mode.str_FightTimeInSeconds].toInt(), 3600);
	mode.ExtractFightTimeOverrides(modeObject[mode.str_FightTimeOverrides].toString(), mode.fightTimeOverrides);
	mode.rules = modeObject[mode.str_Rules].toString();
	mode.options = modeObject[mode.str_Options].toString();
	parsed.mode = mode;

	const QJsonArray roundsArray = saveObject[str_Rounds].toArray();
	parsed.rounds.resize(roundsArray.size());
	for (int roundIndex = 0; roundIndex < roundsArray.size(); ++roundIndex)
	{
		const QJsonArray roundArray = roundsArray.at(roundIndex).toArray();
		auto& round = parsed.rounds[roundIndex];
		round.reserve(roundArray.size());
		for (int fightIndex = 0; fightIndex < roundArray.size(); ++fightIndex)
		{
			const QJsonObject fightObject = roundArray.at(fightIndex).toObject();
			Fight fight;
			fight.weight = fightObject[str_Weight].toString();
			fight.SetSecondsElapsed(qBound(0, fightObject[str_SecondsElapsed].toInt(), 3600));
			fight.SetRoundTime(qBound(0, fightObject[str_RoundTimeSeconds].toInt(), 3600));
			fight.SetGoldenScore(fightObject[str_IsGoldenScore].toBool());
			fight.is_saved = fightObject[str_IsSaved].toBool();

			const QJsonObject firstFighter = fightObject[str_FirstFighter].toObject();
			fight.fighters[0].name = firstFighter[str_Name].toString();
			fight.fighters[0].club = firstFighter[str_Club].toString();
			fight.GetScore1().SetValue(Score::Point::Ippon, qBound(0, firstFighter[str_Ippon].toInt(), 1));
			fight.GetScore1().SetValue(Score::Point::Wazaari, qBound(0, firstFighter[str_Wazaari].toInt(), 100));
			fight.GetScore1().SetValue(Score::Point::Yuko, qBound(0, firstFighter[str_Yuko].toInt(), 100));
			fight.GetScore1().SetValue(Score::Point::Shido, qBound(0, firstFighter[str_Shido].toInt(), 4));
			fight.GetScore1().SetValue(Score::Point::Hansokumake, qBound(0, firstFighter[str_Hansokumake].toInt(), 1));

			const QJsonObject secondFighter = fightObject[str_SecondFighter].toObject();
			fight.fighters[1].name = secondFighter[str_Name].toString();
			fight.fighters[1].club = secondFighter[str_Club].toString();
			fight.GetScore2().SetValue(Score::Point::Ippon, qBound(0, secondFighter[str_Ippon].toInt(), 1));
			fight.GetScore2().SetValue(Score::Point::Wazaari, qBound(0, secondFighter[str_Wazaari].toInt(), 100));
			fight.GetScore2().SetValue(Score::Point::Yuko, qBound(0, secondFighter[str_Yuko].toInt(), 100));
			fight.GetScore2().SetValue(Score::Point::Shido, qBound(0, secondFighter[str_Shido].toInt(), 4));
			fight.GetScore2().SetValue(Score::Point::Hansokumake, qBound(0, secondFighter[str_Hansokumake].toInt(), 100));

			round.push_back(std::move(fight));
		}
	}

data = std::move(parsed);
	return 0;
}

ReadSaveFileStatus ReadSaveFile(
	const QString& filePath,
	QJsonDocument& document,
	QString* errorMessage)
{
	document = QJsonDocument();

	QFile file(filePath);
	if (!file.exists())
	{
		if (errorMessage != nullptr)
		{
			errorMessage->clear();
		}
		return ReadSaveFileStatus::FileNotFound;
	}

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		if (errorMessage != nullptr)
		{
			*errorMessage = file.errorString();
		}
		return ReadSaveFileStatus::OpenError;
	}

	const auto bytes = file.readAll();
	QJsonParseError parseError;
	const auto parsed = QJsonDocument::fromJson(bytes, &parseError);
	if (parseError.error != QJsonParseError::NoError)
	{
		if (errorMessage != nullptr)
		{
			*errorMessage = parseError.errorString();
		}
		return ReadSaveFileStatus::ParseError;
	}

	document = parsed;
	if (errorMessage != nullptr)
	{
		errorMessage->clear();
	}
	return ReadSaveFileStatus::Success;
}

} // namespace Ipponboard::TournamentSerialization
