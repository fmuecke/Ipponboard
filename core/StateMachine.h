// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__STATEMACHINE_H_
#define BASE__STATEMACHINE_H_

#include "Enums.h"
#include "Score.h"
#include "iControllerCore.h"

namespace Ipponboard
{

class IpponboardSM
{
  public:
    explicit IpponboardSM(IControllerCore& core) : m_core(core) {}

    void ToggleMainTimer();
    void BeginHold(ContestSide who);
    void EndHold();
    void SetHoldOwner(ContestSide who);
    void ClearHoldOwner();
    void AwardPoint(Score::Point point, ContestSide who);
    void RevokePoint(Score::Point point, ContestSide who);
    void AwardShido(ContestSide who);
    void RevokeShido(ContestSide who);
    void RevokeHansokumake(ContestSide who);
    void AwardHansokumake(ContestSide who);
    void ResetContest();
    void FinishContest();
    void OnMainTimerElapsed();
    void OnHoldTimerTick(int seconds);

    [[nodiscard]] EState CurrentState() const noexcept { return m_state; }
    [[nodiscard]] ContestSide CurrentHoldSide() const noexcept { return m_holder; }

  private:
    void handleRunningWazaari(ContestSide who);
    void handleRunningShido(ContestSide who);
    void maybeStopForGoldenScore(Score::Point point);
    void maybeStopForGoldenScoreAfterPenalty();
    void stopContest();

    void resetContest();
    void saveContest();
    void startMainTimer();
    void stopMainTimer();
    void startHoldTimer();
    void stopHoldTimer();
    void stopAllTimers();
    void awardPoint(Score::Point point, ContestSide who);
    void revokePoint(Score::Point point, ContestSide who);
    void awardIppon(ContestSide who);
    void awardShido(ContestSide who);
    void revokeShidoOrHansokumake(ContestSide who);
    void awardHansokumake(ContestSide who);
    void applyHoldScore(int seconds, ContestSide who);

    [[nodiscard]] bool canAddWazaari(ContestSide who) const;
    [[nodiscard]] bool isWazaariMatchPoint(ContestSide who) const;
    [[nodiscard]] bool canTakeShido(ContestSide who) const;
    [[nodiscard]] bool isShidoMatchPoint(ContestSide who) const;
    [[nodiscard]] bool hasIpponTime(int seconds) const;
    [[nodiscard]] bool hasWazaariTime(int seconds) const;
    [[nodiscard]] bool hasAwaseteTime(int seconds) const;
    [[nodiscard]] bool hasYukoTime(int seconds) const;
    [[nodiscard]] bool mainTimeIsUp() const;
    [[nodiscard]] int compareScore() const;

    [[nodiscard]] Score& score(ContestSide who) { return m_core.get_score(who); }
    [[nodiscard]] const Score& score(ContestSide who) const { return m_core.get_score(who); }

    IControllerCore& m_core;
    EState m_state{ eState_TimerStopped };
    ContestSide m_holder{ ContestSide::None };
};

} // namespace Ipponboard

#endif // BASE__STATEMACHINE_H_
