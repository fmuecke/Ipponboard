
#ifndef _WIN32 // Linux / Qt implementation

#include "GamepadLinuxImpl.h"

#include "JessTechPadAdapter.h"

#include <QDebug>
#include <QList>
#include <QObject>
#include <QProcessEnvironment>
#include <QStringList>

namespace FMlib
{

constexpr std::size_t axisIndex(Gamepad::EAxis axis) { return static_cast<std::size_t>(axis); }

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

GamepadImpl::GamepadImpl()
{
    ranges.fill({ 0u, Gamepad::eMax });
    axisSupported.fill(true);
    ensureDebugConnections();
}

Gamepad::EState GamepadImpl::Init()
{
    const auto ids = QGamepadManager::instance()->connectedGamepads();
    if (ids.isEmpty())
    {
        resetState();
        state = Gamepad::eState_unplugged;
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
    state = Gamepad::eState_ok;
    ReadData();
    return state;
}

const wchar_t* GamepadImpl::ProductName() const
{
    return productName.empty() ? L"" : productName.c_str();
}

std::uint16_t GamepadImpl::VendorId() const { return vendorId; }
std::uint16_t GamepadImpl::ProductId() const { return productId; }
unsigned GamepadImpl::NumButtons() const { return buttonCount; }
unsigned GamepadImpl::NumAxes() const { return axisCount; }
Gamepad::UnsignedPair GamepadImpl::PollingFrequency() const { return { 0u, 0u }; }

Gamepad::UnsignedPair GamepadImpl::AxisRange(Gamepad::EAxis axis) const
{
    return ranges[axisIndex(axis)];
}

bool GamepadImpl::HasAxis(Gamepad::EAxis axis) const { return axisSupported[axisIndex(axis)]; }

Gamepad::EPovType GamepadImpl::PovType() const
{
    return dpadPresent ? Gamepad::ePovType_discrete : Gamepad::ePovType_no_pov;
}

Gamepad::EState GamepadImpl::ReadData()
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
        axisCount = Gamepad::eAxis_MAX;
        dpadPresent = true;
        ranges.fill({ 0u, Gamepad::eMax });
        state = Gamepad::eState_ok;
        return state;
    }

    if (!gamepad)
    {
        axes.fill(Gamepad::eMid);
        buttons = 0;
        povValue = Gamepad::kPovCentered;
        return state;
    }

    debugLogRawState();

    axes = sampleAxes();
    buttons = sampleButtons();
    povValue = computePov();
    state = Gamepad::eState_ok;
    return state;
}

Gamepad::EState GamepadImpl::StateValue() const { return state; }

unsigned GamepadImpl::AxisValue(Gamepad::EAxis axis) const { return axes[axisIndex(axis)]; }

unsigned GamepadImpl::LastAxisValue(Gamepad::EAxis axis) const { return lastAxes[axisIndex(axis)]; }

std::uint32_t GamepadImpl::ButtonsMask() const { return buttons; }
std::uint32_t GamepadImpl::LastButtonsMask() const { return lastButtons; }

unsigned GamepadImpl::Pov() const { return povValue; }
unsigned GamepadImpl::LastPov() const { return lastPovValue; }
int GamepadImpl::PressedCount() const { return static_cast<int>(popcount(buttons)); }

unsigned GamepadImpl::Threshold() const { return 0; }
bool GamepadImpl::SetThreshold(unsigned) const { return false; }

bool GamepadImpl::Capture(void*, unsigned int, Gamepad::EUpdateAction) { return false; }
bool GamepadImpl::Release() { return false; }

bool GamepadImpl::isDebugLoggingEnabled()
{
    static const bool enabled = qEnvironmentVariableIsSet("IPPONBOARD_DEBUG_GAMEPAD");
    return enabled;
}

const char* GamepadImpl::buttonName(QGamepadManager::GamepadButton button)
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

const char* GamepadImpl::axisName(QGamepadManager::GamepadAxis axis)
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

void GamepadImpl::ensureDebugConnections()
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

bool GamepadImpl::hasSignificantDelta(double lhs, double rhs)
{
    constexpr double kEpsilon = 0.01;
    return std::fabs(lhs - rhs) > kEpsilon;
}

void GamepadImpl::debugLogRawState() const
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

void GamepadImpl::resetState()
{
    gamepad.reset();
    jessTech.reset();
    productName.clear();
    vendorId = 0;
    productId = 0;
    buttonCount = 0;
    axisCount = 0;
    axes.fill(Gamepad::eMid);
    lastAxes = axes;
    buttons = 0;
    lastButtons = 0;
    povValue = Gamepad::kPovCentered;
    lastPovValue = Gamepad::kPovCentered;
    dpadPresent = false;
}

void GamepadImpl::refreshMetadata()
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
    axisCount = Gamepad::eAxis_MAX;
    dpadPresent = true;
    jessTech.reset();
    ranges.fill({ 0u, Gamepad::eMax });
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

void GamepadImpl::updateConnection()
{
    auto& mgr = *QGamepadManager::instance();
    const auto ids = mgr.connectedGamepads();

    if (deviceId != -1 && !ids.contains(deviceId))
    {
        resetState();
        state = Gamepad::eState_unplugged;
    }

    if (!gamepad && !ids.isEmpty())
    {
        deviceId = ids.first();
        gamepad = std::make_unique<QGamepad>(deviceId);
        refreshMetadata();
    }
}

unsigned GamepadImpl::toUnsignedAxis(double value)
{
    value = std::clamp(value, -1.0, 1.0);
    return static_cast<unsigned>(((value + 1.0) * 0.5) * Gamepad::eMax + 0.5);
}

unsigned GamepadImpl::toUnsignedTrigger(double value)
{
    value = std::clamp(value, 0.0, 1.0);
    return static_cast<unsigned>(value * Gamepad::eMax + 0.5);
}

std::array<unsigned, Gamepad::eAxis_MAX> GamepadImpl::sampleAxes() const
{
    std::array<unsigned, Gamepad::eAxis_MAX> result{};
    result[Gamepad::eAxis_X] = toUnsignedAxis(gamepad->axisLeftX());
    result[Gamepad::eAxis_Y] = toUnsignedAxis(gamepad->axisLeftY());
    result[Gamepad::eAxis_Z] = toUnsignedAxis(gamepad->axisRightX());
    result[Gamepad::eAxis_R] = toUnsignedAxis(gamepad->axisRightY());
    result[Gamepad::eAxis_U] = toUnsignedTrigger(gamepad->buttonL2());
    result[Gamepad::eAxis_V] = toUnsignedTrigger(gamepad->buttonR2());
    return result;
}

std::uint32_t GamepadImpl::sampleButtons() const
{
    std::uint32_t mask = 0;
    setButton(mask, Gamepad::eButton1, gamepad->buttonA());
    setButton(mask, Gamepad::eButton2, gamepad->buttonB());
    setButton(mask, Gamepad::eButton3, gamepad->buttonX());
    setButton(mask, Gamepad::eButton4, gamepad->buttonY());
    setButton(mask, Gamepad::eButton5, gamepad->buttonL1());
    setButton(mask, Gamepad::eButton6, gamepad->buttonR1());
    setButton(mask, Gamepad::eButton7, gamepad->buttonL2() > 0.5); // analogue treated as button
    setButton(mask, Gamepad::eButton8, gamepad->buttonR2() > 0.5);
    setButton(mask, Gamepad::eButton9, gamepad->buttonSelect());
    setButton(mask, Gamepad::eButton10, gamepad->buttonStart());
    setButton(mask, Gamepad::eButton11, gamepad->buttonL3());
    setButton(mask, Gamepad::eButton12, gamepad->buttonR3());
    setButton(mask, Gamepad::eButton17, gamepad->buttonCenter());
    setButton(mask, Gamepad::eButton18, gamepad->buttonGuide());
    setButton(mask, Gamepad::eButton13, gamepad->buttonUp());
    setButton(mask, Gamepad::eButton14, gamepad->buttonRight());
    setButton(mask, Gamepad::eButton15, gamepad->buttonDown());
    setButton(mask, Gamepad::eButton16, gamepad->buttonLeft());
    return mask;
}

void GamepadImpl::setButton(std::uint32_t& mask, Gamepad::EButton button, bool pressed)
{
    if (pressed)
    {
        mask |= Gamepad::ButtonCode(button);
    }
}

unsigned GamepadImpl::computePov() const
{
    const bool up = gamepad->buttonUp();
    const bool down = gamepad->buttonDown();
    const bool left = gamepad->buttonLeft();
    const bool right = gamepad->buttonRight();

    if ((up && down) || (left && right))
    {
        return Gamepad::kPovCentered;
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

    return Gamepad::kPovCentered;
}

} // namespace FMlib

#endif