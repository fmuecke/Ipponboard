#include "../base/GamepadSectionMapper.h"
#include "../core/Enums.h"
#include "../util/catch2/catch_amalgamated.hpp"

using Catch::Approx;
using Ipponboard::GamepadSectionMapper;

TEST_CASE("Gamepad section mapper preserves action sequencing")
{
    const auto& actions = GamepadSectionMapper::Actions();
    REQUIRE(actions.size() == 9);

    REQUIRE(actions[0].action == Ipponboard::eAction_NONE);
    REQUIRE_FALSE(actions[0].revoke);

    REQUIRE(actions[7].action == Ipponboard::eAction_Ippon);
    REQUIRE_FALSE(actions[7].revoke);

    REQUIRE(actions[8].action == Ipponboard::eAction_Wazaari);
    REQUIRE_FALSE(actions[8].revoke);

    REQUIRE(actions[5].action == Ipponboard::eAction_Yuko);
    REQUIRE_FALSE(actions[5].revoke);

    REQUIRE(actions[3].action == Ipponboard::eAction_Shido);
    REQUIRE(actions[3].revoke);

    REQUIRE(actions[2].action == Ipponboard::eAction_Ippon);
    REQUIRE(actions[2].revoke);

    REQUIRE(actions[1].action == Ipponboard::eAction_Wazaari);
    REQUIRE(actions[1].revoke);

    REQUIRE(actions[4].action == Ipponboard::eAction_Yuko);
    REQUIRE(actions[4].revoke);

    REQUIRE(actions[6].action == Ipponboard::eAction_Shido);
    REQUIRE_FALSE(actions[6].revoke);
}
