// Shared lightweight gamepad types to avoid include cycles
#pragma once

#include <cstdint>
#include <utility>

namespace GamepadLib
{
using UnsignedPair = std::pair<unsigned, unsigned>;

enum class EGamepadState : int8_t
{
    unknown = -1,
    ok = 0,
    no_driver,
    invalid_param,
    bad_device_id,
    no_service,
    unplugged,
    invalid_id
};

enum EUpdateAction : int8_t
{
    always = 0,
    on_change = 1
};

enum class EPovType : int8_t
{
    unknown = -1,
    no_pov = 0,
    discrete,
    continuous
};

enum EAxis : uint8_t
{
    X = 0,
    Y,
    Z,
    R,
    U,
    V,
    MaxValue
};

enum EButton : uint8_t
{
    button1 = 0,
    button2,
    button3,
    button4,
    button5,
    button6,
    button7,
    button8,
    button9,
    button10,
    button11,
    button12,
    button13,
    button14,
    button15,
    button16,
    button17,
    button18,
    button19,
    button20,
    button21,
    button22,
    button23,
    button24,
    button25,
    button26,
    button27,
    button28,
    button29,
    button30,
    button31,
    button32,
    button_pov_fwd,
    button_pov_right,
    button_pov_back,
    button_pov_left,
    button_pov_right_fwd,
    button_pov_right_back,
    button_pov_left_back,
    button_pov_left_fwd
};

namespace Constants
{
static constexpr unsigned MaxButtons = 40;
static constexpr unsigned PovCodeBase = 50000;
static constexpr unsigned PovCodeCount = 8;
static constexpr unsigned MaxPos = 0xFFFF;
static constexpr unsigned MidPos = 0x7FFF;
static constexpr float ToleranceVal = 100.0f;
static constexpr unsigned PovCenteredVal = 65535;
} // namespace Constants

} // namespace GamepadLib
