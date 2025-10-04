#if defined(_WIN32)

#include "GamepadWin.h"

#include <cstring>
#include <mmsystem.h>

namespace GamepadLib
{
namespace
{
constexpr std::uint16_t povCode(unsigned pov)
{
    switch (pov)
    {
    case JOY_POVFORWARD:
        return static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_fwd));
    case JOY_POVRIGHT:
        return static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_right));
    case JOY_POVBACKWARD:
        return static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_back));
    case JOY_POVLEFT:
        return static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_left));
    case 4500:
        return static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_right_fwd));
    case 13500:
        return static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_right_back));
    case 22500:
        return static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_left_back));
    case 31500:
        return static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_left_fwd));
    default:
        return Constants::PovCenteredVal;
    }
}
}

GamepadWin::GamepadWin()
{
    reset();
}

EGamepadState GamepadWin::Init()
{
    reset();
    state = toState(::joyGetDevCapsW(currentId, &caps, sizeof(caps)));

    if (state == EGamepadState::ok)
    {
        productName.assign(caps.szPname);
    }
    else
    {
        productName.clear();
    }

    ReadData();
    return state;
}

EGamepadState GamepadWin::ReadData()
{
    lastData = data;
    state = toState(::joyGetPosEx(currentId, &data));
    updateButtonSets();
    return state;
}

EGamepadState GamepadWin::StateValue() const { return state; }

const wchar_t* GamepadWin::ProductName() const
{
    return productName.empty() ? L"" : productName.c_str();
}

std::uint16_t GamepadWin::VendorId() const { return caps.wMid; }
std::uint16_t GamepadWin::ProductId() const { return caps.wPid; }
unsigned GamepadWin::NumButtons() const { return caps.wNumButtons; }
unsigned GamepadWin::NumAxes() const { return caps.wNumAxes; }

UnsignedPair GamepadWin::PollingFrequency() const { return { caps.wPeriodMin, caps.wPeriodMax }; }

UnsignedPair GamepadWin::AxisRange(EAxis axis) const
{
    switch (axis)
    {
    case EAxis::X:
        return { caps.wXmin, caps.wXmax };
    case EAxis::Y:
        return { caps.wYmin, caps.wYmax };
    case EAxis::Z:
        return { caps.wZmin, caps.wZmax };
    case EAxis::R:
        return { caps.wRmin, caps.wRmax };
    case EAxis::U:
        return { caps.wUmin, caps.wUmax };
    case EAxis::V:
        return { caps.wVmin, caps.wVmax };
    default:
        return { 0u, Constants::MaxAngle };
    }
}

bool GamepadWin::HasAxis(EAxis axis) const
{
    switch (axis)
    {
    case EAxis::Z:
        return (caps.wCaps & JOYCAPS_HASZ) != 0;
    case EAxis::R:
        return (caps.wCaps & JOYCAPS_HASR) != 0;
    case EAxis::U:
        return (caps.wCaps & JOYCAPS_HASU) != 0;
    case EAxis::V:
        return (caps.wCaps & JOYCAPS_HASV) != 0;
    default:
        return true;
    }
}

unsigned GamepadWin::AxisValue(GamepadLib::EAxis axis) const
{
    switch (axis)
    {
    case EAxis::X:
        return data.dwXpos;
    case EAxis::Y:
        return data.dwYpos;
    case EAxis::Z:
        return data.dwZpos;
    case EAxis::R:
        return data.dwRpos;
    case EAxis::U:
        return data.dwUpos;
    case EAxis::V:
        return data.dwVpos;
    default:
        return 0;
    }
}

unsigned GamepadWin::LastAxisValue(EAxis axis) const
{
    switch (axis)
    {
    case EAxis::X:
        return lastData.dwXpos;
    case EAxis::Y:
        return lastData.dwYpos;
    case EAxis::Z:
        return lastData.dwZpos;
    case EAxis::R:
        return lastData.dwRpos;
    case EAxis::U:
        return lastData.dwUpos;
    case EAxis::V:
        return lastData.dwVpos;
    default:
        return 0;
    }
}

const std::unordered_set<std::uint16_t>& GamepadWin::CurrentButtons() const
{
    return m_buttons;
}

const std::unordered_set<std::uint16_t>& GamepadWin::PreviousButtons() const
{
    return m_lastButtons;
}

unsigned GamepadWin::Pov() const { return data.dwPOV == JOY_POVCENTERED ? Constants::PovCenteredVal : data.dwPOV; }
unsigned GamepadWin::LastPov() const
{
    return lastData.dwPOV == JOY_POVCENTERED ? Constants::PovCenteredVal : lastData.dwPOV;
}

EPovType GamepadWin::PovType() const
{
    return (caps.wCaps & JOYCAPS_HASPOV) ? EPovType::discrete : EPovType::no_pov;
}

int GamepadWin::PressedCount() const
{
    return static_cast<int>(m_buttons.size());
}

unsigned GamepadWin::Threshold() const
{
    unsigned value = 0;
    if (JOYERR_NOERROR != ::joyGetThreshold(currentId, &value))
    {
        return static_cast<unsigned>(-1);
    }
    return value;
}

bool GamepadWin::SetThreshold(unsigned thresholdValue) const
{
    return JOYERR_NOERROR == ::joySetThreshold(currentId, thresholdValue);
}

bool GamepadWin::Capture(void* windowHandle, unsigned int period, EUpdateAction when)
{
    if (hwnd != nullptr)
    {
        return true;
    }

    auto targetHwnd = static_cast<HWND>(windowHandle);
    if (targetHwnd == nullptr)
    {
        return false;
    }

    const auto result =
        ::joySetCapture(targetHwnd, currentId, period, when == EUpdateAction::on_change);
    if (result == JOYERR_NOERROR)
    {
        hwnd = targetHwnd;
        return true;
    }

    return false;
}

bool GamepadWin::Release()
{
    if (hwnd == nullptr)
    {
        return true;
    }

    const auto result = ::joyReleaseCapture(currentId);
    if (result == JOYERR_NOERROR)
    {
        hwnd = nullptr;
        return true;
    }

    return false;
}

void GamepadWin::reset()
{
    productName.clear();
    std::memset(&caps, 0, sizeof(caps));
    std::memset(&data, 0, sizeof(data));
    data.dwSize = sizeof(data);
    data.dwFlags = JOY_RETURNALL;
    lastData = data;
    state = EGamepadState::unknown;
    hwnd = nullptr;
    m_buttons.clear();
    m_lastButtons.clear();
}

void GamepadWin::updateButtonSets()
{
    m_lastButtons = m_buttons;
    m_buttons.clear();

    const std::uint32_t mask = data.dwButtons;
    for (unsigned bit = 0; bit < 32; ++bit)
    {
        if (mask & (1u << bit))
        {
            m_buttons.insert(static_cast<std::uint16_t>(bit));
        }
    }

    const auto povRaw = data.dwPOV;
    const auto code = povCode(povRaw);
    if (code != Constants::PovCenteredVal)
    {
        m_buttons.insert(code);
    }
}

EGamepadState GamepadWin::toState(unsigned code)
{
    switch (code)
    {
    case JOYERR_NOERROR:
        return EGamepadState::ok;
    case MMSYSERR_NODRIVER:
        return EGamepadState::no_driver;
    case MMSYSERR_INVALPARAM:
        return EGamepadState::invalid_param;
    case MMSYSERR_BADDEVICEID:
        return EGamepadState::bad_device_id;
    case JOYERR_NOCANDO:
        return EGamepadState::no_service;
    case JOYERR_UNPLUGGED:
        return EGamepadState::unplugged;
    case JOYERR_PARMS:
        return EGamepadState::invalid_id;
    default:
        return EGamepadState::unknown;
    }
}

} // namespace GamepadLib

#endif // _WIN32
