#include "Gamepad.h"

#if defined(_WIN32)
#include "GamepadWinImpl.h"
#else
#include "GamepadLinuxImpl.h"
#endif
#include "JessTechPadAdapter.h"

#include <algorithm>
#include <array>
#include <bitset>
#include <cmath>
#include <cstring>
#include <string>

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

} // namespace

unsigned Gamepad::ButtonCode(Gamepad::EButton button)
{
    return kButtonCodes[static_cast<std::size_t>(button)];
}

Gamepad::Gamepad(std::unique_ptr<GamepadImpl> impl) : m_impl(std::move(impl)) { Init(); }
Gamepad::~Gamepad() = default;
Gamepad::Gamepad(Gamepad&&) noexcept = default;
Gamepad& Gamepad::operator=(Gamepad&&) noexcept = default;

Gamepad::EState Gamepad::Init()
{
    m_invertedAxes.reset();
    return m_impl->Init();
}

const wchar_t* Gamepad::GetProductName() const { return m_impl->ProductName(); }

std::uint16_t Gamepad::GetMId() const { return m_impl->VendorId(); }

std::uint16_t Gamepad::GetPId() const { return m_impl->ProductId(); }

unsigned Gamepad::GetNumButtons() const { return m_impl->NumButtons(); }

unsigned Gamepad::GetNumAxes() const { return m_impl->NumAxes(); }

Gamepad::UnsignedPair Gamepad::GetPollingFreq() const { return m_impl->PollingFrequency(); }

Gamepad::UnsignedPair Gamepad::GetRangeX() const { return m_impl->AxisRange(eAxis_X); }

Gamepad::UnsignedPair Gamepad::GetRangeY() const { return m_impl->AxisRange(eAxis_Y); }

Gamepad::UnsignedPair Gamepad::GetRangeZ() const { return m_impl->AxisRange(eAxis_Z); }

Gamepad::UnsignedPair Gamepad::GetRangeR() const { return m_impl->AxisRange(eAxis_R); }

Gamepad::UnsignedPair Gamepad::GetRangeU() const { return m_impl->AxisRange(eAxis_U); }

Gamepad::UnsignedPair Gamepad::GetRangeV() const { return m_impl->AxisRange(eAxis_V); }

bool Gamepad::HasAxisZ() const { return m_impl->HasAxis(eAxis_Z); }

bool Gamepad::HasAxisR() const { return m_impl->HasAxis(eAxis_R); }

bool Gamepad::HasAxisU() const { return m_impl->HasAxis(eAxis_U); }

bool Gamepad::HasAxisV() const { return m_impl->HasAxis(eAxis_V); }

Gamepad::EPovType Gamepad::GetPovType() const { return m_impl->PovType(); }

void Gamepad::SetInverted(EAxis axis, bool value) { m_invertedAxes.set(axis, value); }

bool Gamepad::IsInverted(EAxis axis) const { return m_invertedAxes.test(axis); }

bool Gamepad::WasPressed(EButton button) const
{
    const auto code = ButtonCode(button);
    if (button <= eButton32)
    {
        return (m_impl->LastButtonsMask() & code) == 0 && (m_impl->ButtonsMask() & code) != 0;
    }

    if (code == kPovCentered)
    {
        return false;
    }

    return m_impl->LastPov() != code && m_impl->Pov() == code;
}

bool Gamepad::WasReleased(EButton button) const
{
    const auto code = ButtonCode(button);
    if (button <= eButton32)
    {
        return (m_impl->LastButtonsMask() & code) != 0 && (m_impl->ButtonsMask() & code) == 0;
    }

    if (code == kPovCentered)
    {
        return false;
    }

    return m_impl->LastPov() == code && m_impl->Pov() != code;
}

bool Gamepad::IsPressed(EButton button) const
{
    const auto code = ButtonCode(button);
    if (button <= eButton32)
    {
        return (m_impl->ButtonsMask() & code) != 0;
    }

    if (code == kPovCentered)
    {
        return false;
    }

    return m_impl->Pov() == code;
}

bool Gamepad::WasSectionEnteredXY(float min, float max) const
{
    const auto lastX = static_cast<int>(applyInversion(eAxis_X, m_impl->LastAxisValue(eAxis_X)));
    const auto lastY = static_cast<int>(applyInversion(eAxis_Y, m_impl->LastAxisValue(eAxis_Y)));
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
    const auto lastR = static_cast<int>(applyInversion(eAxis_R, m_impl->LastAxisValue(eAxis_R)));
    const auto lastZ = static_cast<int>(applyInversion(eAxis_Z, m_impl->LastAxisValue(eAxis_Z)));
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

int Gamepad::PressedCount() const { return m_impl->PressedCount(); }

unsigned Gamepad::applyInversion(EAxis axis, unsigned value) const
{
    return m_invertedAxes.test(axis) ? eMax - value : value;
}

unsigned Gamepad::GetXPos() const { return applyInversion(eAxis_X, m_impl->AxisValue(eAxis_X)); }

unsigned Gamepad::GetYPos() const { return applyInversion(eAxis_Y, m_impl->AxisValue(eAxis_Y)); }

unsigned Gamepad::GetZPos() const { return applyInversion(eAxis_Z, m_impl->AxisValue(eAxis_Z)); }

unsigned Gamepad::GetRPos() const { return applyInversion(eAxis_R, m_impl->AxisValue(eAxis_R)); }

unsigned Gamepad::GetUPos() const { return applyInversion(eAxis_U, m_impl->AxisValue(eAxis_U)); }

unsigned Gamepad::GetVPos() const { return applyInversion(eAxis_V, m_impl->AxisValue(eAxis_V)); }

unsigned Gamepad::GetPOV() const { return m_impl->Pov(); }

Gamepad::EState Gamepad::ReadData() { return m_impl->ReadData(); }

Gamepad::EState Gamepad::GetState() const { return m_impl->StateValue(); }

unsigned Gamepad::GetThreshold() const { return m_impl->Threshold(); }

bool Gamepad::SetThreshold(unsigned threshold) const { return m_impl->SetThreshold(threshold); }

bool Gamepad::Capture(void* windowHandle, unsigned int period, EUpdateAction when)
{
    return m_impl->Capture(windowHandle, period, when);
}

bool Gamepad::Release() { return m_impl->Release(); }

} // namespace FMlib
