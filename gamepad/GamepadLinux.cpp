
#ifndef _WIN32 // Linux / Qt implementation

#include "GamepadLinux.h"

#include "JessTechPadAdapter.h"

#include <QDebug>
#include <QList>
#include <QObject>
#include <QProcessEnvironment>
#include <QStringList>

namespace GamepadLib
{

constexpr std::size_t axisIndex(EAxis axis) { return static_cast<std::size_t>(axis); }

unsigned popcount(std::uint32_t value)
{
    unsigned count = 0;
    while (value != 0)
    {
        value &= (value - 1);
        ++count;
    }
    return count;
}

GamepadLinux::GamepadLinux()
{
    ranges.fill({ 0u, Constants::MaxAngle });
    axisSupported.fill(true);
    ensureDebugConnections();
}

EGamepadState GamepadLinux::Init()
{
    const auto ids = QGamepadManager::instance()->connectedGamepads();
    if (ids.isEmpty())
    {
        resetState();
        state = EGamepadState::unplugged;
        return state;
    }

    if (isDebugLoggingEnabled())
    {
        QStringList idStrings;
        for (int id : ids)
        {
            idStrings.push_back(QString::number(id));
        }
        qInfo().noquote() << "[QGamepad info] discovered devices=" << idStrings.join(',');
    }

    deviceId = ids.first();
    if (isDebugLoggingEnabled())
    {
        qInfo().noquote() << "[QGamepad info] using device=" << deviceId;
    }
    gamepad = std::make_unique<QGamepad>(deviceId);
    refreshMetadata();
    state = EGamepadState::ok;
    ReadData();
    return state;
}

const wchar_t* GamepadLinux::ProductName() const
{
    return productName.empty() ? L"" : productName.c_str();
}

std::uint16_t GamepadLinux::VendorId() const { return vendorId; }
std::uint16_t GamepadLinux::ProductId() const { return productId; }
unsigned GamepadLinux::NumButtons() const { return buttonCount; }
unsigned GamepadLinux::NumAxes() const { return axisCount; }
UnsignedPair GamepadLinux::PollingFrequency() const { return { 0u, 0u }; }

UnsignedPair GamepadLinux::AxisRange(EAxis axis) const { return ranges[axisIndex(axis)]; }

bool GamepadLinux::HasAxis(EAxis axis) const { return axisSupported[axisIndex(axis)]; }

EPovType GamepadLinux::PovType() const
{
    return dpadPresent ? EPovType::discrete : EPovType::no_pov;
}

EGamepadState GamepadLinux::ReadData()
{
    updateConnection();

    lastAxes = axes;
    lastButtons = buttons;
    lastPovValue = povValue;

    if (jessTech && jessTech->Available())
    {
        jessTech->Poll();
        axes = jessTech->Axes();
        axisSupported = jessTech->AxisSupported();
        buttons = jessTech->Buttons();
        povValue = jessTech->Pov();
        buttonCount = jessTech->ButtonCount();
        axisCount = EAxis::MaxValue;
        dpadPresent = true;
        ranges.fill({ 0u, Constants::MaxAngle });
        state = EGamepadState::ok;
        return state;
    }

    if (!gamepad)
    {
        axes.fill(Constants::MidAngle);
        buttons = 0;
        povValue = Constants::PovCenteredVal;
        return state;
    }

    debugLogRawState();

    axes = sampleAxes();
    buttons = sampleButtons();
    povValue = computePov();
    state = EGamepadState::ok;
    return state;
}

EGamepadState GamepadLinux::StateValue() const { return state; }

unsigned GamepadLinux::AxisValue(EAxis axis) const { return axes[axisIndex(axis)]; }

unsigned GamepadLinux::LastAxisValue(EAxis axis) const { return lastAxes[axisIndex(axis)]; }

std::uint32_t GamepadLinux::ButtonsMask() const { return buttons; }
std::uint32_t GamepadLinux::LastButtonsMask() const { return lastButtons; }

unsigned GamepadLinux::Pov() const { return povValue; }
unsigned GamepadLinux::LastPov() const { return lastPovValue; }
int GamepadLinux::PressedCount() const { return static_cast<int>(popcount(buttons)); }

unsigned GamepadLinux::Threshold() const { return 0; }
bool GamepadLinux::SetThreshold(unsigned) const { return false; }

bool GamepadLinux::Capture(void*, unsigned int, EUpdateAction) { return false; }
bool GamepadLinux::Release() { return false; }

bool GamepadLinux::isDebugLoggingEnabled()
{
    static const bool enabled = qEnvironmentVariableIsSet("IPPONBOARD_DEBUG_GAMEPAD");
    return enabled;
}

const char* GamepadLinux::buttonName(QGamepadManager::GamepadButton button)
{
    using Button = QGamepadManager::GamepadButton;
    switch (button)
    {
    case Button::ButtonA:
        return "ButtonA";
    case Button::ButtonB:
        return "ButtonB";
    case Button::ButtonX:
        return "ButtonX";
    case Button::ButtonY:
        return "ButtonY";
    case Button::ButtonL1:
        return "ButtonL1";
    case Button::ButtonR1:
        return "ButtonR1";
    case Button::ButtonL2:
        return "ButtonL2";
    case Button::ButtonR2:
        return "ButtonR2";
    case Button::ButtonSelect:
        return "ButtonSelect";
    case Button::ButtonStart:
        return "ButtonStart";
    case Button::ButtonL3:
        return "ButtonL3";
    case Button::ButtonR3:
        return "ButtonR3";
    case Button::ButtonUp:
        return "ButtonUp";
    case Button::ButtonDown:
        return "ButtonDown";
    case Button::ButtonRight:
        return "ButtonRight";
    case Button::ButtonLeft:
        return "ButtonLeft";
    case Button::ButtonCenter:
        return "ButtonCenter";
    case Button::ButtonGuide:
        return "ButtonGuide";
    case Button::ButtonInvalid:
    default:
        return "ButtonInvalid";
    }
}

const char* GamepadLinux::axisName(QGamepadManager::GamepadAxis axis)
{
    using Axis = QGamepadManager::GamepadAxis;
    switch (axis)
    {
    case Axis::AxisLeftX:
        return "AxisLeftX";
    case Axis::AxisLeftY:
        return "AxisLeftY";
    case Axis::AxisRightX:
        return "AxisRightX";
    case Axis::AxisRightY:
        return "AxisRightY";
    case Axis::AxisInvalid:
    default:
        return "AxisInvalid";
    }
}

void GamepadLinux::ensureDebugConnections()
{
    if (!isDebugLoggingEnabled())
    {
        return;
    }

    static bool connected = false;
    if (connected)
    {
        return;
    }

    if (auto* mgr = QGamepadManager::instance())
    {
        QObject::connect(mgr,
                         &QGamepadManager::gamepadButtonPressEvent,
                         mgr,
                         [](int device, QGamepadManager::GamepadButton button, double value)
                         {
                             qInfo().nospace() << "[QGamepad event] device=" << device << " press "
                                               << buttonName(button) << " value=" << value;
                         });
        QObject::connect(mgr,
                         &QGamepadManager::gamepadButtonReleaseEvent,
                         mgr,
                         [](int device, QGamepadManager::GamepadButton button)
                         {
                             qInfo().nospace() << "[QGamepad event] device=" << device
                                               << " release " << buttonName(button);
                         });
        QObject::connect(mgr,
                         &QGamepadManager::gamepadAxisEvent,
                         mgr,
                         [](int device, QGamepadManager::GamepadAxis axis, double value)
                         {
                             qInfo().nospace() << "[QGamepad event] device=" << device << " axis "
                                               << axisName(axis) << " value=" << value;
                         });
        connected = true;
    }
}

bool GamepadLinux::hasSignificantDelta(double lhs, double rhs)
{
    constexpr double kEpsilon = 0.01;
    return std::fabs(lhs - rhs) > kEpsilon;
}

void GamepadLinux::debugLogRawState() const
{
    if (!isDebugLoggingEnabled() || !gamepad)
    {
        return;
    }

    const auto toDouble = [](auto value) { return static_cast<double>(value); };

    const std::array<double, 22> rawReadings = {
        toDouble(gamepad->axisLeftX()),    toDouble(gamepad->axisLeftY()),
        toDouble(gamepad->axisRightX()),   toDouble(gamepad->axisRightY()),
        toDouble(gamepad->buttonA()),      toDouble(gamepad->buttonB()),
        toDouble(gamepad->buttonX()),      toDouble(gamepad->buttonY()),
        toDouble(gamepad->buttonL1()),     toDouble(gamepad->buttonR1()),
        toDouble(gamepad->buttonL2()),     toDouble(gamepad->buttonR2()),
        toDouble(gamepad->buttonSelect()), toDouble(gamepad->buttonStart()),
        toDouble(gamepad->buttonL3()),     toDouble(gamepad->buttonR3()),
        toDouble(gamepad->buttonUp()),     toDouble(gamepad->buttonRight()),
        toDouble(gamepad->buttonDown()),   toDouble(gamepad->buttonLeft()),
        toDouble(gamepad->buttonCenter()), toDouble(gamepad->buttonGuide())
    };

    static std::array<double, rawReadings.size()> s_lastReadings{};
    static bool s_initialised = false;

    if (!s_initialised)
    {
        s_lastReadings = rawReadings;
        s_initialised = true;
    }

    QStringList changes;
    const std::array<const char*, rawReadings.size()> names = {
        "axisLeftX",    "axisLeftY",   "axisRightX",   "axisRightY", "buttonA",  "buttonB",
        "buttonX",      "buttonY",     "buttonL1",     "buttonR1",   "buttonL2", "buttonR2",
        "buttonSelect", "buttonStart", "buttonL3",     "buttonR3",   "dpadUp",   "dpadRight",
        "dpadDown",     "dpadLeft",    "buttonCenter", "buttonGuide"
    };

    for (std::size_t i = 0; i < rawReadings.size(); ++i)
    {
        if (!s_initialised || hasSignificantDelta(rawReadings[i], s_lastReadings[i]))
        {
            changes.push_back(QStringLiteral("%1=%2")
                                  .arg(QLatin1String(names[i]))
                                  .arg(rawReadings[i], 0, 'f', 3));
            s_lastReadings[i] = rawReadings[i];
        }
    }

    if (!changes.isEmpty())
    {
        qInfo().noquote() << "[QGamepad raw]" << changes.join(QLatin1String(", "));
    }
}

void GamepadLinux::resetState()
{
    gamepad.reset();
    jessTech.reset();
    productName.clear();
    vendorId = 0;
    productId = 0;
    buttonCount = 0;
    axisCount = 0;
    axes.fill(Constants::MidAngle);
    lastAxes = axes;
    buttons = 0;
    lastButtons = 0;
    povValue = Constants::PovCenteredVal;
    lastPovValue = Constants::PovCenteredVal;
    dpadPresent = false;
}

void GamepadLinux::refreshMetadata()
{
    if (!gamepad)
    {
        return;
    }

    auto& mgr = *QGamepadManager::instance();
    productName = mgr.gamepadName(deviceId).toStdWString();
    if (isDebugLoggingEnabled())
    {
        qInfo().noquote() << "[QGamepad info] device=" << deviceId
                          << " name=" << QString::fromStdWString(productName);
    }
    vendorId = 0;
    productId = 0;
    buttonCount = 16; // default assumption
    axisCount = EAxis::MaxValue;
    dpadPresent = true;
    jessTech.reset();
    ranges.fill({ 0u, Constants::MaxAngle });
    axisSupported.fill(true);

    const QString deviceName = QString::fromStdWString(productName);
    jessTech = CreateJessTechPadAdapter(deviceName);
    if (jessTech && jessTech->Available())
    {
        buttonCount = jessTech->ButtonCount();
        axisSupported = jessTech->AxisSupported();
    }
    else
    {
        jessTech.reset();
    }
}

void GamepadLinux::updateConnection()
{
    auto& mgr = *QGamepadManager::instance();
    const auto ids = mgr.connectedGamepads();

    if (deviceId != -1 && !ids.contains(deviceId))
    {
        resetState();
        state = EGamepadState::unplugged;
    }

    if (!gamepad && !ids.isEmpty())
    {
        deviceId = ids.first();
        gamepad = std::make_unique<QGamepad>(deviceId);
        refreshMetadata();
    }
}

unsigned GamepadLinux::toUnsignedAxis(double value)
{
    value = std::clamp(value, -1.0, 1.0);
    return static_cast<unsigned>(((value + 1.0) * 0.5) * Constants::MaxAngle + 0.5);
}

unsigned GamepadLinux::toUnsignedTrigger(double value)
{
    value = std::clamp(value, 0.0, 1.0);
    return static_cast<unsigned>(value * Constants::MaxAngle + 0.5);
}

std::array<unsigned, EAxis::MaxValue> GamepadLinux::sampleAxes() const
{
    std::array<unsigned, EAxis::MaxValue> result{};
    result[(unsigned)EAxis::X] = toUnsignedAxis(gamepad->axisLeftX());
    result[(unsigned)EAxis::Y] = toUnsignedAxis(gamepad->axisLeftY());
    result[(unsigned)EAxis::Z] = toUnsignedAxis(gamepad->axisRightX());
    result[(unsigned)EAxis::R] = toUnsignedAxis(gamepad->axisRightY());
    result[(unsigned)EAxis::U] = toUnsignedTrigger(gamepad->buttonL2());
    result[(unsigned)EAxis::V] = toUnsignedTrigger(gamepad->buttonR2());
    return result;
}

std::uint32_t GamepadLinux::sampleButtons() const
{
    std::uint32_t mask = 0;
    setButton(mask, EButton::button1, gamepad->buttonA());
    setButton(mask, EButton::button2, gamepad->buttonB());
    setButton(mask, EButton::button3, gamepad->buttonX());
    setButton(mask, EButton::button4, gamepad->buttonY());
    setButton(mask, EButton::button5, gamepad->buttonL1());
    setButton(mask, EButton::button6, gamepad->buttonR1());
    setButton(mask, EButton::button7, gamepad->buttonL2() > 0.5); // analogue treated as button
    setButton(mask, EButton::button8, gamepad->buttonR2() > 0.5);
    setButton(mask, EButton::button9, gamepad->buttonSelect());
    setButton(mask, EButton::button10, gamepad->buttonStart());
    setButton(mask, EButton::button11, gamepad->buttonL3());
    setButton(mask, EButton::button12, gamepad->buttonR3());
    setButton(mask, EButton::button17, gamepad->buttonCenter());
    setButton(mask, EButton::button18, gamepad->buttonGuide());
    setButton(mask, EButton::button13, gamepad->buttonUp());
    setButton(mask, EButton::button14, gamepad->buttonRight());
    setButton(mask, EButton::button15, gamepad->buttonDown());
    setButton(mask, EButton::button16, gamepad->buttonLeft());
    return mask;
}

void GamepadLinux::setButton(std::uint32_t& mask, EButton button, bool pressed)
{
    if (pressed)
    {
        mask |= Gamepad::ButtonCode(button);
    }
}

unsigned GamepadLinux::computePov() const
{
    const bool up = gamepad->buttonUp();
    const bool down = gamepad->buttonDown();
    const bool left = gamepad->buttonLeft();
    const bool right = gamepad->buttonRight();

    if ((up && down) || (left && right))
    {
        return Constants::PovCenteredVal;
    }

    if (up)
    {
        if (right)
            return 4500;
        if (left)
            return 31500;
        return 0;
    }

    if (down)
    {
        if (right)
            return 13500;
        if (left)
            return 22500;
        return 18000;
    }

    if (right)
        return 9000;
    if (left)
        return 27000;

    return Constants::PovCenteredVal;
}

} // namespace GamepadLib

#endif