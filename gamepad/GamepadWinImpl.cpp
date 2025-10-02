#if defined(_WIN32)

#include "GamepadWinImpl.h"

#include "IGamepadImpl.h"

#include <Windows.h>
#include <cstring>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "Winmm.lib")

namespace FMlib
{
GamepadImpl::GamepadImpl() { reset(); }

Gamepad::EState GamepadImpl::Init()
{
    reset();
    state = toState(::joyGetDevCapsW(currentId, &caps, sizeof(caps)));

    if (state == Gamepad::eState_ok)
    {
        productName.assign(caps.szPname);
    }
    else
    {
        productName.clear();
    }

    return state;
}

Gamepad::EState GamepadImpl::ReadData()
{
    lastData = data;
    state = toState(::joyGetPosEx(currentId, &data));
    return state;
}

Gamepad::EState GamepadImpl::StateValue() const { return state; }

const wchar_t* GamepadImpl::ProductName() const
{
    return productName.empty() ? L"" : productName.c_str();
}

std::uint16_t GamepadImpl::VendorId() const { return caps.wMid; }
std::uint16_t GamepadImpl::ProductId() const { return caps.wPid; }
unsigned GamepadImpl::NumButtons() const { return caps.wNumButtons; }
unsigned GamepadImpl::NumAxes() const { return caps.wNumAxes; }

Gamepad::UnsignedPair GamepadImpl::PollingFrequency() const
{
    return { caps.wPeriodMin, caps.wPeriodMax };
}

Gamepad::UnsignedPair GamepadImpl::AxisRange(Gamepad::EAxis axis) const
{
    switch (axis)
    {
    case Gamepad::eAxis_X:
        return { caps.wXmin, caps.wXmax };
    case Gamepad::eAxis_Y:
        return { caps.wYmin, caps.wYmax };
    case Gamepad::eAxis_Z:
        return { caps.wZmin, caps.wZmax };
    case Gamepad::eAxis_R:
        return { caps.wRmin, caps.wRmax };
    case Gamepad::eAxis_U:
        return { caps.wUmin, caps.wUmax };
    case Gamepad::eAxis_V:
        return { caps.wVmin, caps.wVmax };
    default:
        return { 0u, Gamepad::eMax };
    }
}

bool GamepadImpl::HasAxis(Gamepad::EAxis axis) const
{
    switch (axis)
    {
    case Gamepad::eAxis_Z:
        return (caps.wCaps & JOYCAPS_HASZ) != 0;
    case Gamepad::eAxis_R:
        return (caps.wCaps & JOYCAPS_HASR) != 0;
    case Gamepad::eAxis_U:
        return (caps.wCaps & JOYCAPS_HASU) != 0;
    case Gamepad::eAxis_V:
        return (caps.wCaps & JOYCAPS_HASV) != 0;
    default:
        return true;
    }
}

unsigned GamepadImpl::AxisValue(Gamepad::EAxis axis) const
{
    switch (axis)
    {
    case Gamepad::eAxis_X:
        return data.dwXpos;
    case Gamepad::eAxis_Y:
        return data.dwYpos;
    case Gamepad::eAxis_Z:
        return data.dwZpos;
    case Gamepad::eAxis_R:
        return data.dwRpos;
    case Gamepad::eAxis_U:
        return data.dwUpos;
    case Gamepad::eAxis_V:
        return data.dwVpos;
    default:
        return 0;
    }
}

unsigned GamepadImpl::LastAxisValue(Gamepad::EAxis axis) const
{
    switch (axis)
    {
    case Gamepad::eAxis_X:
        return lastData.dwXpos;
    case Gamepad::eAxis_Y:
        return lastData.dwYpos;
    case Gamepad::eAxis_Z:
        return lastData.dwZpos;
    case Gamepad::eAxis_R:
        return lastData.dwRpos;
    case Gamepad::eAxis_U:
        return lastData.dwUpos;
    case Gamepad::eAxis_V:
        return lastData.dwVpos;
    default:
        return 0;
    }
}

std::uint32_t GamepadImpl::ButtonsMask() const { return data.dwButtons; }
std::uint32_t GamepadImpl::LastButtonsMask() const { return lastData.dwButtons; }

unsigned GamepadImpl::Pov() const { return data.dwPOV; }
unsigned GamepadImpl::LastPov() const { return lastData.dwPOV; }

int GamepadImpl::PressedCount() const { return static_cast<int>(data.dwButtonNumber); }

unsigned GamepadImpl::Threshold() const
{
    unsigned value = 0;
    if (JOYERR_NOERROR != ::joyGetThreshold(currentId, &value))
    {
        return static_cast<unsigned>(-1);
    }
    return value;
}

bool GamepadImpl::SetThreshold(unsigned thresholdValue) const
{
    return JOYERR_NOERROR == ::joySetThreshold(currentId, thresholdValue);
}

bool GamepadImpl::Capture(void* windowHandle, unsigned int period, Gamepad::EUpdateAction when)
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
        ::joySetCapture(targetHwnd, currentId, period, when == Gamepad::update_on_change);
    if (result == JOYERR_NOERROR)
    {
        hwnd = targetHwnd;
        return true;
    }

    return false;
}

bool GamepadImpl::Release()
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

void GamepadImpl::reset()
{
    productName.clear();
    std::memset(&caps, 0, sizeof(caps));
    std::memset(&data, 0, sizeof(data));
    data.dwSize = sizeof(data);
    data.dwFlags = JOY_RETURNALL;
    lastData = data;
    state = Gamepad::eState_unknown;
    hwnd = nullptr;
}

Gamepad::EState GamepadImpl::toState(unsigned code)
{
    switch (code)
    {
    case JOYERR_NOERROR:
        return Gamepad::eState_ok;
    case MMSYSERR_NODRIVER:
        return Gamepad::eState_no_driver;
    case MMSYSERR_INVALPARAM:
        return Gamepad::eState_invalid_param;
    case MMSYSERR_BADDEVICEID:
        return Gamepad::eState_bad_device_id;
    case JOYERR_NOCANDO:
        return Gamepad::eState_no_service;
    case JOYERR_UNPLUGGED:
        return Gamepad::eState_unplugged;
    case JOYERR_PARMS:
        return Gamepad::eState_invalid_id;
    default:
        return Gamepad::eState_unknown;
    }
}

} // namespace FMlib

#endif // _WIN32
