// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include <array>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <memory>
#include <unordered_set>
#include <utility>
// Core types are declared here
#pragma once
#include "GamepadTypes.h"

namespace GamepadLib
{
class GamepadBackend
{
  public:
    virtual ~GamepadBackend() = default;

    virtual EGamepadState Init() = 0;
    virtual const wchar_t* ProductName() const = 0;
    virtual std::uint16_t VendorId() const = 0;
    virtual std::uint16_t ProductId() const = 0;
    virtual unsigned NumButtons() const = 0;
    virtual unsigned NumAxes() const = 0;
    virtual UnsignedPair PollingFrequency() const = 0;
    virtual UnsignedPair AxisRange(EAxis axis) const = 0;
    virtual bool HasAxis(EAxis axis) const = 0;
    virtual EGamepadState ReadData() = 0;
    virtual EGamepadState StateValue() const = 0;
    virtual unsigned AxisValue(EAxis axis) const = 0;
    virtual unsigned LastAxisValue(EAxis axis) const = 0;
    virtual const std::unordered_set<std::uint16_t>& CurrentButtons() const = 0;
    virtual const std::unordered_set<std::uint16_t>& PreviousButtons() const = 0;
    virtual unsigned Pov() const = 0;
    virtual unsigned LastPov() const = 0;
    virtual EPovType PovType() const = 0;
    virtual int PressedCount() const = 0;
    virtual unsigned Threshold() const = 0;
    virtual bool SetThreshold(unsigned value) const = 0;
    virtual bool Capture(void* windowHandle, unsigned int period, EUpdateAction when) = 0;
    virtual bool Release() = 0;
};

class Gamepad
{
  public:
    unsigned static ButtonCode(EButton button);

    Gamepad(std::unique_ptr<class GamepadBackend> impl = nullptr);
    ~Gamepad();

    Gamepad(Gamepad const&) = delete;
    Gamepad& operator=(Gamepad const&) = delete;
    Gamepad(Gamepad&&) noexcept;
    Gamepad& operator=(Gamepad&&) noexcept;

    EGamepadState Init();

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
    bool WasPressedRaw(std::uint16_t code) const;
    bool WasReleasedRaw(std::uint16_t code) const;
    bool IsPressedRaw(std::uint16_t code) const;
    const std::unordered_set<std::uint16_t>& CurrentButtons() const;
    const std::unordered_set<std::uint16_t>& PreviousButtons() const;

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

    EGamepadState ReadData();
    EGamepadState GetState() const;

    unsigned GetThreshold() const;
    bool SetThreshold(unsigned threshold) const;

    bool Capture(void* windowHandle, unsigned int period = 125,
                 EUpdateAction when = EUpdateAction::on_change);
    bool Release();

  private:
    unsigned applyInversion(EAxis axis, unsigned value) const;

    std::unique_ptr<GamepadBackend> m_impl;
    std::bitset<EAxis::MaxValue> m_invertedAxes{};
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

} // namespace GamepadLib
