// Copyright 2025 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "GamepadSectionMapper.h"

#include <cstddef>

namespace
{

using Ipponboard::GamepadSectionMapper;

std::array<GamepadSectionMapper::SectionAction, 9> make_actions() noexcept
{
    using SectionAction = GamepadSectionMapper::SectionAction;

    return { SectionAction{ Ipponboard::eAction_NONE, false },
             SectionAction{ Ipponboard::eAction_Wazaari, true },
             SectionAction{ Ipponboard::eAction_Ippon, true },
             SectionAction{ Ipponboard::eAction_Shido, true },
             SectionAction{ Ipponboard::eAction_Yuko, true },
             SectionAction{ Ipponboard::eAction_Yuko, false },
             SectionAction{ Ipponboard::eAction_Shido, false },
             SectionAction{ Ipponboard::eAction_Ippon, false },
             SectionAction{ Ipponboard::eAction_Wazaari, false } };
}

} // namespace

namespace Ipponboard
{

const std::array<GamepadSectionMapper::SectionAction, 9>& GamepadSectionMapper::Actions() noexcept
{
    static const auto actions = make_actions();
    return actions;
}

} // namespace Ipponboard
