#pragma once

#include "Gamepad.h"

#include <array>
#include <cstdint>
#include <memory>
#include <utility>

class QString;

namespace GamepadLib
{
class JessTechPadAdapter
{
  public:
    virtual ~JessTechPadAdapter() = default;

    virtual bool Available() const = 0;
    virtual void Poll() = 0;

    virtual const std::array<unsigned, EAxis::MaxValue>& Axes() const = 0;
    virtual const std::array<bool, EAxis::MaxValue>& AxisSupported() const = 0;
    virtual std::uint32_t Buttons() const = 0;
    virtual unsigned Pov() const = 0;
    virtual unsigned ButtonCount() const = 0;
};

std::unique_ptr<JessTechPadAdapter> CreateJessTechPadAdapter(const QString& deviceName);

} // namespace GamepadLib
