#include "TimerService.h"

#include <array>

namespace Ipponboard
{
namespace
{
constexpr int kTimerIntervalMs = 1000;
}

TimerService::TimerService(QObject* parent) : QObject(parent)
{
    m_mainTimer.setParent(this);
    m_holdTimer.setParent(this);

    m_mainTimer.setInterval(kTimerIntervalMs);
    m_holdTimer.setInterval(kTimerIntervalMs);

    QObject::connect(&m_mainTimer, &QTimer::timeout, this, &TimerService::mainTimeout);
    QObject::connect(&m_holdTimer, &QTimer::timeout, this, &TimerService::holdTimeout);
}

void TimerService::startTimer(ETimer timer)
{
    if (timer == eTimer_Main)
    {
        m_mainTimer.start();
    }
    else
    {
        m_holdTimer.start();
    }
}

void TimerService::stopTimer(ETimer timer)
{
    if (timer == eTimer_Main)
    {
        m_mainTimer.stop();
    }
    else
    {
        m_holdTimer.stop();
    }
}

bool TimerService::isActive(ETimer timer) const
{
    return (timer == eTimer_Main) ? m_mainTimer.isActive() : m_holdTimer.isActive();
}

void TimerService::advanceTicks(ETimer timer, int ticks)
{
    if (ticks <= 0)
    {
        return;
    }

    auto const signal =
        (timer == eTimer_Main) ? &TimerService::mainTimeout : &TimerService::holdTimeout;

    for (int i = 0; i < ticks; ++i)
    {
        emit(this->*signal)();
    }
}

} // namespace Ipponboard
