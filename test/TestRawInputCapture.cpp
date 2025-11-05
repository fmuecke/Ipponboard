// Copyright 2024 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../base/RawInputCapture.h"
#include "../util/catch2/catch_amalgamated.hpp"

#include <algorithm>
#include <array>
#include <optional>
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

namespace
{
struct FakeAxisGamepad
{
    using AxisValues = std::array<unsigned, GamepadLib::EAxis::MaxValue>;

    void enqueue(AxisValues values) { m_states.push_back(std::move(values)); }

    void ReadData()
    {
        if (m_nextState < m_states.size())
        {
            m_current = m_states[m_nextState++];
        }
    }

    unsigned AxisValueRaw(GamepadLib::EAxis axis) const
    {
        return m_current[static_cast<std::size_t>(axis)];
    }

    std::optional<int> RawAxisCode(GamepadLib::EAxis axis) const { return static_cast<int>(axis); }

  private:
    std::vector<AxisValues> m_states;
    std::size_t m_nextState{ 0 };
    AxisValues m_current{};
};
} // namespace

SCENARIO("Raw axis capture reports the axis with the largest movement")
{
    using Capture = Ipponboard::BasicRawAxisCapture<FakeAxisGamepad>;

    GIVEN("neutral and moved axis snapshots")
    {
        FakeAxisGamepad gamepad;
        FakeAxisGamepad::AxisValues neutral{};
        neutral.fill(GamepadLib::Constants::MidPos);
        gamepad.enqueue(neutral);

        auto movedX = neutral;
        movedX[static_cast<std::size_t>(GamepadLib::EAxis::X)] =
            GamepadLib::Constants::MidPos + 6000;
        gamepad.enqueue(movedX);

        Capture capture(gamepad,
                        { GamepadLib::EAxis::X, GamepadLib::EAxis::Y, GamepadLib::EAxis::R });
        capture.Prime();

        WHEN("polling after significant movement")
        {
            const auto code = capture.PollAxis();

            THEN("the moved axis is reported")
            {
                REQUIRE(code.has_value());
                REQUIRE(*code == static_cast<int>(GamepadLib::EAxis::X));
            }
        }
    }
}

SCENARIO("Raw axis capture ignores minor noise")
{
    using Capture = Ipponboard::BasicRawAxisCapture<FakeAxisGamepad>;

    GIVEN("axis readings with subtle changes only")
    {
        FakeAxisGamepad gamepad;
        FakeAxisGamepad::AxisValues neutral{};
        neutral.fill(GamepadLib::Constants::MidPos);
        gamepad.enqueue(neutral);

        auto noisy = neutral;
        noisy[static_cast<std::size_t>(GamepadLib::EAxis::Y)] += 500; // below threshold
        gamepad.enqueue(noisy);

        Capture capture(gamepad,
                        { GamepadLib::EAxis::X, GamepadLib::EAxis::Y, GamepadLib::EAxis::R });
        capture.Prime();

        WHEN("polling after minor noise")
        {
            const auto code = capture.PollAxis();

            THEN("no axis is reported") { REQUIRE_FALSE(code.has_value()); }
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

SCENARIO("Raw input capture reports encoded POV button codes")
{
    using Capture = Ipponboard::BasicRawInputCapture<FakeGamepad>;

    GIVEN("a new POV direction is emitted")
    {
        FakeGamepad gamepad;
        gamepad.enqueue({});
        gamepad.enqueue({ GamepadLib::Constants::PovCodeBase });

        Capture capture(gamepad);
        capture.Prime();

        WHEN("polling after the POV change")
        {
            const auto value = capture.PollButton();

            THEN("the encoded POV button is reported")
            {
                REQUIRE(value.has_value());
                REQUIRE(*value == GamepadLib::Constants::PovCodeBase);
            }
        }
    }
}
