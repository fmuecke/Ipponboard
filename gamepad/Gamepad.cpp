#include "Gamepad.h"

#if defined(_WIN32)
#include "GamepadWin.h"
#else
#include "GamepadLinux.h"
#endif

#include <algorithm>
#include <cmath>
#include <string>
#include <unordered_set>

namespace GamepadLib
{
namespace
{
constexpr std::uint16_t encode_pov_button(EButton button) noexcept
{
    using Constants::PovCodeBase;

    switch (button)
    {
    case EButton::button_pov_fwd:
        return PovCodeBase + 0;
    case EButton::button_pov_right:
        return PovCodeBase + 1;
    case EButton::button_pov_back:
        return PovCodeBase + 2;
    case EButton::button_pov_left:
        return PovCodeBase + 3;
    case EButton::button_pov_right_fwd:
        return PovCodeBase + 4;
    case EButton::button_pov_right_back:
        return PovCodeBase + 5;
    case EButton::button_pov_left_back:
        return PovCodeBase + 6;
    case EButton::button_pov_left_fwd:
        return PovCodeBase + 7;
    default:
        return Constants::PovCenteredVal;
    }
}

std::unique_ptr<GamepadBackend> make_default_backend()
{
#if defined(_WIN32)
    return std::make_unique<GamepadWin>();
#else
    return std::make_unique<GamepadLinux>();
#endif
}
} // namespace

unsigned Gamepad::ButtonCode(EButton button)
{
    switch (button)
    {
    case EButton::button_pov_fwd:
    case EButton::button_pov_right:
    case EButton::button_pov_back:
    case EButton::button_pov_left:
    case EButton::button_pov_right_fwd:
    case EButton::button_pov_right_back:
    case EButton::button_pov_left_back:
    case EButton::button_pov_left_fwd:
        return encode_pov_button(button);
    default:
        return static_cast<unsigned>(button);
    }
}

Gamepad::Gamepad(std::unique_ptr<GamepadBackend> impl)
    : m_impl(impl ? std::move(impl) : make_default_backend())
{
    Init();
}
Gamepad::~Gamepad() = default;
Gamepad::Gamepad(Gamepad&&) noexcept = default;
Gamepad& Gamepad::operator=(Gamepad&&) noexcept = default;

EGamepadState Gamepad::Init()
{
    m_invertedAxes.reset();
    return m_impl->Init();
}

const wchar_t* Gamepad::GetProductName() const { return m_impl->ProductName(); }

std::uint16_t Gamepad::GetMId() const { return m_impl->VendorId(); }

std::uint16_t Gamepad::GetPId() const { return m_impl->ProductId(); }

unsigned Gamepad::GetNumButtons() const { return m_impl->NumButtons(); }

unsigned Gamepad::GetNumAxes() const { return m_impl->NumAxes(); }

UnsignedPair Gamepad::GetPollingFreq() const { return m_impl->PollingFrequency(); }

UnsignedPair Gamepad::GetRange(EAxis axis) const { return m_impl->AxisRange(axis); }

bool Gamepad::HasAxis(EAxis axis) const { return m_impl->HasAxis(axis); }

EPovType Gamepad::GetPovType() const { return m_impl->PovType(); }

void Gamepad::SetInverted(EAxis axis, bool value) { m_invertedAxes.set(axis, value); }

bool Gamepad::IsInverted(EAxis axis) const { return m_invertedAxes.test(axis); }

bool Gamepad::WasPressed(EButton button) const
{
    return WasPressedRaw(static_cast<std::uint16_t>(ButtonCode(button)));
}

bool Gamepad::WasReleased(EButton button) const
{
    return WasReleasedRaw(static_cast<std::uint16_t>(ButtonCode(button)));
}

bool Gamepad::IsPressed(EButton button) const
{
    return IsPressedRaw(static_cast<std::uint16_t>(ButtonCode(button)));
}

bool Gamepad::WasPressedRaw(std::uint16_t code) const
{
    if (code == Constants::PovCenteredVal)
    {
        return false;
    }

    const auto& previous = m_impl->PreviousButtons();
    const auto& current = m_impl->CurrentButtons();
    const bool wasPressed = previous.find(code) != previous.end();
    const bool isPressed = current.find(code) != current.end();

    return !wasPressed && isPressed;
}

bool Gamepad::WasReleasedRaw(std::uint16_t code) const
{
    if (code == Constants::PovCenteredVal)
    {
        return false;
    }

    const auto& previous = m_impl->PreviousButtons();
    const auto& current = m_impl->CurrentButtons();
    const bool wasPressed = previous.find(code) != previous.end();
    const bool isPressed = current.find(code) != current.end();

    return wasPressed && !isPressed;
}

bool Gamepad::IsPressedRaw(std::uint16_t code) const
{
    if (code == Constants::PovCenteredVal)
    {
        return false;
    }

    const auto& current = m_impl->CurrentButtons();
    return current.find(code) != current.end();
}

const std::unordered_set<std::uint16_t>& Gamepad::CurrentButtons() const
{
    return m_impl->CurrentButtons();
}

const std::unordered_set<std::uint16_t>& Gamepad::PreviousButtons() const
{
    return m_impl->PreviousButtons();
}

unsigned char Gamepad::GetMappedSection(int x, int y)
{
    /* 
        Sector layout, mapped to 8 sectors and 0 in between:

             6 6 . 7 7 7 . 8 8
             6 6 . . 7 . . 8 8
             . . . . . . . . .
             4 . . . . . . . 5
             4 4 . . 0 . . 5 5
             4 . . . . . . . 5
     y-axis  . . . . . . . . .
        ^    1 1 . . 2 . . 3 3
        |    1 1 . 2 2 2 . 3 3
        |
         ----->  x-axis
    */

    // Lookup table for sectors based on 9x9 grid (stored as [x][y])
    constexpr unsigned char SectorLUT[9][9] = {
        { 1, 1, 0, 4, 4, 4, 0, 6, 6 }, { 1, 1, 0, 0, 4, 0, 0, 6, 6 }, { 0, 0, 0, 0, 0, 0, 0, 0, 0 },
        { 2, 0, 0, 0, 0, 0, 0, 0, 7 }, { 2, 2, 0, 0, 0, 0, 0, 7, 7 }, { 2, 0, 0, 0, 0, 0, 0, 0, 7 },
        { 0, 0, 0, 0, 0, 0, 0, 0, 0 }, { 3, 3, 0, 0, 5, 0, 0, 8, 8 }, { 3, 3, 0, 5, 5, 5, 0, 8, 8 }
    };

    constexpr auto max = Constants::MaxPos;
    auto xVal = 0;
    if (x > max * 8 / 9)
        xVal = 8;
    else if (x > max * 7 / 9)
        xVal = 7;
    else if (x > max * 6 / 9)
        xVal = 6;
    else if (x > max * 5 / 9)
        xVal = 5;
    else if (x > max * 4 / 9)
        xVal = 4;
    else if (x > max * 3 / 9)
        xVal = 3;
    else if (x > max * 2 / 9)
        xVal = 2;
    else if (x > max * 1 / 9)
        xVal = 1;

    auto yVal = 0;
    if (y > max * 8 / 9)
        yVal = 8;
    else if (y > max * 7 / 9)
        yVal = 7;
    else if (y > max * 6 / 9)
        yVal = 6;
    else if (y > max * 5 / 9)
        yVal = 5;
    else if (y > max * 4 / 9)
        yVal = 4;
    else if (y > max * 3 / 9)
        yVal = 3;
    else if (y > max * 2 / 9)
        yVal = 2;
    else if (y > max * 1 / 9)
        yVal = 1;

    return SectorLUT[xVal][yVal];
}

unsigned char Gamepad::GetSection(EAxis axis1, EAxis axis2) const
{
    const auto x = static_cast<int>(GetPos(axis1));
    const auto y = static_cast<int>(GetPos(axis2));
    return GetMappedSection(x, y);
}

unsigned char Gamepad::GetLastSection(EAxis axis1, EAxis axis2) const
{
    const auto lastX = static_cast<int>(applyInversion(axis1, m_impl->LastAxisValue(axis1)));
    const auto lastY = static_cast<int>(applyInversion(axis2, m_impl->LastAxisValue(axis2)));

    return GetMappedSection(lastX, lastY);
}

int Gamepad::PressedCount() const { return static_cast<int>(m_impl->CurrentButtons().size()); }

unsigned Gamepad::applyInversion(EAxis axis, unsigned value) const
{
    return m_invertedAxes.test(axis) ? Constants::MaxPos - value : value;
}

unsigned Gamepad::GetPos(EAxis axis) const { return applyInversion(axis, m_impl->AxisValue(axis)); }

unsigned Gamepad::GetPOV() const { return m_impl->Pov(); }

unsigned Gamepad::AxisValueRaw(EAxis axis) const { return m_impl->AxisValue(axis); }

unsigned Gamepad::LastAxisValueRaw(EAxis axis) const { return m_impl->LastAxisValue(axis); }

std::optional<int> Gamepad::RawAxisCode(EAxis axis) const { return m_impl->RawAxisCode(axis); }

EGamepadState Gamepad::ReadData() { return m_impl->ReadData(); }

EGamepadState Gamepad::GetState() const { return m_impl->StateValue(); }

unsigned Gamepad::GetThreshold() const { return m_impl->Threshold(); }

bool Gamepad::SetThreshold(unsigned threshold) const { return m_impl->SetThreshold(threshold); }

bool Gamepad::Capture(void* windowHandle, unsigned int period, EUpdateAction when)
{
    return m_impl->Capture(windowHandle, period, when);
}

bool Gamepad::Release() { return m_impl->Release(); }

} // namespace GamepadLib
