#include "ControllerTestHelpers.h"

#include <catch2/catch_test_macros.hpp>

using namespace Ipponboard;
using namespace TestSupport;

namespace
{
TournamentMode simple_mode()
{
    TournamentMode mode;
    mode.id = "test-mode";
    mode.title = "Test";
    mode.weights = "-60;-73";
    mode.nRounds = 1;
    mode.fightTimeInSeconds = 180;
    mode.weightsAreDoubled = false;
    return mode;
}
} // namespace

TEST_CASE("[TournamentModel] Aggregates wins and score across fights")
{
    ControllerFixture fixture;
    auto mode = simple_mode();

    fixture.controller.InitTournament(mode);

    fixture.controller.SetFight(0, 0, "-60", "Alice", "", "Bob", "", 0, 0, 1, 0, 0, 0, 0, 0, 0, 0);

    fixture.controller.SetFight(0, 1, "-73", "Carol", "", "Dave", "", 0, 0, 0, 0, 0, 0, 1, 0, 0, 0);

    auto model = fixture.controller.GetTournamentScoreModel();

    auto wins = model->GetTotalWins();
    REQUIRE(wins.first == 1);
    REQUIRE(wins.second == 1);

    auto scores = model->GetTotalScore();
    REQUIRE(scores.first == 10);
    REQUIRE(scores.second == 7);
}

TEST_CASE("[TournamentModel] Data returns fight details for display role")
{
    ControllerFixture fixture;
    auto mode = simple_mode();

    fixture.controller.InitTournament(mode);

    fixture.controller.SetFight(
        0, 0, "-60", "Alice", "Club A", "Bob", "Club B", 0, 1, 0, 0, 0, 0, 0, 0, 0, 0);

    auto model = fixture.controller.GetTournamentScoreModel();

    QModelIndex weightIdx = model->index(0, TournamentModel::eCol_weight);
    QModelIndex nameIdx = model->index(0, TournamentModel::eCol_name1);
    QModelIndex wazaariIdx = model->index(0, TournamentModel::eCol_wazaari1);

    REQUIRE(model->data(weightIdx, Qt::DisplayRole).toString() == "-60");
    REQUIRE(model->data(nameIdx, Qt::DisplayRole).toString() == "Alice");
    REQUIRE(model->data(wazaariIdx, Qt::DisplayRole).toString() == "1");
}
