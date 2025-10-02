// Copyright 2025 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#include "Gamepad.h"

namespace FMlib
{

class IGamepadImpl
{
  public:
    virtual inline ~IGamepadImpl() = 0;

    virtual Gamepad::EState Init() = 0;
    virtual const wchar_t* ProductName() const = 0;
    virtual std::uint16_t VendorId() const = 0;
    virtual std::uint16_t ProductId() const = 0;
    virtual unsigned NumButtons() const = 0;
    virtual unsigned NumAxes() const = 0;
    virtual Gamepad::UnsignedPair PollingFrequency() const = 0;
    virtual Gamepad::UnsignedPair AxisRange(Gamepad::EAxis axis) const = 0;
    virtual bool HasAxis(Gamepad::EAxis axis) const = 0;
    virtual Gamepad::EState ReadData() = 0;
    virtual Gamepad::EState StateValue() const = 0;
    virtual unsigned AxisValue(Gamepad::EAxis axis) const = 0;
    virtual unsigned LastAxisValue(Gamepad::EAxis axis) const = 0;
    virtual std::uint32_t ButtonsMask() const = 0;
    virtual std::uint32_t LastButtonsMask() const = 0;
    virtual unsigned Pov() const = 0;
    virtual unsigned LastPov() const = 0;
    virtual Gamepad::EPovType PovType() const = 0;
    virtual int PressedCount() const = 0;
    virtual unsigned Threshold() const = 0;
    virtual bool SetThreshold(unsigned value) const = 0;
    virtual bool Capture(void* windowHandle, unsigned int period, Gamepad::EUpdateAction when) = 0;
    virtual bool Release() = 0;
};

IGamepadImpl::~IGamepadImpl() {} // implementation of pure virtual destructor to avoid memory leaks on derived classes

} // namespace FMlib
