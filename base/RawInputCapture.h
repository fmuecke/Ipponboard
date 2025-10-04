// Copyright 2024 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#include "../gamepad/GamepadTypes.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>
#include <unordered_set>
#include <vector>

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

template <class Provider> class BasicRawAxisCapture
{
  public:
    BasicRawAxisCapture(Provider& provider,
                        std::vector<GamepadLib::EAxis> axes,
                        unsigned threshold = 2000u) noexcept;

    void Prime();
    [[nodiscard]] std::optional<int> PollAxis();

  private:
    [[nodiscard]] unsigned read_axis(GamepadLib::EAxis axis) const;

    Provider* m_provider;
    std::vector<GamepadLib::EAxis> m_axes;
    std::array<unsigned, GamepadLib::EAxis::MaxValue> m_lastValues;
    unsigned m_threshold;
    bool m_primed;
};

template <class Provider>
inline BasicRawAxisCapture<Provider>::BasicRawAxisCapture(Provider& provider,
                                                          std::vector<GamepadLib::EAxis> axes,
                                                          unsigned threshold) noexcept
    : m_provider(&provider), m_axes(std::move(axes)), m_lastValues{}, m_threshold(threshold),
      m_primed(false)
{
    m_lastValues.fill(0u);
}

template <class Provider> inline void BasicRawAxisCapture<Provider>::Prime()
{
    if (!m_provider)
    {
        return;
    }

    m_provider->ReadData();
    for (const auto axis : m_axes)
    {
        m_lastValues[static_cast<std::size_t>(axis)] = read_axis(axis);
    }
    m_primed = true;
}

template <class Provider> inline std::optional<int> BasicRawAxisCapture<Provider>::PollAxis()
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

    GamepadLib::EAxis bestAxis = GamepadLib::EAxis::X;
    unsigned bestDelta = 0u;

    for (const auto axis : m_axes)
    {
        const auto current = read_axis(axis);
        const auto last = m_lastValues[static_cast<std::size_t>(axis)];
        const unsigned delta = static_cast<unsigned>(std::abs(static_cast<int>(current) -
                                                              static_cast<int>(last)));

        if (delta > bestDelta)
        {
            bestAxis = axis;
            bestDelta = delta;
        }

        m_lastValues[static_cast<std::size_t>(axis)] = current;
    }

    if (bestDelta < m_threshold)
    {
        return std::nullopt;
    }

    if (auto rawCode = m_provider->RawAxisCode(bestAxis))
    {
        return rawCode;
    }

    return static_cast<int>(bestAxis);
}

template <class Provider>
inline unsigned BasicRawAxisCapture<Provider>::read_axis(GamepadLib::EAxis axis) const
{
    if (!m_provider)
    {
        return 0u;
    }

    return m_provider->AxisValueRaw(axis);
}

using RawAxisCapture = BasicRawAxisCapture<GamepadLib::Gamepad>;

} // namespace Ipponboard
