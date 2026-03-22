#include "GamepadNull.h"

namespace GamepadLib
{

EGamepadState GamepadNull::Init()
{
    m_state = EGamepadState::no_driver;
    return m_state;
}

const wchar_t* GamepadNull::ProductName() const { return ProductNameValue.data(); }

std::uint16_t GamepadNull::VendorId() const { return 0; }

std::uint16_t GamepadNull::ProductId() const { return 0; }

unsigned GamepadNull::NumButtons() const { return 0; }

unsigned GamepadNull::NumAxes() const { return 0; }

UnsignedPair GamepadNull::PollingFrequency() const { return { 0u, 0u }; }

UnsignedPair GamepadNull::AxisRange(EAxis axis) const { return m_ranges.at(axis); }

bool GamepadNull::HasAxis(EAxis) const { return false; }

EGamepadState GamepadNull::ReadData() { return m_state; }

EGamepadState GamepadNull::StateValue() const { return m_state; }

unsigned GamepadNull::AxisValue(EAxis axis) const { return m_axisValues.at(axis); }

unsigned GamepadNull::LastAxisValue(EAxis axis) const { return m_axisValues.at(axis); }

std::optional<int> GamepadNull::RawAxisCode(EAxis) const { return std::nullopt; }

const std::unordered_set<std::uint16_t>& GamepadNull::CurrentButtons() const { return m_buttons; }

const std::unordered_set<std::uint16_t>& GamepadNull::PreviousButtons() const { return m_buttons; }

unsigned GamepadNull::Pov() const { return Constants::PovCenteredVal; }

unsigned GamepadNull::LastPov() const { return Constants::PovCenteredVal; }

EPovType GamepadNull::PovType() const { return EPovType::no_pov; }

int GamepadNull::PressedCount() const { return 0; }

unsigned GamepadNull::Threshold() const { return 0; }

bool GamepadNull::SetThreshold(unsigned) const { return false; }

bool GamepadNull::Capture(void*, unsigned int, EUpdateAction) { return false; }

bool GamepadNull::Release() { return false; }

} // namespace GamepadLib
