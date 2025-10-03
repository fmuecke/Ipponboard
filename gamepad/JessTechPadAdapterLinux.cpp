#include "GamepadTypes.h"
#include "JessTechPadAdapter.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QtGlobal>
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstring>
#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

namespace GamepadLib
{

constexpr std::uint32_t buttonMask(EButton button) { return 1u << static_cast<unsigned>(button); }

constexpr std::size_t axisIndex(EAxis axis) { return static_cast<std::size_t>(axis); }

constexpr double kDeadzone = 0.10; // 10% deadzone around the centre

class JessTechPadLinux final : public JessTechPadAdapter
{
  public:
    JessTechPadLinux()
    {
        openDevice();
        reset();
    }

    ~JessTechPadLinux() override
    {
        if (m_fd >= 0)
        {
            ::close(m_fd);
        }
    }

    bool Available() const override { return m_fd >= 0; }

    void Poll() override
    {
        if (m_fd < 0)
        {
            return;
        }

        input_event events[32];
        for (;;)
        {
            const auto bytes = ::read(m_fd, events, sizeof(events));
            if (bytes <= 0)
            {
                break;
            }

            const auto count = static_cast<std::size_t>(bytes) / sizeof(input_event);
            for (std::size_t i = 0; i < count; ++i)
            {
                const auto& ev = events[i];
                if (ev.type == EV_KEY)
                {
                    handleKey(ev.code, ev.value != 0);
                }
                else if (ev.type == EV_ABS)
                {
                    handleAbs(ev.code, ev.value);
                }
            }
        }
    }

    const std::array<unsigned, EAxis::MaxValue>& Axes() const override { return m_axes; }
    const std::array<bool, EAxis::MaxValue>& AxisSupported() const override
    {
        return m_axisSupported;
    }

    std::uint32_t Buttons() const override { return m_buttons; }
    unsigned Pov() const override
    {
        if (m_hatX == 0 && m_hatY == 0)
        {
            return Constants::PovCenteredVal;
        }

        if (m_hatY == -1)
        {
            if (m_hatX == 0)
                return 0;
            if (m_hatX == 1)
                return 4500;
            return 31500;
        }

        if (m_hatY == 1)
        {
            if (m_hatX == 0)
                return 18000;
            if (m_hatX == 1)
                return 13500;
            return 22500;
        }

        if (m_hatX == 1)
            return 9000;
        if (m_hatX == -1)
            return 27000;

        return Constants::PovCenteredVal;
    }

    unsigned ButtonCount() const override { return 12; }

  private:
    void reset()
    {
        m_axes.fill(Constants::MidAngle);
        m_axisSupported.fill(false);
        m_axisSupported[axisIndex(EAxis::X)] = true;
        m_axisSupported[axisIndex(EAxis::Y)] = true;
        m_axisSupported[axisIndex(EAxis::Z)] = true;
        m_axisSupported[axisIndex(EAxis::R)] = true;
        m_buttons = 0;
        m_hatX = 0;
        m_hatY = 0;
    }

    void openDevice()
    {
        QDir byId(QStringLiteral("/dev/input/by-id"));
        if (!byId.exists())
        {
            return;
        }

        const auto entries = byId.entryList(QDir::Files | QDir::System);
        for (const auto& entry : entries)
        {
            if (!entry.contains(QStringLiteral("Jess_Tech_Dual_Analog_Pad"), Qt::CaseInsensitive) ||
                !entry.endsWith(QStringLiteral("event-joystick")))
            {
                continue;
            }

            const QString path = byId.absoluteFilePath(entry);
            const int fd = ::open(path.toLocal8Bit().constData(), O_RDONLY | O_NONBLOCK);
            if (fd >= 0)
            {
                m_fd = fd;
                if (qEnvironmentVariableIsSet("IPPONBOARD_DEBUG_GAMEPAD"))
                {
                    qInfo().noquote() << "[JessTech] using" << path;
                }
                break;
            }

            if (qEnvironmentVariableIsSet("IPPONBOARD_DEBUG_GAMEPAD"))
            {
                qWarning().noquote() << "[JessTech] failed to open" << path << ":"
                                     << QString::fromLocal8Bit(std::strerror(errno));
            }
        }
    }

    static unsigned scaleAxis(int value, int min, int max)
    {
        const int clamped = std::clamp(value, min, max);
        const int range = max - min;
        if (range <= 0)
        {
            return Constants::MidAngle;
        }

        const double normalized = (static_cast<double>(clamped - min) / static_cast<double>(range));
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

    static EButton mapButton(int code)
    {
        switch (code)
        {
        case BTN_TRIGGER:
            return EButton::button1;
        case BTN_THUMB:
            return EButton::button2;
        case BTN_THUMB2:
            return EButton::button3;
        case BTN_TOP:
            return EButton::button4;
        case BTN_TOP2:
            return EButton::button5;
        case BTN_PINKIE:
            return EButton::button6;
        case BTN_BASE:
            return EButton::button7;
        case BTN_BASE2:
            return EButton::button8;
        case BTN_BASE3:
            return EButton::button9;
        case BTN_BASE4:
            return EButton::button10;
        case BTN_BASE5:
            return EButton::button11;
        case BTN_BASE6:
            return EButton::button12;
        default:
            return EButton::button32;
        }
    }

    void handleKey(int code, bool pressed)
    {
        const auto button = mapButton(code);
        if (button == EButton::button32)
        {
            return;
        }

        const auto mask = buttonMask(button);
        if (pressed)
        {
            m_buttons |= mask;
        }
        else
        {
            m_buttons &= ~mask;
        }
    }

    void handleAbs(int code, int value)
    {
        switch (code)
        {
        case ABS_X:
            m_axes[axisIndex(EAxis::X)] = scaleAxis(value, 0, 255);
            break;
        case ABS_Y:
            m_axes[axisIndex(EAxis::Y)] = scaleAxis(value, 0, 255);
            break;
        case ABS_Z:
            m_axes[axisIndex(EAxis::Z)] = scaleAxis(value, 0, 255);
            break;
        case ABS_RZ:
            m_axes[axisIndex(EAxis::R)] = scaleAxis(value, 0, 255);
            break;
        case ABS_HAT0X:
            m_hatX = std::clamp(value, -1, 1);
            break;
        case ABS_HAT0Y:
            m_hatY = std::clamp(value, -1, 1);
            break;
        default:
            break;
        }
    }

    int m_fd{ -1 };
    std::array<unsigned, EAxis::MaxValue> m_axes{};
    std::array<bool, EAxis::MaxValue> m_axisSupported{};
    std::uint32_t m_buttons{ 0 };
    int m_hatX{ 0 };
    int m_hatY{ 0 };
};

std::unique_ptr<JessTechPadAdapter> CreateJessTechPadAdapter(const QString& deviceName)
{
    if (!deviceName.contains(QStringLiteral("Jess Tech Dual Analog Pad"), Qt::CaseInsensitive))
    {
        return nullptr;
    }

    auto adapter = std::make_unique<JessTechPadLinux>();
    if (!adapter->Available())
    {
        return nullptr;
    }

    return adapter;
}

} // namespace GamepadLib
