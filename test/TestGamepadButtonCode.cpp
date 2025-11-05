#include "../gamepad/Gamepad.h"
#include "../util/catch2/catch_amalgamated.hpp"

TEST_CASE("Gamepad button codes use dedicated POV range")
{
    using GamepadLib::Constants::PovCodeBase;
    using GamepadLib::EButton;
    using GamepadLib::Gamepad;

    SECTION("regular buttons keep their numeric ids")
    {
        REQUIRE(Gamepad::ButtonCode(EButton::button1) == 0);
        REQUIRE(Gamepad::ButtonCode(EButton::button12) == 11);
    }

    SECTION("pov buttons are offset from regular buttons")
    {
        REQUIRE(Gamepad::ButtonCode(EButton::button_pov_fwd) == PovCodeBase + 0);
        REQUIRE(Gamepad::ButtonCode(EButton::button_pov_right) == PovCodeBase + 1);
        REQUIRE(Gamepad::ButtonCode(EButton::button_pov_back) == PovCodeBase + 2);
        REQUIRE(Gamepad::ButtonCode(EButton::button_pov_left) == PovCodeBase + 3);
        REQUIRE(Gamepad::ButtonCode(EButton::button_pov_right_fwd) == PovCodeBase + 4);
        REQUIRE(Gamepad::ButtonCode(EButton::button_pov_right_back) == PovCodeBase + 5);
        REQUIRE(Gamepad::ButtonCode(EButton::button_pov_left_back) == PovCodeBase + 6);
        REQUIRE(Gamepad::ButtonCode(EButton::button_pov_left_fwd) == PovCodeBase + 7);
    }
}
