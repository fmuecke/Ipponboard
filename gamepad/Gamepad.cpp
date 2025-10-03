#include "Gamepad.h"

#if defined(_WIN32)
#include "GamepadWin.h"
#else
#include "GamepadLinux.h"
#endif
#include "JessTechPadAdapter.h"

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <cstring>
#include <string>

namespace GamepadLib
{

constexpr std::array<unsigned, Constants::MaxButtons> makeButtonCodes()
{
    std::array<unsigned, Constants::MaxButtons> codes{};
    for (std::size_t i = 0; i < 32; ++i)
    {
        codes[i] = 1u << i;
    }

    codes[EButton::button_pov_fwd] = 0;
    codes[EButton::button_pov_right] = 9000;
    codes[EButton::button_pov_back] = 18000;
    codes[EButton::button_pov_left] = 27000;
    codes[EButton::button_pov_right_fwd] = 4500;
    codes[EButton::button_pov_right_back] = 13500;
    codes[EButton::button_pov_left_back] = 22500;
    codes[EButton::button_pov_left_fwd] = 31500;

    return codes;
}

constexpr auto kButtonCodes = makeButtonCodes();

static std::unique_ptr<GamepadBackend> make_default_backend()
{
#if defined(_WIN32)
    return std::make_unique<GamepadWin>();
#else
    return std::make_unique<GamepadLinux>();
#endif
}

unsigned Gamepad::ButtonCode(EButton button)
{
    return kButtonCodes[static_cast<std::size_t>(button)];
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

UnsignedPair Gamepad::GetRangeX() const { return m_impl->AxisRange(EAxis::X); }

UnsignedPair Gamepad::GetRangeY() const { return m_impl->AxisRange(EAxis::Y); }

UnsignedPair Gamepad::GetRangeZ() const { return m_impl->AxisRange(EAxis::Z); }

UnsignedPair Gamepad::GetRangeR() const { return m_impl->AxisRange(EAxis::R); }

UnsignedPair Gamepad::GetRangeU() const { return m_impl->AxisRange(EAxis::U); }

UnsignedPair Gamepad::GetRangeV() const { return m_impl->AxisRange(EAxis::V); }

bool Gamepad::HasAxisZ() const { return m_impl->HasAxis(EAxis::Z); }

bool Gamepad::HasAxisR() const { return m_impl->HasAxis(EAxis::R); }

bool Gamepad::HasAxisU() const { return m_impl->HasAxis(EAxis::U); }

bool Gamepad::HasAxisV() const { return m_impl->HasAxis(EAxis::V); }

EPovType Gamepad::GetPovType() const { return m_impl->PovType(); }

void Gamepad::SetInverted(EAxis axis, bool value) { m_invertedAxes.set(axis, value); }

bool Gamepad::IsInverted(EAxis axis) const { return m_invertedAxes.test(axis); }

bool Gamepad::WasPressed(EButton button) const
{
    const auto code = ButtonCode(button);
    if (button <= EButton::button32)
    {
        return (m_impl->LastButtonsMask() & code) == 0 && (m_impl->ButtonsMask() & code) != 0;
    }

    if (code == Constants::PovCenteredVal)
    {
        return false;
    }

    return m_impl->LastPov() != code && m_impl->Pov() == code;
}

bool Gamepad::WasReleased(EButton button) const
{
    const auto code = ButtonCode(button);
    if (button <= EButton::button32)
    {
        return (m_impl->LastButtonsMask() & code) != 0 && (m_impl->ButtonsMask() & code) == 0;
    }

    if (code == Constants::PovCenteredVal)
    {
        return false;
    }

    return m_impl->LastPov() == code && m_impl->Pov() != code;
}

bool Gamepad::IsPressed(EButton button) const
{
    const auto code = ButtonCode(button);
    if (button <= EButton::button32)
    {
        return (m_impl->ButtonsMask() & code) != 0;
    }

    if (code == Constants::PovCenteredVal)
    {
        return false;
    }

    return m_impl->Pov() == code;
}

bool Gamepad::WasSectionEnteredXY(float min, float max) const
{
    const auto lastX = static_cast<int>(applyInversion(EAxis::X, m_impl->LastAxisValue(EAxis::X)));
    const auto lastY = static_cast<int>(applyInversion(EAxis::Y, m_impl->LastAxisValue(EAxis::Y)));
    const auto curX = static_cast<int>(GetXPos());
    const auto curY = static_cast<int>(GetYPos());

    const auto lastAlpha = GetAngle<float>(lastX - static_cast<int>(Constants::MidAngle),
                                           lastY - static_cast<int>(Constants::MidAngle),
                                           1.02f,
                                           Constants::MidAngle);
    const auto curAlpha = GetAngle<float>(curX - static_cast<int>(Constants::MidAngle),
                                          curY - static_cast<int>(Constants::MidAngle),
                                          1.02f,
                                          Constants::MidAngle);

    const bool changed =
        (lastAlpha < 0.0f || curAlpha < 0.0f ||
         std::fabs(lastAlpha - curAlpha) > static_cast<float>(Constants::ToleranceVal));
    return changed && IsInSection(curAlpha, min, max);
}

bool Gamepad::WasSectionEnteredRZ(float min, float max) const
{
    const auto lastR = static_cast<int>(applyInversion(EAxis::R, m_impl->LastAxisValue(EAxis::R)));
    const auto lastZ = static_cast<int>(applyInversion(EAxis::Z, m_impl->LastAxisValue(EAxis::Z)));
    const auto curR = static_cast<int>(GetRPos());
    const auto curZ = static_cast<int>(GetZPos());

    const auto lastAlpha = GetAngle<float>(lastR - static_cast<int>(Constants::MidAngle),
                                           lastZ - static_cast<int>(Constants::MidAngle),
                                           1.02f,
                                           Constants::MidAngle);
    const auto curAlpha = GetAngle<float>(curR - static_cast<int>(Constants::MidAngle),
                                          curZ - static_cast<int>(Constants::MidAngle),
                                          1.02f,
                                          Constants::MidAngle);

    const bool changed =
        (lastAlpha < 0.0f || curAlpha < 0.0f ||
         std::fabs(lastAlpha - curAlpha) > static_cast<float>(Constants::ToleranceVal));
    return changed && IsInSection(curAlpha, min, max);
}

bool Gamepad::WasSectionLeft(float, float) const { return false; }

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
    return GetAngle<float>(static_cast<int>(GetXPos()) - static_cast<int>(Constants::MidAngle),
                           static_cast<int>(GetYPos()) - static_cast<int>(Constants::MidAngle),
                           1.2f,
                           Constants::MidAngle);
}

float Gamepad::GetAngleRZ() const
{
    return GetAngle<float>(static_cast<int>(GetRPos()) - static_cast<int>(Constants::MidAngle),
                           static_cast<int>(GetZPos()) - static_cast<int>(Constants::MidAngle),
                           1.2f,
                           Constants::MidAngle);
}

int Gamepad::PressedCount() const { return m_impl->PressedCount(); }

unsigned Gamepad::applyInversion(EAxis axis, unsigned value) const
{
    return m_invertedAxes.test(axis) ? Constants::MaxAngle - value : value;
}

unsigned Gamepad::GetXPos() const { return applyInversion(EAxis::X, m_impl->AxisValue(EAxis::X)); }

unsigned Gamepad::GetYPos() const { return applyInversion(EAxis::Y, m_impl->AxisValue(EAxis::Y)); }

unsigned Gamepad::GetZPos() const { return applyInversion(EAxis::Z, m_impl->AxisValue(EAxis::Z)); }

unsigned Gamepad::GetRPos() const { return applyInversion(EAxis::R, m_impl->AxisValue(EAxis::R)); }

unsigned Gamepad::GetUPos() const { return applyInversion(EAxis::U, m_impl->AxisValue(EAxis::U)); }

unsigned Gamepad::GetVPos() const { return applyInversion(EAxis::V, m_impl->AxisValue(EAxis::V)); }

unsigned Gamepad::GetPOV() const { return m_impl->Pov(); }

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
