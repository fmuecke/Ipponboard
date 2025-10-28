// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include <array>
#include <bitset>
#include <cmath>
#include <cstdint>
#include <memory>
#include <optional>
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
    virtual std::optional<int> RawAxisCode(EAxis axis) const = 0;
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
    UnsignedPair GetRange(EAxis axis) const;

    bool HasAxis(EAxis axis) const;

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

    static unsigned char GetMappedSection(int x, int y);
    unsigned char GetSection(EAxis axis1, EAxis axis2) const;
    unsigned char GetLastSection(EAxis axis1, EAxis axis2) const;

    static float GetAngle(int x, int y);

    int PressedCount() const;

    unsigned GetPos(EAxis axis) const;
    unsigned GetPOV() const;
    unsigned AxisValueRaw(EAxis axis) const;
    unsigned LastAxisValueRaw(EAxis axis) const;
    std::optional<int> RawAxisCode(EAxis axis) const;

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

} // namespace GamepadLib
