#ifndef _WIN32 // Linux / Qt implementation

#pragma once

#include "Gamepad.h"

#include <QGamepad>
#include <QGamepadManager>
#include <array>
#include <memory>

namespace GamepadLib
{

class GamepadLinux : public GamepadBackend
{
  public:
    GamepadLinux();
    ~GamepadLinux() override = default;
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
    std::uint32_t ButtonsMask() const override;
    std::uint32_t LastButtonsMask() const override;
    unsigned Pov() const override;
    unsigned LastPov() const override;
    EPovType PovType() const override;
    int PressedCount() const override;
    unsigned Threshold() const override;
    bool SetThreshold(unsigned value) const override;
    bool Capture(void* windowHandle, unsigned int period, EUpdateAction when) override;
    bool Release() override;

  private:
    static bool isDebugLoggingEnabled();
    static const char* buttonName(QGamepadManager::GamepadButton button);
    static const char* axisName(QGamepadManager::GamepadAxis axis);
    static void ensureDebugConnections();
    static bool hasSignificantDelta(double lhs, double rhs);
    void debugLogRawState() const;
    std::array<unsigned, EAxis::MaxValue> sampleAxes() const;
    std::uint32_t sampleButtons() const;
    unsigned computePov() const;
    void resetState();
    void refreshMetadata();
    void updateConnection();

    static unsigned toUnsignedAxis(double value);
    static unsigned toUnsignedTrigger(double value);
    static void setButton(std::uint32_t& mask, EButton button, bool pressed);

    int deviceId{ -1 };
    std::unique_ptr<QGamepad> gamepad;
    std::unique_ptr<class JessTechPadAdapter> jessTech;
    std::wstring productName;
    std::uint16_t vendorId{ 0 };
    std::uint16_t productId{ 0 };
    unsigned buttonCount{ 0 };
    unsigned axisCount{ 0 };
    EGamepadState state{ EGamepadState::unknown };
    std::array<UnsignedPair, EAxis::MaxValue> ranges{};
    std::array<unsigned, EAxis::MaxValue> axes{};
    std::array<unsigned, EAxis::MaxValue> lastAxes{};
    std::array<bool, EAxis::MaxValue> axisSupported{};
    std::uint32_t buttons{ 0 };
    std::uint32_t lastButtons{ 0 };
    unsigned povValue{ Constants::PovCenteredVal };
    unsigned lastPovValue{ Constants::PovCenteredVal };
    bool dpadPresent{ false };
};

} // namespace GamepadLib

#endif