// Copyright 2025 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#include "../core/Enums.h"

#include <array>

namespace Ipponboard
{

class GamepadSectionMapper
{
  public:
    struct SectionAction
    {
        EAction action;
        bool revoke;
    };

    [[nodiscard]] static const std::array<SectionAction, 9>& Actions() noexcept;
};

} // namespace Ipponboard
