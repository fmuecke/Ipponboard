// Copyright 2024 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#include "../gamepad/GamepadTypes.h"

#include <algorithm>
#include <optional>
#include <unordered_set>

namespace GamepadLib
{
class Gamepad;
}

namespace Ipponboard
{

template <class Provider> class BasicRawInputCapture
{
  public:
    explicit BasicRawInputCapture(Provider& provider) noexcept
        : m_provider(&provider), m_lastButtons(), m_primed(false)
    {
    }

    void Prime()
    {
        if (!m_provider)
        {
            return;
        }

        m_provider->ReadData();
        const auto& current = m_provider->CurrentButtons();
        m_lastButtons = current;
        m_primed = true;
    }

    [[nodiscard]] std::optional<std::uint16_t> PollButton()
    {
        if (!m_provider)
        {
            return std::nullopt;
        }

        if (!m_primed)
        {
            Prime();
        }

        m_provider->ReadData();
        const auto& current = m_provider->CurrentButtons();

        std::optional<std::uint16_t> newest;
        for (const auto code : current)
        {
            if (code == GamepadLib::Constants::PovCenteredVal)
            {
                continue;
            }

            if (m_lastButtons.find(code) == m_lastButtons.end())
            {
                if (!newest || code < *newest)
                {
                    newest = code;
                }
            }
        }

        m_lastButtons = current;
        return newest;
    }

  private:
    Provider* m_provider;
    std::unordered_set<std::uint16_t> m_lastButtons;
    bool m_primed;
};

using RawInputCapture = BasicRawInputCapture<GamepadLib::Gamepad>;

} // namespace Ipponboard
