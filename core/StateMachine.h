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
    void BeginHold(FighterEnum who);
    void EndHold();
    void SetHoldOwner(FighterEnum who);
    void ClearHoldOwner();
    void AwardPoint(Score::Point point, FighterEnum who);
    void RevokePoint(Score::Point point, FighterEnum who);
    void AwardShido(FighterEnum who);
    void RevokeShido(FighterEnum who);
    void RevokeHansokumake(FighterEnum who);
    void AwardHansokumake(FighterEnum who);
    void ResetFight();
    void FinishFight();
    void OnMainTimerElapsed();
    void OnHoldTimerTick(int seconds);

    [[nodiscard]] EState CurrentState() const noexcept { return m_state; }
    [[nodiscard]] FighterEnum CurrentHolder() const noexcept { return m_holder; }

  private:
    void handleRunningWazaari(FighterEnum who);
    void handleRunningShido(FighterEnum who);
    void maybeStopForGoldenScore(Score::Point point);
    void maybeStopForGoldenScoreAfterPenalty();
    void stopFight();

    void resetFight();
    void saveFight();
    void startMainTimer();
    void stopMainTimer();
    void startHoldTimer();
    void stopHoldTimer();
    void stopAllTimers();
    void awardPoint(Score::Point point, FighterEnum who);
    void revokePoint(Score::Point point, FighterEnum who);
    void awardIppon(FighterEnum who);
    void awardShido(FighterEnum who);
    void revokeShidoOrHansokumake(FighterEnum who);
    void awardHansokumake(FighterEnum who);
    void applyHoldScore(int seconds, FighterEnum who);

    [[nodiscard]] bool canAddWazaari(FighterEnum who) const;
    [[nodiscard]] bool isWazaariMatchPoint(FighterEnum who) const;
    [[nodiscard]] bool canTakeShido(FighterEnum who) const;
    [[nodiscard]] bool isShidoMatchPoint(FighterEnum who) const;
    [[nodiscard]] bool hasIpponTime(int seconds) const;
    [[nodiscard]] bool hasWazaariTime(int seconds) const;
    [[nodiscard]] bool hasAwaseteTime(int seconds) const;
    [[nodiscard]] bool hasYukoTime(int seconds) const;
    [[nodiscard]] bool mainTimeIsUp() const;
    [[nodiscard]] int compareScore() const;

    [[nodiscard]] Score& score(FighterEnum who) { return m_core.get_score(who); }
    [[nodiscard]] const Score& score(FighterEnum who) const { return m_core.get_score(who); }

    IControllerCore& m_core;
    EState m_state{ eState_TimerStopped };
    FighterEnum m_holder{ FighterEnum::Nobody };
};

} // namespace Ipponboard

#endif // BASE__STATEMACHINE_H_
