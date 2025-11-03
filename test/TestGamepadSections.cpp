#include "../base/GamepadSectionMapper.h"
#include "../gamepad/Gamepad.h"
#include "../gamepad/GamepadTypes.h"

#include <array>
#include <catch2/catch_test_macros.hpp>
#include <optional>
#include <unordered_set>

namespace
{

using EAxis = GamepadLib::EAxis;

class StubGamepadBackend : public GamepadLib::GamepadBackend
{
  public:
    StubGamepadBackend()
    {
        m_axes.fill(GamepadLib::Constants::MidPos);
        m_lastAxes = m_axes;
        m_axisSupported.fill(true);
    }

    void SetAxisSamples(GamepadLib::EAxis axis, unsigned lastValue, unsigned currentValue)
    {
        const std::size_t index = static_cast<std::size_t>(axis);
        m_lastAxes[index] = lastValue;
        m_axes[index] = currentValue;
    }

    // GamepadBackend interface
    GamepadLib::EGamepadState Init() override { return m_state; }
    const wchar_t* ProductName() const override { return L""; }
    std::uint16_t VendorId() const override { return 0; }
    std::uint16_t ProductId() const override { return 0; }
    unsigned NumButtons() const override { return 0; }
    unsigned NumAxes() const override { return static_cast<unsigned>(GamepadLib::EAxis::MaxValue); }
    GamepadLib::UnsignedPair PollingFrequency() const override { return { 0u, 0u }; }
    GamepadLib::UnsignedPair AxisRange(GamepadLib::EAxis) const override
    {
        return { 0u, GamepadLib::Constants::MaxPos };
    }
    bool HasAxis(GamepadLib::EAxis axis) const override
    {
        return m_axisSupported[static_cast<std::size_t>(axis)];
    }
    GamepadLib::EGamepadState ReadData() override { return m_state; }
    GamepadLib::EGamepadState StateValue() const override { return m_state; }
    unsigned AxisValue(GamepadLib::EAxis axis) const override
    {
        return m_axes[static_cast<std::size_t>(axis)];
    }
    unsigned LastAxisValue(GamepadLib::EAxis axis) const override
    {
        return m_lastAxes[static_cast<std::size_t>(axis)];
    }
    std::optional<int> RawAxisCode(GamepadLib::EAxis) const override { return std::nullopt; }
    const std::unordered_set<std::uint16_t>& CurrentButtons() const override { return m_buttons; }
    const std::unordered_set<std::uint16_t>& PreviousButtons() const override
    {
        return m_lastButtons;
    }
    unsigned Pov() const override { return GamepadLib::Constants::PovCenteredVal; }
    unsigned LastPov() const override { return GamepadLib::Constants::PovCenteredVal; }
    GamepadLib::EPovType PovType() const override { return GamepadLib::EPovType::discrete; }
    int PressedCount() const override { return 0; }
    unsigned Threshold() const override { return 0; }
    bool SetThreshold(unsigned) const override { return false; }
    bool Capture(void*, unsigned int, GamepadLib::EUpdateAction) override { return false; }
    bool Release() override { return true; }

  private:
    GamepadLib::EGamepadState m_state{ GamepadLib::EGamepadState::ok };
    std::array<unsigned, static_cast<std::size_t>(GamepadLib::EAxis::MaxValue)> m_axes{};
    std::array<unsigned, static_cast<std::size_t>(GamepadLib::EAxis::MaxValue)> m_lastAxes{};
    std::array<bool, static_cast<std::size_t>(GamepadLib::EAxis::MaxValue)> m_axisSupported{};
    std::unordered_set<std::uint16_t> m_buttons;
    std::unordered_set<std::uint16_t> m_lastButtons;
};

} // namespace

TEST_CASE("Gamepad:GetMappedSection returns expected sections")
{
    auto backend = std::make_unique<StubGamepadBackend>();
    GamepadLib::Gamepad gamepad(std::move(backend));
    auto midPos = GamepadLib::Constants::MidPos;
    auto maxPos = GamepadLib::Constants::MaxPos;

    /*    
        6 6 . 7 7 7 . 8 8
        6 6 . . 7 . . 8 8
        . . . . . . . . .
        4 . . . . . . . 5
        4 4 . . 0 . . 5 5
        4 . . . . . . . 5
        . . . . . . . . .
        1 1 . . 2 . . 3 3
        1 1 . 2 2 2 . 3 3
    */
    REQUIRE(gamepad.GetMappedSection(0, 0) == 1);           // bottom left
    REQUIRE(gamepad.GetMappedSection(midPos, 0) == 2);      // bottom center
    REQUIRE(gamepad.GetMappedSection(maxPos, 0) == 3);      // bottom right
    REQUIRE(gamepad.GetMappedSection(0, midPos) == 4);      // middle left
    REQUIRE(gamepad.GetMappedSection(midPos, midPos) == 0); // middle
    REQUIRE(gamepad.GetMappedSection(maxPos, midPos) == 5); // middle right
    REQUIRE(gamepad.GetMappedSection(0, maxPos) == 6);      // top left
    REQUIRE(gamepad.GetMappedSection(midPos, maxPos) == 7); // top
    REQUIRE(gamepad.GetMappedSection(maxPos, maxPos) == 8); // top right
}

TEST_CASE("Gamepad detects entry into XY sections")
{
    auto backend = std::make_unique<StubGamepadBackend>();
    auto* backendPtr = backend.get();
    GamepadLib::Gamepad gamepad(std::move(backend));

    const unsigned mid = GamepadLib::Constants::MidPos;
    backendPtr->SetAxisSamples(GamepadLib::EAxis::X, mid, mid);
    backendPtr->SetAxisSamples(GamepadLib::EAxis::Y, mid, GamepadLib::Constants::MaxPos);

    REQUIRE(gamepad.GetSection(EAxis::X, EAxis::Y) != 0);
}

TEST_CASE("Gamepad detects entry into RZ sections")
{
    auto backend = std::make_unique<StubGamepadBackend>();
    auto* backendPtr = backend.get();
    GamepadLib::Gamepad gamepad(std::move(backend));

    const unsigned mid = GamepadLib::Constants::MidPos;
    constexpr unsigned diagonal = 32124u;
    backendPtr->SetAxisSamples(GamepadLib::EAxis::R, mid, mid + diagonal);
    backendPtr->SetAxisSamples(GamepadLib::EAxis::Z, mid, mid + diagonal);

    REQUIRE(gamepad.GetSection(EAxis::R, EAxis::Z) != 0);
}

TEST_CASE("Gamepad reports no entry when sticks remain neutral")
{
    auto backend = std::make_unique<StubGamepadBackend>();
    auto* backendPtr = backend.get();
    GamepadLib::Gamepad gamepad(std::move(backend));

    const unsigned mid = GamepadLib::Constants::MidPos;
    backendPtr->SetAxisSamples(GamepadLib::EAxis::X, mid, mid);
    backendPtr->SetAxisSamples(GamepadLib::EAxis::Y, mid, mid);

    REQUIRE(gamepad.GetSection(EAxis::X, EAxis::Y) == 0);
}
