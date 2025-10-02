#if defined(_WIN32)

#include "IGamepadImpl.h"

#include <Windows.h>
#include <cstring>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "Winmm.lib")

namespace FMlib
{
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
    Gamepad::EPovType PovType() const override { return Gamepad::ePovType_no_pov; } // TODO: check this!
    int PressedCount() const override;
    unsigned Threshold() const override;
    bool SetThreshold(unsigned thresholdValue) const override;
    bool Capture(void* windowHandle, unsigned int period, Gamepad::EUpdateAction when) override;
    bool Release() override;

  private:
    void reset();
    static Gamepad::EState toState(unsigned code);

    unsigned currentId{ 0 };
    std::wstring productName;
    JOYCAPSW caps{};
    JOYINFOEX data{};
    JOYINFOEX lastData{};
    Gamepad::EState state{ Gamepad::eState_unknown };
    HWND hwnd{ nullptr };
};

} // namespace FMlib

#endif // _WIN32
