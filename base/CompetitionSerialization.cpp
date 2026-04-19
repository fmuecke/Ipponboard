// Copyright 2018 Florian Muecke.
// SPDX-License-Identifier: BSD-2-Clause

#include "CompetitionSerialization.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QtGlobal>
#include <utility>

using namespace Ipponboard;

namespace Ipponboard::CompetitionSerialization
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
constexpr auto str_SideA = "SideA";
constexpr auto str_SideB = "SideB";

constexpr auto str_CompetitionMode = "CompetitionMode";
constexpr auto str_Rounds = "Rounds";
constexpr auto str_ID = "ID";
constexpr auto str_FileVersion = "FileVersion";
constexpr auto str_Host = "Host";
constexpr auto str_Date = "Date";
constexpr auto str_Location = "Location";
constexpr auto str_Home = "Home";
constexpr auto str_Guest = "Guest";
constexpr auto str_CurrentRound = "CurrentRound";
constexpr auto str_CurrentContest = "CurrentContest";
constexpr auto str_TimeInSeconds = "TimeInSeconds";
constexpr auto str_TimeOverrides = "TimeOverrides";
constexpr auto str_FgColorInfoText = "FgColorInfoText";
constexpr auto str_BgColorInfoText = "BgColorInfoText";
constexpr auto str_FgColorFirst = "FgColorFirst";
constexpr auto str_BgColorFirst = "BgColorFirst";
constexpr auto str_FgColorSecond = "FgColorSecond";
constexpr auto str_BgColorSecond = "BgColorSecond";

QJsonDocument ToJson(const CompetitionSaveData& data)
{
    QJsonObject saveObject;
    saveObject.insert(str_FileVersion, data.fileVersion);
    saveObject.insert(str_Host, data.host);
    saveObject.insert(str_Date, data.date);
    saveObject.insert(str_Location, data.location);
    saveObject.insert(str_Home, data.home);
    saveObject.insert(str_Guest, data.guest);
    saveObject.insert(str_CurrentRound, data.currentRound);
    saveObject.insert(str_CurrentContest, data.currentContest);
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
    modeObject.insert(str_TimeInSeconds, mode.timeInSeconds);
    modeObject.insert(str_TimeOverrides, mode.GetTimeOverridesString());
    modeObject.insert(mode.str_Rules, mode.rules);
    modeObject.insert(mode.str_Options, mode.options);
    saveObject.insert(str_CompetitionMode, modeObject);

    QJsonArray tournamentArray;
    for (const auto& round : data.rounds)
    {
        QJsonArray roundArray;
        for (const auto& contest : round)
        {
            QJsonObject contestObject;

            contestObject.insert(str_Weight, contest.weight);
            contestObject.insert(str_SecondsElapsed, contest.GetSecondsElapsed());
            contestObject.insert(str_RoundTimeSeconds, contest.GetRoundSeconds());
            contestObject.insert(str_IsGoldenScore, contest.IsGoldenScore());
            contestObject.insert(str_IsSaved, contest.is_saved);

            QJsonObject sideAObject;
            sideAObject.insert(str_Name, contest.GetAthlete(ContestSide::SideA).name);
            sideAObject.insert(str_Club, contest.GetAthlete(ContestSide::SideA).club);
            sideAObject.insert(str_Ippon,
                               contest.GetScore(ContestSide::SideA).Value(Score::Point::Ippon));
            sideAObject.insert(str_Wazaari,
                               contest.GetScore(ContestSide::SideA).Value(Score::Point::Wazaari));
            sideAObject.insert(str_Yuko,
                               contest.GetScore(ContestSide::SideA).Value(Score::Point::Yuko));
            sideAObject.insert(str_Shido,
                               contest.GetScore(ContestSide::SideA).Value(Score::Point::Shido));
            sideAObject.insert(
                str_Hansokumake,
                contest.GetScore(ContestSide::SideA).Value(Score::Point::Hansokumake));
            contestObject.insert(str_SideA, sideAObject);

            QJsonObject sideBObject;
            sideBObject.insert(str_Name, contest.GetAthlete(ContestSide::SideB).name);
            sideBObject.insert(str_Club, contest.GetAthlete(ContestSide::SideB).club);
            sideBObject.insert(str_Ippon,
                               contest.GetScore(ContestSide::SideB).Value(Score::Point::Ippon));
            sideBObject.insert(str_Wazaari,
                               contest.GetScore(ContestSide::SideB).Value(Score::Point::Wazaari));
            sideBObject.insert(str_Yuko,
                               contest.GetScore(ContestSide::SideB).Value(Score::Point::Yuko));
            sideBObject.insert(str_Shido,
                               contest.GetScore(ContestSide::SideB).Value(Score::Point::Shido));
            sideBObject.insert(
                str_Hansokumake,
                contest.GetScore(ContestSide::SideB).Value(Score::Point::Hansokumake));
            contestObject.insert(str_SideB, sideBObject);

            roundArray.append(contestObject);
        }
        tournamentArray.append(roundArray);
    }
    saveObject.insert(str_Rounds, tournamentArray);

    return QJsonDocument(saveObject);
}

int CreateFromJson(const QJsonDocument& doc, const QString& expectedVersion,
                   bool loadWithIncompatibleVersion, CompetitionSaveData& data)
{
    const QJsonObject saveObject = doc.object();

    const auto fileVersion = saveObject[str_FileVersion].toString();
    if (fileVersion != expectedVersion && !loadWithIncompatibleVersion)
    {
        return 1;
    }

    CompetitionSaveData parsed;
    parsed.fileVersion = fileVersion;
    parsed.host = saveObject[str_Host].toString();
    parsed.date = saveObject[str_Date].toString();
    parsed.location = saveObject[str_Location].toString();
    parsed.home = saveObject[str_Home].toString();
    parsed.guest = saveObject[str_Guest].toString();
    parsed.currentRound = qBound(0, saveObject[str_CurrentRound].toInt(), 100);
    parsed.currentContest = qBound(0, saveObject[str_CurrentContest].toInt(), 100);
    parsed.infoTextFg = static_cast<QRgb>(saveObject[str_FgColorInfoText].toInt());
    parsed.infoTextBg = static_cast<QRgb>(saveObject[str_BgColorInfoText].toInt());
    parsed.firstFg = static_cast<QRgb>(saveObject[str_FgColorFirst].toInt());
    parsed.firstBg = static_cast<QRgb>(saveObject[str_BgColorFirst].toInt());
    parsed.secondFg = static_cast<QRgb>(saveObject[str_FgColorSecond].toInt());
    parsed.secondBg = static_cast<QRgb>(saveObject[str_BgColorSecond].toInt());

    const QJsonObject modeObject = saveObject[str_CompetitionMode].toObject();
    CompetitionMode mode;
    mode.id = modeObject[str_ID].toString();
    mode.title = modeObject[mode.str_Title].toString();
    mode.subTitle = modeObject[mode.str_SubTitle].toString();
    mode.listTemplate = modeObject[mode.str_Template].toString();
    mode.weights = modeObject[mode.str_Weights].toString();
    mode.weightsAreDoubled = modeObject[mode.str_WeightsAreDoubled].toBool();
    mode.nRounds = qBound(0, modeObject[mode.str_Rounds].toInt(), 10);
    mode.timeInSeconds = qBound(0, modeObject[str_TimeInSeconds].toInt(), 3600);
    mode.ExtractTimeOverrides(modeObject[str_TimeOverrides].toString(), mode.contestTimeOverrides);
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
        for (int contestIndex = 0; contestIndex < roundArray.size(); ++contestIndex)
        {
            const QJsonObject contestObject = roundArray.at(contestIndex).toObject();
            Contest contest;
            contest.weight = contestObject[str_Weight].toString();
            contest.SetSecondsElapsed(qBound(0, contestObject[str_SecondsElapsed].toInt(), 3600));
            contest.SetRoundTime(qBound(0, contestObject[str_RoundTimeSeconds].toInt(), 3600));
            contest.SetGoldenScore(contestObject[str_IsGoldenScore].toBool());
            contest.is_saved = contestObject[str_IsSaved].toBool();

            const QJsonObject sideAObject = contestObject[str_SideA].toObject();
            contest.GetAthlete(ContestSide::SideA).name = sideAObject[str_Name].toString();
            contest.GetAthlete(ContestSide::SideA).club = sideAObject[str_Club].toString();
            contest.GetScore(ContestSide::SideA)
                .SetValue(Score::Point::Ippon, qBound(0, sideAObject[str_Ippon].toInt(), 1));
            contest.GetScore(ContestSide::SideA)
                .SetValue(Score::Point::Wazaari, qBound(0, sideAObject[str_Wazaari].toInt(), 100));
            contest.GetScore(ContestSide::SideA)
                .SetValue(Score::Point::Yuko, qBound(0, sideAObject[str_Yuko].toInt(), 100));
            contest.GetScore(ContestSide::SideA)
                .SetValue(Score::Point::Shido, qBound(0, sideAObject[str_Shido].toInt(), 4));
            contest.GetScore(ContestSide::SideA)
                .SetValue(Score::Point::Hansokumake,
                          qBound(0, sideAObject[str_Hansokumake].toInt(), 1));

            const QJsonObject sideBObject = contestObject[str_SideB].toObject();
            contest.GetAthlete(ContestSide::SideB).name = sideBObject[str_Name].toString();
            contest.GetAthlete(ContestSide::SideB).club = sideBObject[str_Club].toString();
            contest.GetScore(ContestSide::SideB)
                .SetValue(Score::Point::Ippon, qBound(0, sideBObject[str_Ippon].toInt(), 1));
            contest.GetScore(ContestSide::SideB)
                .SetValue(Score::Point::Wazaari, qBound(0, sideBObject[str_Wazaari].toInt(), 100));
            contest.GetScore(ContestSide::SideB)
                .SetValue(Score::Point::Yuko, qBound(0, sideBObject[str_Yuko].toInt(), 100));
            contest.GetScore(ContestSide::SideB)
                .SetValue(Score::Point::Shido, qBound(0, sideBObject[str_Shido].toInt(), 4));
            contest.GetScore(ContestSide::SideB)
                .SetValue(Score::Point::Hansokumake,
                          qBound(0, sideBObject[str_Hansokumake].toInt(), 100));

            round.push_back(std::move(contest));
        }
    }

    data = std::move(parsed);
    return 0;
}

ReadSaveFileStatus ReadSaveFile(const QString& filePath, QJsonDocument& document,
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

} // namespace Ipponboard::CompetitionSerialization
