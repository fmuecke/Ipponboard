#pragma once

#include "Enums.h"

#include <QObject>
#include <QTimer>

namespace Ipponboard
{

class TimerService : public QObject
{
    Q_OBJECT

  public:
    explicit TimerService(QObject* parent = nullptr);

    void startTimer(ETimer timer);
    void stopTimer(ETimer timer);
    bool isActive(ETimer timer) const;
    void advanceTicks(ETimer timer, int ticks);

  signals:
    void mainTimeout();
    void holdTimeout();

  private:
    QTimer m_mainTimer;
    QTimer m_holdTimer;
};

} // namespace Ipponboard
