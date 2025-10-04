// Copyright 2024 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "catch2/catch.hpp"

#include "../base/RawInputCapture.h"

#include <vector>

namespace
{
struct FakeGamepad
{
    using ButtonSet = std::unordered_set<std::uint16_t>;

    void enqueue(ButtonSet buttons) { m_states.push_back(std::move(buttons)); }

    void ReadData()
    {
        if (m_nextState < m_states.size())
        {
            m_current = m_states[m_nextState++];
        }
    }

    const ButtonSet& CurrentButtons() const { return m_current; }

  private:
    std::vector<ButtonSet> m_states;
    std::size_t m_nextState{ 0 };
    ButtonSet m_current;
};
} // namespace

SCENARIO("Raw input capture detects newly pressed buttons")
{
    using Capture = Ipponboard::BasicRawInputCapture<FakeGamepad>;

    GIVEN("a sequence of button snapshots")
    {
        FakeGamepad gamepad;
        gamepad.enqueue({});
        gamepad.enqueue({ 5u });
        gamepad.enqueue({ 5u, 7u });

        Capture capture(gamepad);

        WHEN("capture is primed and polled")
        {
            capture.Prime();
            const auto first = capture.PollButton();
            const auto second = capture.PollButton();

            THEN("the first new button is reported in order")
            {
                REQUIRE(first.has_value());
                REQUIRE(*first == 5u);
                REQUIRE(second.has_value());
                REQUIRE(*second == 7u);
            }
        }
    }
}

SCENARIO("Raw input capture ignores unchanged or centered values")
{
    using Capture = Ipponboard::BasicRawInputCapture<FakeGamepad>;

    GIVEN("a sustained press with no new buttons")
    {
        FakeGamepad gamepad;
        gamepad.enqueue({});
        gamepad.enqueue({ 3u });
        gamepad.enqueue({ 3u });

        Capture capture(gamepad);
        capture.Prime();
        const auto first = capture.PollButton();

        WHEN("no new buttons are pressed afterwards")
        {
            const auto second = capture.PollButton();

            THEN("no additional button is reported")
            {
                REQUIRE(first.has_value());
                REQUIRE(*first == 3u);
                REQUIRE_FALSE(second.has_value());
            }
        }
    }

    GIVEN("a centered POV value only")
    {
        FakeGamepad gamepad;
        gamepad.enqueue({});
        gamepad.enqueue({ GamepadLib::Constants::PovCenteredVal });

        Capture capture(gamepad);
        capture.Prime();

        WHEN("polling the POV centered state")
        {
            const auto value = capture.PollButton();

            THEN("no button is captured") { REQUIRE_FALSE(value.has_value()); }
        }
    }
}
