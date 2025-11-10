#include "../base/TournamentSerialization.cpp" // for access to internals
#include "../base/TournamentSerialization.h"
#include "TestData/IpponboardAutosaveJson.h"

#include <QColor>
#include <QDir>
#include <QFile>
#include <QJsonArray>
#include <QJsonObject>
#include <QString>
#include <QTemporaryDir>
#include <catch2/catch_test_macros.hpp>

using namespace Ipponboard;
using namespace Ipponboard::TournamentSerialization;

namespace
{
TournamentSaveData MakeSampleData()
{
    TournamentSaveData data;
    data.fileVersion = QString::fromLatin1(TournamentSaveFileVersion);
    data.host = "Host Club";
    data.date = "01.02.2024";
    data.location = "Sample City";
    data.home = "Home Club";
    data.guest = "Guest Club";
    data.currentRound = 1;
    data.currentFight = 2;
    data.infoTextFg = QColor(Qt::yellow).rgb();
    data.infoTextBg = QColor(Qt::black).rgb();
    data.firstFg = QColor(Qt::white).rgb();
    data.firstBg = QColor(Qt::blue).rgb();
    data.secondFg = QColor(Qt::black).rgb();
    data.secondBg = QColor(Qt::red).rgb();

    TournamentMode mode;
    mode.id = "mode-1";
    mode.title = "League";
    mode.subTitle = "Season";
    mode.listTemplate = "template.html";
    mode.weights = "60,66,73";
    mode.weightsAreDoubled = true;
    mode.nRounds = 2;
    mode.fightTimeInSeconds = 180;
    mode.fightTimeOverrides = { { "73", 120 } };
    mode.rules = "Default";
    mode.options = "SubscoreEnabled";
    data.mode = mode;

    Fight firstFight;
    firstFight.weight = "60";
    firstFight.SetSecondsElapsed(45);
    firstFight.SetRoundTime(180);
    firstFight.SetGoldenScore(true);
    firstFight.is_saved = true;
    firstFight.fighters[0].name = "First One";
    firstFight.fighters[0].club = "Home Club";
    firstFight.GetScore1().SetValue(Score::Point::Ippon, 1);
    firstFight.fighters[1].name = "Second One";
    firstFight.fighters[1].club = "Guest Club";
    firstFight.GetScore2().SetValue(Score::Point::Wazaari, 2);

    Fight secondFight;
    secondFight.weight = "66";
    secondFight.SetSecondsElapsed(100);
    secondFight.SetRoundTime(180);
    secondFight.SetGoldenScore(false);
    secondFight.is_saved = false;
    secondFight.fighters[0].name = "Home Fighter";
    secondFight.fighters[0].club = "Home Club";
    secondFight.GetScore1().SetValue(Score::Point::Shido, 1);
    secondFight.fighters[1].name = "Guest Fighter";
    secondFight.fighters[1].club = "Guest Club";
    secondFight.GetScore2().SetValue(Score::Point::Yuko, 3);

    data.rounds = { { firstFight, secondFight }, { secondFight } };

    return data;
}
} // namespace

TEST_CASE("ToJson serializes provided data")
{
    const auto data = MakeSampleData();
    const auto document = TournamentSerialization::ToJson(data);
    const auto object = document.object();

    REQUIRE(object["Host"].toString() == data.host);
    REQUIRE(object["Home"].toString() == data.home);
    REQUIRE(object["Guest"].toString() == data.guest);
    REQUIRE(object["CurrentRound"].toInt() == data.currentRound);
    REQUIRE(object["FgColorFirst"].toInt() == static_cast<int>(data.firstFg));

    const auto rounds = object["Rounds"].toArray();
    REQUIRE(rounds.size() == static_cast<int>(data.rounds.size()));

    const auto firstFight = rounds.first().toArray().first().toObject();
    CHECK(firstFight["Weight"].toString() == data.rounds.front().front().weight);
    CHECK(firstFight["IsGoldenScore"].toBool() == data.rounds.front().front().IsGoldenScore());
    CHECK(firstFight["FirstFighter"].toObject()["Name"].toString() ==
          data.rounds.front().front().GetFighter(FighterEnum::First).name);
    CHECK(firstFight["SecondFighter"].toObject()["Wazaari"].toInt() ==
          data.rounds.front().front().GetScore2().Value(Score::Point::Wazaari));
}

TEST_CASE("Conversion to and from json results in same data")
{
    QString jsonStr(TestData::IpponboardAutosaveJson);
    jsonStr.remove('\r'); // normalize line endings
    jsonStr.remove('\n');
    jsonStr.remove(' ');
    REQUIRE_FALSE(jsonStr.isEmpty());

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(jsonStr.toUtf8(), &err);
    REQUIRE(err.error == QJsonParseError::NoError);

    TournamentSaveData data;
    const auto result =
        TournamentSerialization::CreateFromJson(doc, TournamentSaveFileVersion, false, data);
    REQUIRE(result == 0);

    QString outJson(TournamentSerialization::ToJson(data).toJson(QJsonDocument::Indented));
    outJson.remove('\r'); // normalize line endings
    outJson.remove('\n');
    outJson.remove(' ');
    REQUIRE_FALSE(outJson.isEmpty());
    REQUIRE(jsonStr == outJson);
}

TEST_CASE("CreateFromJson round-trips tournament data")
{
    const auto original = MakeSampleData();
    const auto document = TournamentSerialization::ToJson(original);

    TournamentSaveData parsed;
    const auto result =
        TournamentSerialization::CreateFromJson(document, TournamentSaveFileVersion, false, parsed);

    REQUIRE(result == 0);
    CHECK(parsed.host == original.host);
    CHECK(parsed.guest == original.guest);
    CHECK(parsed.mode.id == original.mode.id);
    REQUIRE(parsed.rounds.size() == original.rounds.size());
    CHECK(parsed.rounds.front().front().GetScore2().Value(Score::Point::Wazaari) ==
          original.rounds.front().front().GetScore2().Value(Score::Point::Wazaari));
    CHECK(parsed.rounds.back().size() == original.rounds.back().size());
}

TEST_CASE("CreateFromJson reports version mismatch")
{
    const auto baseData = MakeSampleData();
    const auto document = TournamentSerialization::ToJson(baseData);

    auto mismatchingObject = document.object();
    mismatchingObject["FileVersion"] = "2.0";
    const QJsonDocument mismatchingDoc(mismatchingObject);

    TournamentSaveData parsed;
    const auto refusal = TournamentSerialization::CreateFromJson(
        mismatchingDoc, QString::fromLatin1(TournamentSaveFileVersion), false, parsed);
    REQUIRE(refusal == 1);

    const auto acceptance = TournamentSerialization::CreateFromJson(
        mismatchingDoc, QString::fromLatin1(TournamentSaveFileVersion), true, parsed);
    REQUIRE(acceptance == 0);
    CHECK(parsed.fileVersion == "2.0");
    CHECK(parsed.mode.id == baseData.mode.id);
}

SCENARIO("Autosave files on disk can be read back into JSON documents")
{
    GIVEN("a persisted autosave file containing the latest tournament state")
    {
        QTemporaryDir tempDir;
        REQUIRE(tempDir.isValid());

        const auto filePath = tempDir.filePath("Ipponboard-autosave.json");
        QFile file(filePath);
        REQUIRE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
        file.write(TestData::IpponboardAutosaveJson);
        file.close();

        QJsonDocument document;
        QString error;
        const auto status = TournamentSerialization::ReadSaveFile(filePath, document, &error);

        REQUIRE(status == TournamentSerialization::ReadSaveFileStatus::Success);
        REQUIRE(error.isEmpty());

        TournamentSaveData parsed;
        const auto parseResult = TournamentSerialization::CreateFromJson(
            document, TournamentSerialization::TournamentSaveFileVersion, false, parsed);

        REQUIRE(parseResult == 0);
        CHECK(parsed.host == "Entenhausen");
        CHECK(parsed.home == "Entenhausen");
        CHECK(parsed.guest == "The Daltons");
    }

    GIVEN("no autosave file exists at the expected location")
    {
        const auto nonexistentPath = QDir::temp().filePath("not-there/Ipponboard-autosave.json");
        QJsonDocument document;
        QString error;
        const auto status =
            TournamentSerialization::ReadSaveFile(nonexistentPath, document, &error);

        CHECK(status == TournamentSerialization::ReadSaveFileStatus::FileNotFound);
        CHECK(document.isNull());
        CHECK(error.isEmpty());
    }
}
