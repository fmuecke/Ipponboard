#if defined(_WIN32)

#include "Gamepad.h"

#include <Windows.h>
#include <cstring>
#include <mmsystem.h>
#include <string>

#pragma comment(lib, "Winmm.lib")

namespace GamepadLib
{
class GamepadWin : public GamepadBackend
{
  public:
    GamepadWin();
    ~GamepadWin() override = default;

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
    EPovType PovType() const override { return EPovType::no_pov; } // TODO: check this!
    int PressedCount() const override;
    unsigned Threshold() const override;
    bool SetThreshold(unsigned thresholdValue) const override;
    bool Capture(void* windowHandle, unsigned int period, EUpdateAction when) override;
    bool Release() override;

  private:
    void reset();
    static EGamepadState toState(unsigned code);

    unsigned currentId{ 0 };
    std::wstring productName;
    JOYCAPSW caps{};
    JOYINFOEX data{};
    JOYINFOEX lastData{};
    EGamepadState state{ EGamepadState::unknown };
    HWND hwnd{ nullptr };
};

} // namespace GamepadLib

#endif // _WIN32
