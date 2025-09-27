// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#include <bitset>
#include <cmath>
#include <cstdint>
#include <memory>
#include <utility>

namespace FMlib
{
class Gamepad
{
  public:
    using UnsignedPair = std::pair<unsigned, unsigned>;

    static constexpr unsigned kMaxButtons = 40;
    static constexpr unsigned eMax = 65535;
    static constexpr unsigned eMid = 32767;
    static constexpr unsigned eTolerance = 100;
    static constexpr unsigned kPovCentered = 0xFFFF;

    enum EButton
    {
        eButton1 = 0,
        eButton2,
        eButton3,
        eButton4,
        eButton5,
        eButton6,
        eButton7,
        eButton8,
        eButton9,
        eButton10,
        eButton11,
        eButton12,
        eButton13,
        eButton14,
        eButton15,
        eButton16,
        eButton17,
        eButton18,
        eButton19,
        eButton20,
        eButton21,
        eButton22,
        eButton23,
        eButton24,
        eButton25,
        eButton26,
        eButton27,
        eButton28,
        eButton29,
        eButton30,
        eButton31,
        eButton32,
        eButton_pov_fwd,
        eButton_pov_right,
        eButton_pov_back,
        eButton_pov_left,
        eButton_pov_right_fwd,
        eButton_pov_right_back,
        eButton_pov_left_back,
        eButton_pov_left_fwd
    };

    enum EState
    {
        eState_unknown = -1,
        eState_ok = 0,
        eState_no_driver,
        eState_invalid_param,
        eState_bad_device_id,
        eState_no_service,
        eState_unplugged,
        eState_invalid_id
    };

    enum EUpdateAction
    {
        update_always = 0,
        update_on_change = 1
    };

    enum EPovType
    {
        ePovType_unknown = -1,
        ePovType_no_pov = 0,
        ePovType_discrete,
        ePovType_continuous
    };

    enum EAxis
    {
        eAxis_X = 0,
        eAxis_Y,
        eAxis_Z,
        eAxis_R,
        eAxis_U,
        eAxis_V,
        eAxis_MAX
    };

    Gamepad();
    ~Gamepad();

    Gamepad(Gamepad const&) = delete;
    Gamepad& operator=(Gamepad const&) = delete;
    Gamepad(Gamepad&&) noexcept;
    Gamepad& operator=(Gamepad&&) noexcept;

    EState Init();

    const wchar_t* GetProductName() const;
    std::uint16_t GetMId() const;
    std::uint16_t GetPId() const;
    unsigned GetNumButtons() const;
    unsigned GetNumAxes() const;
    UnsignedPair GetPollingFreq() const;
    UnsignedPair GetRangeX() const;
    UnsignedPair GetRangeY() const;
    UnsignedPair GetRangeZ() const;
    UnsignedPair GetRangeR() const;
    UnsignedPair GetRangeU() const;
    UnsignedPair GetRangeV() const;

    bool HasAxisZ() const;
    bool HasAxisR() const;
    bool HasAxisU() const;
    bool HasAxisV() const;

    EPovType GetPovType() const;

    void SetInverted(EAxis axis, bool value = true);
    bool IsInverted(EAxis axis) const;

    bool WasPressed(EButton button) const;
    bool WasReleased(EButton button) const;
    bool IsPressed(EButton button) const;

    bool WasSectionEnteredXY(float min, float max) const;
    bool WasSectionEnteredRZ(float min, float max) const;
    bool WasSectionLeft(float min, float max) const;
    bool IsInSection(float alpha, float min, float max) const;

    template <class T> T GetAngle(int x, int y, float factor = 0.0f, int max = 0) const;

    float GetAngleXY() const;
    float GetAngleRZ() const;

    int PressedCount() const;

    unsigned GetXPos() const;
    unsigned GetYPos() const;
    unsigned GetZPos() const;
    unsigned GetRPos() const;
    unsigned GetUPos() const;
    unsigned GetVPos() const;
    unsigned GetPOV() const;

    EState ReadData();
    EState GetState() const;

    unsigned GetThreshold() const;
    bool SetThreshold(unsigned threshold) const;

    bool Capture(void* windowHandle, unsigned int period = 125,
                 EUpdateAction when = update_on_change);
    bool Release();

  private:
    unsigned applyInversion(EAxis axis, unsigned value) const;

    struct Impl;
    std::unique_ptr<Impl> d;
    std::bitset<eAxis_MAX> m_invertedAxes{};
};

template <class T> inline T Gamepad::GetAngle(int x, int y, float factor, int max) const
{
    if (x == 0 && y == 0)
    {
        return static_cast<T>(-1);
    }

    const auto hyp = std::sqrt(static_cast<T>(x) * x + static_cast<T>(y) * y);

    if (hyp * factor < static_cast<float>(max))
    {
        return static_cast<T>(-1);
    }

    T angle = std::asin(static_cast<T>(y) / hyp) * static_cast<T>(180.0L / 3.141592653589793L);
    angle = x > 0 ? static_cast<T>(90) - angle : static_cast<T>(270) + angle;
    return angle;
}

} // namespace FMlib
