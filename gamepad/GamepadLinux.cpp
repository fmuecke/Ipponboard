#ifndef _WIN32

#include "GamepadLinux.h"

#include <fcntl.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <algorithm>
#include <array>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <optional>

namespace GamepadLib
{
namespace
{
constexpr double kDeadzone = 0.10; // 10% deadzone around the centre

const std::array<std::uint16_t, 8> kPovButtonCodes = {
    static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_fwd)),
    static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_right)),
    static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_back)),
    static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_left)),
    static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_right_fwd)),
    static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_right_back)),
    static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_left_back)),
    static_cast<std::uint16_t>(Gamepad::ButtonCode(EButton::button_pov_left_fwd)) };

std::optional<std::string> findJoystickPath()
{
    namespace fs = std::filesystem;

    const fs::path byId{ "/dev/input/by-id" };
    if (fs::exists(byId))
    {
        for (const auto& entry : fs::directory_iterator(byId))
        {
            const auto& path = entry.path();
            if (!path.has_filename())
            {
                continue;
            }
            const auto name = path.filename().string();
            if (name.find("event-joystick") != std::string::npos)
            {
                return path.string();
            }
        }
    }

    const fs::path devInput{ "/dev/input" };
    if (fs::exists(devInput))
    {
        for (const auto& entry : fs::directory_iterator(devInput))
        {
            const auto& path = entry.path();
            if (!path.has_filename())
            {
                continue;
            }
            const auto name = path.filename().string();
            if (name.rfind("event", 0) == 0)
            {
                return path.string();
            }
        }
    }

    return std::nullopt;
}

std::optional<EAxis> axisFromCode(int code)
{
    switch (code)
    {
    case ABS_X:
        return EAxis::X;
    case ABS_Y:
        return EAxis::Y;
    case ABS_Z:
        return EAxis::Z;
    case ABS_RZ:
        return EAxis::R;
    case ABS_RX:
        return EAxis::U;
    case ABS_RY:
        return EAxis::V;
    default:
        break;
    }
    return std::nullopt;
}

unsigned scaleAxis(int value, int min, int max)
{
    if (max <= min)
    {
        return Constants::MidAngle;
    }

    value = std::clamp(value, min, max);
    const double normalized = static_cast<double>(value - min) / static_cast<double>(max - min);
    double centered = normalized * 2.0 - 1.0; // [-1, 1]

    const double absCentered = std::fabs(centered);
    if (absCentered <= kDeadzone)
    {
        centered = 0.0;
    }
    else
    {
        const double adjusted = (absCentered - kDeadzone) / (1.0 - kDeadzone);
        centered = std::copysign(std::clamp(adjusted, 0.0, 1.0), centered);
    }

    const double scaled = (centered + 1.0) * 0.5; // back to [0,1]
    return static_cast<unsigned>(std::clamp(scaled, 0.0, 1.0) * Constants::MaxAngle + 0.5);
}

unsigned povFromHat(int hatX, int hatY)
{
    if (hatX == 0 && hatY == 0)
    {
        return Constants::PovCenteredVal;
    }

    if (hatY == -1)
    {
        if (hatX == 0)
            return 0;
        if (hatX == 1)
            return 4500;
        return 31500;
    }

    if (hatY == 1)
    {
        if (hatX == 0)
            return 18000;
        if (hatX == 1)
            return 13500;
        return 22500;
    }

    if (hatX == 1)
        return 9000;
    if (hatX == -1)
        return 27000;

    return Constants::PovCenteredVal;
}

} // namespace

GamepadLinux::GamepadLinux()
{
    reset();
}

GamepadLinux::~GamepadLinux()
{
    closeDevice();
}

void GamepadLinux::reset()
{
    closeDevice();
    m_devicePath.clear();
    m_productName = L"";
    m_vendorId = 0;
    m_productId = 0;
    m_buttonCount = 0;
    m_axisCount = 0;
    m_state = EGamepadState::unknown;
    m_axes.fill(Constants::MidAngle);
    m_lastAxes = m_axes;
    m_axisSupported.fill(false);
    m_axisMin.fill(0);
    m_axisMax.fill(0);
    m_ranges.fill({ 0u, Constants::MaxAngle });
    m_buttons.clear();
    m_lastButtons.clear();
    m_knownButtons.clear();
    m_hatX = 0;
    m_hatY = 0;
    m_pov = Constants::PovCenteredVal;
    m_lastPov = Constants::PovCenteredVal;
}

void GamepadLinux::closeDevice()
{
    if (m_fd >= 0)
    {
        ::close(m_fd);
        m_fd = -1;
    }
}

bool GamepadLinux::openDevice()
{
    const auto path = findJoystickPath();
    if (!path)
    {
        return false;
    }

    const int fd = ::open(path->c_str(), O_RDONLY | O_NONBLOCK);
    if (fd < 0)
    {
        return false;
    }

    m_fd = fd;
    m_devicePath = *path;
    return true;
}

void GamepadLinux::queryDeviceInfo()
{
    if (m_fd < 0)
    {
        return;
    }

    char nameBuf[128]{};
    if (::ioctl(m_fd, EVIOCGNAME(sizeof(nameBuf)), nameBuf) >= 0)
    {
        std::string name(nameBuf);
        m_productName.assign(name.begin(), name.end());
    }
    else
    {
        m_productName = L"Linux Gamepad";
    }

    input_id id{};
    if (::ioctl(m_fd, EVIOCGID, &id) >= 0)
    {
        m_vendorId = id.vendor;
        m_productId = id.product;
    }
    else
    {
        m_vendorId = 0;
        m_productId = 0;
    }
}

void GamepadLinux::queryAxisInfo()
{
    if (m_fd < 0)
    {
        return;
    }

    std::array<int, EAxis::MaxValue> counts{};
    for (int code : { ABS_X, ABS_Y, ABS_Z, ABS_RZ, ABS_RX, ABS_RY })
    {
        auto axis = axisFromCode(code);
        if (!axis)
        {
            continue;
        }

        input_absinfo info{};
        if (::ioctl(m_fd, EVIOCGABS(code), &info) == 0)
        {
            const auto idx = static_cast<std::size_t>(*axis);
            m_axisMin[idx] = info.minimum;
            m_axisMax[idx] = info.maximum;
            m_ranges[idx] = { 0u, Constants::MaxAngle };
            m_axisSupported[idx] = true;
            ++counts[idx];
        }
    }

    m_axisCount = 0;
    for (bool supported : m_axisSupported)
    {
        if (supported)
        {
            ++m_axisCount;
        }
    }
}

EGamepadState GamepadLinux::Init()
{
    reset();

    if (!openDevice())
    {
        m_state = EGamepadState::unplugged;
        return m_state;
    }

    queryDeviceInfo();
    queryAxisInfo();
    m_state = EGamepadState::ok;
    ReadData();
    return m_state;
}

const wchar_t* GamepadLinux::ProductName() const
{
    return m_productName.c_str();
}

std::uint16_t GamepadLinux::VendorId() const { return m_vendorId; }
std::uint16_t GamepadLinux::ProductId() const { return m_productId; }
unsigned GamepadLinux::NumButtons() const { return m_buttonCount; }
unsigned GamepadLinux::NumAxes() const { return m_axisCount; }
UnsignedPair GamepadLinux::PollingFrequency() const { return { 0u, 0u }; }

UnsignedPair GamepadLinux::AxisRange(EAxis axis) const
{
    return m_ranges[static_cast<std::size_t>(axis)];
}

bool GamepadLinux::HasAxis(EAxis axis) const
{
    return m_axisSupported[static_cast<std::size_t>(axis)];
}

EGamepadState GamepadLinux::ReadData()
{
    if (m_fd < 0)
    {
        if (openDevice())
        {
            queryDeviceInfo();
            queryAxisInfo();
        }
        else
        {
            m_state = EGamepadState::unplugged;
            return m_state;
        }
    }

    m_lastButtons = m_buttons;
    m_lastAxes = m_axes;
    m_lastPov = m_pov;

    input_event events[32];
    bool anyEvent = false;

    while (true)
    {
        const auto bytes = ::read(m_fd, events, sizeof(events));
        if (bytes < 0)
        {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
            {
                break;
            }
            if (errno == ENODEV)
            {
                closeDevice();
                m_state = EGamepadState::unplugged;
                m_buttons.clear();
                m_axes.fill(Constants::MidAngle);
                m_pov = Constants::PovCenteredVal;
                return m_state;
            }
            break;
        }
        if (bytes == 0)
        {
            break;
        }

        anyEvent = true;
        const auto count = static_cast<std::size_t>(bytes) / sizeof(input_event);
        for (std::size_t i = 0; i < count; ++i)
        {
            handleEvent(events[i]);
        }
    }

    if (anyEvent)
    {
        m_state = EGamepadState::ok;
    }

    for (auto code : kPovButtonCodes)
    {
        m_buttons.erase(code);
    }

    if (m_pov != Constants::PovCenteredVal)
    {
        m_buttons.insert(static_cast<std::uint16_t>(m_pov));
    }

    return m_state;
}

EGamepadState GamepadLinux::StateValue() const { return m_state; }

unsigned GamepadLinux::AxisValue(EAxis axis) const
{
    return m_axes[static_cast<std::size_t>(axis)];
}

unsigned GamepadLinux::LastAxisValue(EAxis axis) const
{
    return m_lastAxes[static_cast<std::size_t>(axis)];
}

const std::unordered_set<std::uint16_t>& GamepadLinux::CurrentButtons() const
{
    return m_buttons;
}

const std::unordered_set<std::uint16_t>& GamepadLinux::PreviousButtons() const
{
    return m_lastButtons;
}

unsigned GamepadLinux::Pov() const { return m_pov; }

unsigned GamepadLinux::LastPov() const { return m_lastPov; }

EPovType GamepadLinux::PovType() const
{
    return EPovType::discrete;
}

int GamepadLinux::PressedCount() const
{
    return static_cast<int>(m_buttons.size());
}

unsigned GamepadLinux::Threshold() const { return 0; }

bool GamepadLinux::SetThreshold(unsigned) const { return false; }

bool GamepadLinux::Capture(void*, unsigned int, EUpdateAction) { return false; }

bool GamepadLinux::Release() { return false; }

void GamepadLinux::handleEvent(const input_event& ev)
{
    switch (ev.type)
    {
    case EV_KEY:
        handleKeyEvent(ev);
        break;
    case EV_ABS:
        handleAbsEvent(ev);
        break;
    default:
        break;
    }
}

void GamepadLinux::handleKeyEvent(const input_event& ev)
{
    const auto code = static_cast<std::uint16_t>(ev.code);
    if (ev.value)
    {
        m_buttons.insert(code);
        if (m_knownButtons.insert(code).second)
        {
            ++m_buttonCount;
        }
    }
    else
    {
        m_buttons.erase(code);
    }
}

void GamepadLinux::handleAbsEvent(const input_event& ev)
{
    if (ev.code == ABS_HAT0X)
    {
        m_hatX = std::clamp(ev.value, -1, 1);
        updatePov();
        return;
    }
    if (ev.code == ABS_HAT0Y)
    {
        m_hatY = std::clamp(ev.value, -1, 1);
        updatePov();
        return;
    }

    if (auto axis = axisFromCode(ev.code))
    {
        const auto idx = static_cast<std::size_t>(*axis);
        if (m_axisSupported[idx])
        {
            m_axes[idx] = scaleAxis(ev.value, m_axisMin[idx], m_axisMax[idx]);
        }
    }
}

void GamepadLinux::updatePov()
{
    m_pov = povFromHat(m_hatX, m_hatY);
}

} // namespace GamepadLib

#endif
