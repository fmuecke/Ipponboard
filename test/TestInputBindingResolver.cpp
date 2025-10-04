// Copyright 2024 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../base/InputBindingResolver.h"
#include "../gamepad/GamepadTypes.h"
#include "catch2/catch.hpp"

using Ipponboard::ResolveRawFirst;

SCENARIO("Raw bindings are prioritised over legacy button mappings")
{
    GIVEN("a configured raw code and fallback button")
    {
        const int rawCode = 42;
        const int fallbackButton = static_cast<int>(GamepadLib::EButton::button1);

        WHEN("the raw predicate reports a press")
        {
            bool fallbackCalled = false;
            const auto rawPredicate = [rawCode](std::uint16_t code) { return code == rawCode; };
            const auto fallbackPredicate = [&fallbackCalled](GamepadLib::EButton)
            {
                fallbackCalled = true;
                return true;
            };

            const bool result =
                ResolveRawFirst(rawCode, fallbackButton, rawPredicate, fallbackPredicate);

            THEN("the raw press triggers the action")
            {
                REQUIRE(result);
                REQUIRE_FALSE(fallbackCalled);
            }
        }

        WHEN("the raw predicate does not match but the fallback does")
        {
            const auto rawPredicate = [](std::uint16_t) { return false; };
            const auto fallbackPredicate = [](GamepadLib::EButton button)
            { return button == GamepadLib::EButton::button1; };

            const bool result =
                ResolveRawFirst(rawCode, fallbackButton, rawPredicate, fallbackPredicate);

            THEN("the legacy button mapping is honoured") { REQUIRE(result); }
        }
    }
}

SCENARIO("Invalid or missing bindings do not trigger actions")
{
    GIVEN("no raw code and no fallback button")
    {
        const auto rawPredicate = [](std::uint16_t) { return true; };
        const auto fallbackPredicate = [](GamepadLib::EButton) { return true; };

        WHEN("both bindings are absent")
        {
            const bool result = ResolveRawFirst(-1, -1, rawPredicate, fallbackPredicate);

            THEN("no action is triggered") { REQUIRE_FALSE(result); }
        }
    }

    GIVEN("an invalid fallback button code")
    {
        const auto rawPredicate = [](std::uint16_t) { return false; };
        const auto fallbackPredicate = [](GamepadLib::EButton) { return true; };

        WHEN("the fallback button is outside the supported range")
        {
            const bool result = ResolveRawFirst(-1, 100, rawPredicate, fallbackPredicate);

            THEN("the action is not triggered") { REQUIRE_FALSE(result); }
        }
    }
}
