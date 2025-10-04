// Copyright 2024 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#include "../gamepad/GamepadTypes.h"

#include <cstdint>

namespace Ipponboard
{

namespace detail
{

[[nodiscard]] inline bool is_valid_button_code(int code) noexcept
{
    return code >= 0 && code < static_cast<int>(GamepadLib::Constants::MaxButtons);
}

} // namespace detail

template <class RawPredicate, class StandardPredicate>
[[nodiscard]] inline bool ResolveRawFirst(int rawCode, int standardCode,
                                          RawPredicate&& rawPredicate,
                                          StandardPredicate&& standardPredicate) noexcept
{
    if (rawCode >= 0 && rawPredicate(static_cast<std::uint16_t>(rawCode)))
    {
        return true;
    }

    if (detail::is_valid_button_code(standardCode))
    {
        return standardPredicate(static_cast<GamepadLib::EButton>(standardCode));
    }

    return false;
}

} // namespace Ipponboard
