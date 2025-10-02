#ifndef _WIN32 // Linux / Qt implementation

#pragma once

#include "IGamepadImpl.h"
//#include "JessTechPadAdapter.h"

// #include <QDebug>
#include <QGamepad>
#include <QGamepadManager>
// #include <QList>
// #include <QObject>
// #include <QProcessEnvironment>
// #include <QStringList>

namespace FMlib
{

//constexpr std::size_t axisIndex(Gamepad::EAxis axis) { return static_cast<std::size_t>(axis); }

//unsigned popcount(std::uint32_t value)

class GamepadImpl : public IGamepadImpl
{
  public:
    GamepadImpl();
    ~GamepadImpl() override = default;
    Gamepad::EState Init() override;
    const wchar_t* ProductName() const override;
    std::uint16_t VendorId() const override;
    std::uint16_t ProductId() const override;
    unsigned NumButtons() const override;
    unsigned NumAxes() const override;
    Gamepad::UnsignedPair PollingFrequency() const override;
    Gamepad::UnsignedPair AxisRange(Gamepad::EAxis axis) const override;
    bool HasAxis(Gamepad::EAxis axis) const override;
    Gamepad::EState ReadData() override;
    Gamepad::EState StateValue() const override;
    unsigned AxisValue(Gamepad::EAxis axis) const override;
    unsigned LastAxisValue(Gamepad::EAxis axis) const override;
    std::uint32_t ButtonsMask() const override;
    std::uint32_t LastButtonsMask() const override;
    unsigned Pov() const override;
    unsigned LastPov() const override;
    Gamepad::EPovType PovType() const override;
    int PressedCount() const override;
    unsigned Threshold() const override;
    bool SetThreshold(unsigned value) const override;
    bool Capture(void* windowHandle, unsigned int period, Gamepad::EUpdateAction when) override;
    bool Release() override;

  private:
    static bool isDebugLoggingEnabled();
    static const char* buttonName(QGamepadManager::GamepadButton button);
    static const char* axisName(QGamepadManager::GamepadAxis axis);
    static void ensureDebugConnections();
    static bool hasSignificantDelta(double lhs, double rhs);
    void debugLogRawState() const;
    std::array<unsigned, Gamepad::eAxis_MAX> sampleAxes() const;
    std::uint32_t sampleButtons() const;
    unsigned computePov() const;
    void resetState();
    void refreshMetadata();
    void updateConnection();

    static unsigned toUnsignedAxis(double value);
    static unsigned toUnsignedTrigger(double value);
    static void setButton(std::uint32_t& mask, Gamepad::EButton button, bool pressed);

    int deviceId{ -1 };
    std::unique_ptr<QGamepad> gamepad;
    std::unique_ptr<class JessTechPadAdapter> jessTech;
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

} // namespace FMlib

#endif