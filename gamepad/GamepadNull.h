#pragma once

#include "Gamepad.h"

#include <array>
#include <cstdint>
#include <optional>
#include <string_view>
#include <unordered_set>

namespace GamepadLib
{

class GamepadNull : public GamepadBackend
{
  public:
    GamepadNull() = default;
    ~GamepadNull() override = default;

    EGamepadState Init() override;
    const wchar_t* ProductName() const override;
    std::uint16_t VendorId() const override;
    std::uint16_t ProductId() const override;
    unsigned NumButtons() const override;
    unsigned NumAxes() const override;
    UnsignedPair PollingFrequency() const override;
    UnsignedPair AxisRange(EAxis axis) const override;
    bool HasAxis(EAxis axis) const override;
    EGamepadState ReadData() override;
    EGamepadState StateValue() const override;
    unsigned AxisValue(EAxis axis) const override;
    unsigned LastAxisValue(EAxis axis) const override;
    std::optional<int> RawAxisCode(EAxis axis) const override;
    const std::unordered_set<std::uint16_t>& CurrentButtons() const override;
    const std::unordered_set<std::uint16_t>& PreviousButtons() const override;
    unsigned Pov() const override;
    unsigned LastPov() const override;
    EPovType PovType() const override;
    int PressedCount() const override;
    unsigned Threshold() const override;
    bool SetThreshold(unsigned value) const override;
    bool Capture(void* windowHandle, unsigned int period, EUpdateAction when) override;
    bool Release() override;

  private:
    static constexpr std::wstring_view ProductNameValue = L"Gamepad unavailable on macOS";

    std::array<unsigned, EAxis::MaxValue> m_axisValues{
        Constants::MidPos,
        Constants::MidPos,
        Constants::MidPos,
        Constants::MidPos,
        Constants::MidPos,
        Constants::MidPos,
    };
    std::array<UnsignedPair, EAxis::MaxValue> m_ranges{
        UnsignedPair{ 0u, Constants::MaxPos },
        UnsignedPair{ 0u, Constants::MaxPos },
        UnsignedPair{ 0u, Constants::MaxPos },
        UnsignedPair{ 0u, Constants::MaxPos },
        UnsignedPair{ 0u, Constants::MaxPos },
        UnsignedPair{ 0u, Constants::MaxPos },
    };
    std::unordered_set<std::uint16_t> m_buttons;
    EGamepadState m_state{ EGamepadState::no_driver };
};

} // namespace GamepadLib
