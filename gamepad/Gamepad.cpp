#include "Gamepad.h"

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <cstring>
#include <string>

#if defined(_WIN32)
#include <Windows.h>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")
#else
#include <QGamepad>
#include <QGamepadManager>
#include <QList>
#endif

namespace FMlib
{
namespace
{
constexpr std::array<unsigned, Gamepad::kMaxButtons> makeButtonCodes()
{
    std::array<unsigned, Gamepad::kMaxButtons> codes{};
    for (std::size_t i = 0; i < 32; ++i)
    {
        codes[i] = 1u << i;
    }

    codes[Gamepad::eButton_pov_fwd] = 0;
    codes[Gamepad::eButton_pov_right] = 9000;
    codes[Gamepad::eButton_pov_back] = 18000;
    codes[Gamepad::eButton_pov_left] = 27000;
    codes[Gamepad::eButton_pov_right_fwd] = 4500;
    codes[Gamepad::eButton_pov_right_back] = 13500;
    codes[Gamepad::eButton_pov_left_back] = 22500;
    codes[Gamepad::eButton_pov_left_fwd] = 31500;

    return codes;
}

constexpr auto kButtonCodes = makeButtonCodes();

constexpr std::size_t axisIndex(Gamepad::EAxis axis)
{
    return static_cast<std::size_t>(axis);
}

unsigned popcount(std::uint32_t value)
{
    unsigned count = 0;
    while (value != 0)
    {
        value &= (value - 1);
        ++count;
    }
    return count;
}

} // namespace

unsigned buttonCode(Gamepad::EButton button)
{
    return kButtonCodes[static_cast<std::size_t>(button)];
}

#if defined(_WIN32)

struct Gamepad::Impl
{
    Impl() { reset(); }

    Gamepad::EState init()
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

    const wchar_t* productNamePtr() const
    {
        return productName.empty() ? L"" : productName.c_str();
    }

    std::uint16_t mid() const { return caps.wMid; }
    std::uint16_t pid() const { return caps.wPid; }
    unsigned numButtons() const { return caps.wNumButtons; }
    unsigned numAxes() const { return caps.wNumAxes; }

    Gamepad::UnsignedPair pollingFreq() const { return { caps.wPeriodMin, caps.wPeriodMax }; }

    Gamepad::UnsignedPair axisRange(Gamepad::EAxis axis) const
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

    bool hasAxis(Gamepad::EAxis axis) const
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

    Gamepad::EPovType povType() const
    {
        if ((caps.wCaps & JOYCAPS_HASPOV) == 0)
        {
            return Gamepad::ePovType_no_pov;
        }

        if ((caps.wCaps & JOYCAPS_POVCTS) != 0)
        {
            return Gamepad::ePovType_continuous;
        }

        if ((caps.wCaps & JOYCAPS_POV4DIR) != 0)
        {
            return Gamepad::ePovType_discrete;
        }

        return Gamepad::ePovType_unknown;
    }

    Gamepad::EState readData()
    {
        lastData = data;
        state = toState(::joyGetPosEx(currentId, &data));
        return state;
    }

    Gamepad::EState stateValue() const { return state; }

    unsigned axisValue(Gamepad::EAxis axis) const
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

    unsigned lastAxisValue(Gamepad::EAxis axis) const
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

    std::uint32_t buttonsMask() const { return data.dwButtons; }
    std::uint32_t lastButtonsMask() const { return lastData.dwButtons; }

    unsigned pov() const { return data.dwPOV; }
    unsigned lastPov() const { return lastData.dwPOV; }

    int pressedCount() const { return static_cast<int>(data.dwButtonNumber); }

    unsigned threshold() const
    {
        unsigned value = 0;
        if (JOYERR_NOERROR != ::joyGetThreshold(currentId, &value))
        {
            return static_cast<unsigned>(-1);
        }
        return value;
    }

    bool setThreshold(unsigned thresholdValue) const
    {
        return JOYERR_NOERROR == ::joySetThreshold(currentId, thresholdValue);
    }

    bool capture(void* windowHandle, unsigned int period, Gamepad::EUpdateAction when)
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

    bool release()
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

  private:
    void reset()
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

    static Gamepad::EState toState(unsigned code)
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

    unsigned currentId{ 0 };
    std::wstring productName;
    JOYCAPSW caps{};
    JOYINFOEX data{};
    JOYINFOEX lastData{};
    Gamepad::EState state{ Gamepad::eState_unknown };
    HWND hwnd{ nullptr };
};

#else // Linux / Qt implementation

struct Gamepad::Impl
{
    Impl()
    {
        ranges.fill({ 0u, Gamepad::eMax });
        axisSupported.fill(true);
    }

    Gamepad::EState init()
    {
        const auto ids = QGamepadManager::instance()->connectedGamepads();
        if (ids.isEmpty())
        {
            resetState();
            state = Gamepad::eState_unplugged;
            return state;
        }

        deviceId = ids.first();
        gamepad = std::make_unique<QGamepad>(deviceId);
        refreshMetadata();
        state = Gamepad::eState_ok;
        readData();
        return state;
    }

    const wchar_t* productNamePtr() const
    {
        return productName.empty() ? L"" : productName.c_str();
    }

    std::uint16_t mid() const { return vendorId; }
    std::uint16_t pid() const { return productId; }
    unsigned numButtons() const { return buttonCount; }
    unsigned numAxes() const { return axisCount; }
    Gamepad::UnsignedPair pollingFreq() const { return { 0u, 0u }; }

    Gamepad::UnsignedPair axisRange(Gamepad::EAxis axis) const { return ranges[axisIndex(axis)]; }

    bool hasAxis(Gamepad::EAxis axis) const { return axisSupported[axisIndex(axis)]; }

    Gamepad::EPovType povType() const
    {
        return dpadPresent ? Gamepad::ePovType_discrete : Gamepad::ePovType_no_pov;
    }

    Gamepad::EState readData()
    {
        updateConnection();

        lastAxes = axes;
        lastButtons = buttons;
        lastPovValue = povValue;

        if (!gamepad)
        {
            axes.fill(Gamepad::eMid);
            buttons = 0;
            povValue = Gamepad::kPovCentered;
            return state;
        }

        axes = sampleAxes();
        buttons = sampleButtons();
        povValue = computePov();
        state = Gamepad::eState_ok;
        return state;
    }

    Gamepad::EState stateValue() const { return state; }

    unsigned axisValue(Gamepad::EAxis axis) const { return axes[axisIndex(axis)]; }

    unsigned lastAxisValue(Gamepad::EAxis axis) const { return lastAxes[axisIndex(axis)]; }

    std::uint32_t buttonsMask() const { return buttons; }
    std::uint32_t lastButtonsMask() const { return lastButtons; }

    unsigned pov() const { return povValue; }
    unsigned lastPov() const { return lastPovValue; }
    int pressedCount() const { return static_cast<int>(popcount(buttons)); }

    unsigned threshold() const { return 0; }
    bool setThreshold(unsigned) const { return false; }

    bool capture(void*, unsigned int, Gamepad::EUpdateAction) { return false; }
    bool release() { return false; }

  private:
    void resetState()
    {
        gamepad.reset();
        productName.clear();
        vendorId = 0;
        productId = 0;
        buttonCount = 0;
        axisCount = 0;
        axes.fill(Gamepad::eMid);
        lastAxes = axes;
        buttons = 0;
        lastButtons = 0;
        povValue = Gamepad::kPovCentered;
        lastPovValue = Gamepad::kPovCentered;
        dpadPresent = false;
    }

    void refreshMetadata()
    {
        if (!gamepad)
        {
            return;
        }

        auto& mgr = *QGamepadManager::instance();
        productName = mgr.gamepadName(deviceId).toStdWString();
        vendorId = 0;
        productId = 0;
        buttonCount = 16; // typical modern controllers expose 12–16 digital buttons
        axisCount = Gamepad::eAxis_MAX;
        dpadPresent = true;
    }

    void updateConnection()
    {
        auto& mgr = *QGamepadManager::instance();
        const auto ids = mgr.connectedGamepads();

        if (deviceId != -1 && !ids.contains(deviceId))
        {
            resetState();
            state = Gamepad::eState_unplugged;
        }

        if (!gamepad && !ids.isEmpty())
        {
            deviceId = ids.first();
            gamepad = std::make_unique<QGamepad>(deviceId);
            refreshMetadata();
        }
    }

    static unsigned toUnsignedAxis(double value)
    {
        value = std::clamp(value, -1.0, 1.0);
        return static_cast<unsigned>(((value + 1.0) * 0.5) * Gamepad::eMax + 0.5);
    }

    static unsigned toUnsignedTrigger(double value)
    {
        value = std::clamp(value, 0.0, 1.0);
        return static_cast<unsigned>(value * Gamepad::eMax + 0.5);
    }

    std::array<unsigned, Gamepad::eAxis_MAX> sampleAxes() const
    {
        std::array<unsigned, Gamepad::eAxis_MAX> result{};
        result[Gamepad::eAxis_X] = toUnsignedAxis(gamepad->axisLeftX());
        result[Gamepad::eAxis_Y] = toUnsignedAxis(gamepad->axisLeftY());
        result[Gamepad::eAxis_Z] = toUnsignedAxis(gamepad->axisRightX());
        result[Gamepad::eAxis_R] = toUnsignedAxis(gamepad->axisRightY());
        result[Gamepad::eAxis_U] = toUnsignedTrigger(gamepad->buttonL2());
        result[Gamepad::eAxis_V] = toUnsignedTrigger(gamepad->buttonR2());
        return result;
    }

    std::uint32_t sampleButtons() const
    {
        std::uint32_t mask = 0;
        setButton(mask, Gamepad::eButton1, gamepad->buttonA());
        setButton(mask, Gamepad::eButton2, gamepad->buttonB());
        setButton(mask, Gamepad::eButton3, gamepad->buttonX());
        setButton(mask, Gamepad::eButton4, gamepad->buttonY());
        setButton(mask, Gamepad::eButton5, gamepad->buttonL1());
        setButton(mask, Gamepad::eButton6, gamepad->buttonR1());
        setButton(mask, Gamepad::eButton7, gamepad->buttonL2() > 0.5); // analogue treated as button
        setButton(mask, Gamepad::eButton8, gamepad->buttonR2() > 0.5);
        setButton(mask, Gamepad::eButton9, gamepad->buttonSelect());
        setButton(mask, Gamepad::eButton10, gamepad->buttonStart());
        setButton(mask, Gamepad::eButton11, gamepad->buttonL3());
        setButton(mask, Gamepad::eButton12, gamepad->buttonR3());
        setButton(mask, Gamepad::eButton17, gamepad->buttonCenter());
        setButton(mask, Gamepad::eButton18, gamepad->buttonGuide());
        setButton(mask, Gamepad::eButton13, gamepad->buttonUp());
        setButton(mask, Gamepad::eButton14, gamepad->buttonRight());
        setButton(mask, Gamepad::eButton15, gamepad->buttonDown());
        setButton(mask, Gamepad::eButton16, gamepad->buttonLeft());
        return mask;
    }

    static void setButton(std::uint32_t& mask, Gamepad::EButton button, bool pressed)
    {
        if (pressed)
        {
            mask |= buttonCode(button);
        }
    }

    unsigned computePov() const
    {
        const bool up = gamepad->buttonUp();
        const bool down = gamepad->buttonDown();
        const bool left = gamepad->buttonLeft();
        const bool right = gamepad->buttonRight();

        if ((up && down) || (left && right))
        {
            return Gamepad::kPovCentered;
        }

        if (up)
        {
            if (right)
                return 4500;
            if (left)
                return 31500;
            return 0;
        }

        if (down)
        {
            if (right)
                return 13500;
            if (left)
                return 22500;
            return 18000;
        }

        if (right)
            return 9000;
        if (left)
            return 27000;

        return Gamepad::kPovCentered;
    }

    int deviceId{ -1 };
    std::unique_ptr<QGamepad> gamepad;
    std::wstring productName;
    std::uint16_t vendorId{ 0 };
    std::uint16_t productId{ 0 };
    unsigned buttonCount{ 0 };
    unsigned axisCount{ 0 };
    Gamepad::EState state{ Gamepad::eState_unknown };
    std::array<Gamepad::UnsignedPair, Gamepad::eAxis_MAX> ranges{};
    std::array<unsigned, Gamepad::eAxis_MAX> axes{};
    std::array<unsigned, Gamepad::eAxis_MAX> lastAxes{};
    std::array<bool, Gamepad::eAxis_MAX> axisSupported{};
    std::uint32_t buttons{ 0 };
    std::uint32_t lastButtons{ 0 };
    unsigned povValue{ Gamepad::kPovCentered };
    unsigned lastPovValue{ Gamepad::kPovCentered };
    bool dpadPresent{ false };
};

#endif

Gamepad::Gamepad() : d(std::make_unique<Impl>())
{
    Init();
}
Gamepad::~Gamepad() = default;
Gamepad::Gamepad(Gamepad&&) noexcept = default;
Gamepad& Gamepad::operator=(Gamepad&&) noexcept = default;

Gamepad::EState Gamepad::Init()
{
    m_invertedAxes.reset();
    return d->init();
}

const wchar_t* Gamepad::GetProductName() const
{
    return d->productNamePtr();
}

std::uint16_t Gamepad::GetMId() const
{
    return d->mid();
}

std::uint16_t Gamepad::GetPId() const
{
    return d->pid();
}

unsigned Gamepad::GetNumButtons() const
{
    return d->numButtons();
}

unsigned Gamepad::GetNumAxes() const
{
    return d->numAxes();
}

Gamepad::UnsignedPair Gamepad::GetPollingFreq() const
{
    return d->pollingFreq();
}

Gamepad::UnsignedPair Gamepad::GetRangeX() const
{
    return d->axisRange(eAxis_X);
}

Gamepad::UnsignedPair Gamepad::GetRangeY() const
{
    return d->axisRange(eAxis_Y);
}

Gamepad::UnsignedPair Gamepad::GetRangeZ() const
{
    return d->axisRange(eAxis_Z);
}

Gamepad::UnsignedPair Gamepad::GetRangeR() const
{
    return d->axisRange(eAxis_R);
}

Gamepad::UnsignedPair Gamepad::GetRangeU() const
{
    return d->axisRange(eAxis_U);
}

Gamepad::UnsignedPair Gamepad::GetRangeV() const
{
    return d->axisRange(eAxis_V);
}

bool Gamepad::HasAxisZ() const
{
    return d->hasAxis(eAxis_Z);
}

bool Gamepad::HasAxisR() const
{
    return d->hasAxis(eAxis_R);
}

bool Gamepad::HasAxisU() const
{
    return d->hasAxis(eAxis_U);
}

bool Gamepad::HasAxisV() const
{
    return d->hasAxis(eAxis_V);
}

Gamepad::EPovType Gamepad::GetPovType() const
{
    return d->povType();
}

void Gamepad::SetInverted(EAxis axis, bool value)
{
    m_invertedAxes.set(axis, value);
}

bool Gamepad::IsInverted(EAxis axis) const
{
    return m_invertedAxes.test(axis);
}

bool Gamepad::WasPressed(EButton button) const
{
    const auto code = buttonCode(button);
    if (button <= eButton32)
    {
        return (d->lastButtonsMask() & code) == 0 && (d->buttonsMask() & code) != 0;
    }

    if (code == kPovCentered)
    {
        return false;
    }

    return d->lastPov() != code && d->pov() == code;
}

bool Gamepad::WasReleased(EButton button) const
{
    const auto code = buttonCode(button);
    if (button <= eButton32)
    {
        return (d->lastButtonsMask() & code) != 0 && (d->buttonsMask() & code) == 0;
    }

    if (code == kPovCentered)
    {
        return false;
    }

    return d->lastPov() == code && d->pov() != code;
}

bool Gamepad::IsPressed(EButton button) const
{
    const auto code = buttonCode(button);
    if (button <= eButton32)
    {
        return (d->buttonsMask() & code) != 0;
    }

    if (code == kPovCentered)
    {
        return false;
    }

    return d->pov() == code;
}

bool Gamepad::WasSectionEnteredXY(float min, float max) const
{
    const auto lastX = static_cast<int>(applyInversion(eAxis_X, d->lastAxisValue(eAxis_X)));
    const auto lastY = static_cast<int>(applyInversion(eAxis_Y, d->lastAxisValue(eAxis_Y)));
    const auto curX = static_cast<int>(GetXPos());
    const auto curY = static_cast<int>(GetYPos());

    const auto lastAlpha = GetAngle<float>(
        lastX - static_cast<int>(eMid), lastY - static_cast<int>(eMid), 1.02f, eMid);
    const auto curAlpha =
        GetAngle<float>(curX - static_cast<int>(eMid), curY - static_cast<int>(eMid), 1.02f, eMid);

    const bool changed = (lastAlpha < 0.0f || curAlpha < 0.0f ||
                          std::fabs(lastAlpha - curAlpha) > static_cast<float>(eTolerance));
    return changed && IsInSection(curAlpha, min, max);
}

bool Gamepad::WasSectionEnteredRZ(float min, float max) const
{
    const auto lastR = static_cast<int>(applyInversion(eAxis_R, d->lastAxisValue(eAxis_R)));
    const auto lastZ = static_cast<int>(applyInversion(eAxis_Z, d->lastAxisValue(eAxis_Z)));
    const auto curR = static_cast<int>(GetRPos());
    const auto curZ = static_cast<int>(GetZPos());

    const auto lastAlpha = GetAngle<float>(
        lastR - static_cast<int>(eMid), lastZ - static_cast<int>(eMid), 1.02f, eMid);
    const auto curAlpha =
        GetAngle<float>(curR - static_cast<int>(eMid), curZ - static_cast<int>(eMid), 1.02f, eMid);

    const bool changed = (lastAlpha < 0.0f || curAlpha < 0.0f ||
                          std::fabs(lastAlpha - curAlpha) > static_cast<float>(eTolerance));
    return changed && IsInSection(curAlpha, min, max);
}

bool Gamepad::WasSectionLeft(float, float) const
{
    return false;
}

bool Gamepad::IsInSection(float alpha, float min, float max) const
{
    if (min > max)
    {
        if ((alpha >= min && alpha <= 360.0f) || (alpha > 0.0f && alpha <= max))
        {
            return true;
        }
    }
    else if (alpha >= min && alpha <= max)
    {
        return true;
    }

    return false;
}

float Gamepad::GetAngleXY() const
{
    return GetAngle<float>(static_cast<int>(GetXPos()) - static_cast<int>(eMid),
                           static_cast<int>(GetYPos()) - static_cast<int>(eMid),
                           1.2f,
                           eMid);
}

float Gamepad::GetAngleRZ() const
{
    return GetAngle<float>(static_cast<int>(GetRPos()) - static_cast<int>(eMid),
                           static_cast<int>(GetZPos()) - static_cast<int>(eMid),
                           1.2f,
                           eMid);
}

int Gamepad::PressedCount() const
{
    return d->pressedCount();
}

unsigned Gamepad::applyInversion(EAxis axis, unsigned value) const
{
    return m_invertedAxes.test(axis) ? eMax - value : value;
}

unsigned Gamepad::GetXPos() const
{
    return applyInversion(eAxis_X, d->axisValue(eAxis_X));
}

unsigned Gamepad::GetYPos() const
{
    return applyInversion(eAxis_Y, d->axisValue(eAxis_Y));
}

unsigned Gamepad::GetZPos() const
{
    return applyInversion(eAxis_Z, d->axisValue(eAxis_Z));
}

unsigned Gamepad::GetRPos() const
{
    return applyInversion(eAxis_R, d->axisValue(eAxis_R));
}

unsigned Gamepad::GetUPos() const
{
    return applyInversion(eAxis_U, d->axisValue(eAxis_U));
}

unsigned Gamepad::GetVPos() const
{
    return applyInversion(eAxis_V, d->axisValue(eAxis_V));
}

unsigned Gamepad::GetPOV() const
{
    return d->pov();
}

Gamepad::EState Gamepad::ReadData()
{
    return d->readData();
}

Gamepad::EState Gamepad::GetState() const
{
    return d->stateValue();
}

unsigned Gamepad::GetThreshold() const
{
    return d->threshold();
}

bool Gamepad::SetThreshold(unsigned threshold) const
{
    return d->setThreshold(threshold);
}

bool Gamepad::Capture(void* windowHandle, unsigned int period, EUpdateAction when)
{
    return d->capture(windowHandle, period, when);
}

bool Gamepad::Release()
{
    return d->release();
}

} // namespace FMlib
