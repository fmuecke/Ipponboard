#ifndef _WIN32

#pragma once

#include "Gamepad.h"

#include <array>
#include <cstdint>
#include <linux/input.h>
#include <optional>
#include <string>
#include <unordered_set>

namespace GamepadLib
{

class GamepadLinux : public GamepadBackend
{
  public:
    GamepadLinux();
    ~GamepadLinux() override;

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
    bool Capture(void*, unsigned int, EUpdateAction) override;
    bool Release() override;

  private:
    using AxisArray = std::array<unsigned, EAxis::MaxValue>;

    void reset();
    void closeDevice();
    bool openDevice();
    void queryDeviceInfo();
    void queryAxisInfo();
    void handleEvent(const input_event& ev);
    void handleKeyEvent(const input_event& ev);
    void handleAbsEvent(const input_event& ev);
    void updatePov();
    std::optional<std::string> findDevicePath() const;

    int m_fd{ -1 };
    std::string m_devicePath;
    std::wstring m_productName;
    std::uint16_t m_vendorId{ 0 };
    std::uint16_t m_productId{ 0 };
    unsigned m_buttonCount{ 0 };
    unsigned m_axisCount{ 0 };
    EGamepadState m_state{ EGamepadState::unknown };

    std::array<UnsignedPair, EAxis::MaxValue> m_ranges{};
    AxisArray m_axes{};
    AxisArray m_lastAxes{};
    std::array<bool, EAxis::MaxValue> m_axisSupported{};
    std::array<int, EAxis::MaxValue> m_axisCodes{};
    std::array<int, EAxis::MaxValue> m_axisMin{};
    std::array<int, EAxis::MaxValue> m_axisMax{};

    std::unordered_set<std::uint16_t> m_buttons;
    std::unordered_set<std::uint16_t> m_lastButtons;
    std::unordered_set<std::uint16_t> m_knownButtons;

    int m_hatX{ 0 };
    int m_hatY{ 0 };
    unsigned m_pov{ Constants::PovCenteredVal };
    unsigned m_lastPov{ Constants::PovCenteredVal };
};

} // namespace GamepadLib

#endif
