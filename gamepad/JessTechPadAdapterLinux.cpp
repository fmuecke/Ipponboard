#include "JessTechPadAdapter.h"

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QString>
#include <QtGlobal>

#include <algorithm>
#include <cmath>
#include <cerrno>
#include <cstring>

#include <fcntl.h>
#include <linux/input.h>
#include <unistd.h>

namespace FMlib
{
namespace
{
constexpr std::uint32_t buttonMask(Gamepad::EButton button)
{
    return 1u << static_cast<unsigned>(button);
}

constexpr std::size_t axisIndex(Gamepad::EAxis axis)
{
    return static_cast<std::size_t>(axis);
}

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

    const std::array<unsigned, Gamepad::eAxis_MAX>& Axes() const override { return m_axes; }
    const std::array<bool, Gamepad::eAxis_MAX>& AxisSupported() const override
    {
        return m_axisSupported;
    }

    std::uint32_t Buttons() const override { return m_buttons; }
    unsigned Pov() const override
    {
        if (m_hatX == 0 && m_hatY == 0)
        {
            return Gamepad::kPovCentered;
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

        return Gamepad::kPovCentered;
    }

    unsigned ButtonCount() const override { return 12; }

  private:
    void reset()
    {
        m_axes.fill(Gamepad::eMid);
        m_axisSupported.fill(false);
        m_axisSupported[axisIndex(Gamepad::eAxis_X)] = true;
        m_axisSupported[axisIndex(Gamepad::eAxis_Y)] = true;
        m_axisSupported[axisIndex(Gamepad::eAxis_Z)] = true;
        m_axisSupported[axisIndex(Gamepad::eAxis_R)] = true;
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
            return Gamepad::eMid;
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
        return static_cast<unsigned>(std::clamp(scaled, 0.0, 1.0) * Gamepad::eMax + 0.5);
    }

    static Gamepad::EButton mapButton(int code)
    {
        switch (code)
        {
        case BTN_TRIGGER:
            return Gamepad::eButton1;
        case BTN_THUMB:
            return Gamepad::eButton2;
        case BTN_THUMB2:
            return Gamepad::eButton3;
        case BTN_TOP:
            return Gamepad::eButton4;
        case BTN_TOP2:
            return Gamepad::eButton5;
        case BTN_PINKIE:
            return Gamepad::eButton6;
        case BTN_BASE:
            return Gamepad::eButton7;
        case BTN_BASE2:
            return Gamepad::eButton8;
        case BTN_BASE3:
            return Gamepad::eButton9;
        case BTN_BASE4:
            return Gamepad::eButton10;
        case BTN_BASE5:
            return Gamepad::eButton11;
        case BTN_BASE6:
            return Gamepad::eButton12;
        default:
            return Gamepad::eButton32;
        }
    }

    void handleKey(int code, bool pressed)
    {
        const auto button = mapButton(code);
        if (button == Gamepad::eButton32)
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
            m_axes[axisIndex(Gamepad::eAxis_X)] = scaleAxis(value, 0, 255);
            break;
        case ABS_Y:
            m_axes[axisIndex(Gamepad::eAxis_Y)] = scaleAxis(value, 0, 255);
            break;
        case ABS_Z:
            m_axes[axisIndex(Gamepad::eAxis_Z)] = scaleAxis(value, 0, 255);
            break;
        case ABS_RZ:
            m_axes[axisIndex(Gamepad::eAxis_R)] = scaleAxis(value, 0, 255);
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
    std::array<unsigned, Gamepad::eAxis_MAX> m_axes{};
    std::array<bool, Gamepad::eAxis_MAX> m_axisSupported{};
    std::uint32_t m_buttons{ 0 };
    int m_hatX{ 0 };
    int m_hatY{ 0 };
};
} // namespace

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

} // namespace FMlib
