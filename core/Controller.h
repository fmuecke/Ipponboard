// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__CONTROLLER_H_
#define BASE__CONTROLLER_H_

#include "Competition.h"
#include "CompetitionMode.h"
#include "CompetitionModel.h"
#include "CompetitionNavigator.h"
#include "CompetitionRepository.h"
#include "Score.h"
#include "StateMachine.h"
#include "TimerService.h"
#include "iController.h"
#include "iControllerCore.h"

#include <QObject>
#include <QTime>
#include <bitset>
#include <memory>
#include <set>
#include <vector>

// forwards
class QSoundEffect;
//class CompetitionModel;

namespace Ipponboard
{
// forwards
class IView;
class IGoldenScoreView;
class AbstractRules;

class Controller : public QObject, public IController, public IControllerCore
{
    Q_OBJECT

  public:
    static const char* const msg_Ippon;
    static const char* const msg_WazariAwaseteIppon;
    static const char* const msg_Wazaari;
    static const char* const msg_Yuko;
    static const char* const msg_Shido;
    static const char* const msg_Hansokumake;
    static const char* const msg_Osaekomi;
    static const char* const msg_SonoMama;
    //static const char* const msg_Hantei;
    static const char* const msg_Hikiwaki;
    static const char* const msg_Winner;

    Controller();
    ~Controller() override;

    // --- IController ---
    //FIXME: use override/final
    void InitCompetition(CompetitionMode const& mode);
    void RegisterView(IView* pView) override;
    void RegisterView(IGoldenScoreView* pView) override;
    int GetScore(Ipponboard::ContestSide whos, Ipponboard::Score::Point point) const override;
    void DoAction(Ipponboard::EAction action,
                  Ipponboard::ContestSide who = Ipponboard::ContestSide::SideA,
                  bool doRevoke = false) override;
    Ipponboard::EState GetCurrentState() const override { return m_State; }
    Ipponboard::ContestSide GetLeadingSide() const override;
    Ipponboard::ContestSide GetCurrentHoldSide() const override;
    QString GetTimeText(Ipponboard::ETimer timer) const override;
    QString GetAthleteName(Ipponboard::ContestSide who) const override;
    QString GetAthleteLastName(Ipponboard::ContestSide) const override;
    QString GetAthleteFirstName(Ipponboard::ContestSide) const override;
    QString GetAthleteClub(Ipponboard::ContestSide who) const override;
    QString const& GetWeight() const override;
    QString GetMessage() const override;
    int GetTeamScore(Ipponboard::ContestSide who) const override;
    void SetTimerValue(Ipponboard::ETimer timer, const QString& value) override;
    void SetRoundTime(const QString& value) override;
    QString GetTimeOverridesString() const;
    int GetContestDuration(QString const& weight) const;
    void SetRoundTime(const QTime& time);
    void OverrideRoundTimeOfContestMode(int contestTimeSecs);
    //FIXME: int GetRound() const;
    void SetWeightClass(QString const& c) override;
    QString const& GetCategoryName() const override
    {
        return m_weight_class;
    } //TODO: weight class should be part of competition!
    void SetGoldenScore(bool isGS) override;
    bool IsGoldenScore() const override { return is_golden_score(); }
    void SetRules(std::shared_ptr<AbstractRules> rules) override;
    std::shared_ptr<AbstractRules> GetRules() const override;
    bool IsAutoAdjustPoints() const final;
    void SetAutoAdjustPoints(bool isActive) final;
    void SetOption(Ipponboard::EOption option, bool isSet) override;
    bool GetOption(Ipponboard::EOption option) const override;
    QString GetHomeLabel() const override { return m_labelHome; }
    QString GetGuestLabel() const override { return m_labelGuest; }
    void SetLabels(QString const& home, QString const& guest) override;

    void Gong() const;
    // Allows tests to advance timers deterministically without relying on QTimer events.
    void AdvanceTimerTicks(Ipponboard::ETimer timer, int ticks = 1);

    IController const* GetIController() const { return this; }
    IController* GetIController() { return this; }

    // --- IControllerCore ---
  private:
    void start_timer(ETimer t) final;
    void stop_timer(ETimer t) final;
    void save_contest() final;
    void reset_contest() final;
    void reset_timer(ETimer) final;
    Score& get_score(Ipponboard::ContestSide who) final;
    Score const& get_score(Ipponboard::ContestSide who) const final;
    int get_time(ETimer) const final;
    bool is_sonomama() const final;
    bool is_golden_score() const final;
    bool is_option(Ipponboard::EOption option) const final
    {
        return GetOption(option);
    } // TODO: use GetOption!
    bool is_auto_adjust() const final { return IsAutoAdjustPoints(); }

  public:
    // --- other functions ---
    int GetContestCount() const { return static_cast<int>(m_navigator.contestCount()); }

    int GetRoundCount() const { return static_cast<int>(m_navigator.roundCount()); }

    void NextContest();
    void PrevContest();
    void SetCurrentContest(unsigned int index);

    int GetCurrentContest() const { return static_cast<int>(m_navigator.currentContest()); }

    void SetCurrentRound(unsigned int index);

    int GetCurrentRound() const { return static_cast<int>(m_navigator.currentRound()); }

    void ClearContestsAndResetTimers();
    void SetClub(Ipponboard::ContestSide whos, const QString& clubName);
    void SetContest(unsigned int roundIndex, unsigned int contestIndex, Contest contest);
    void SetContest(unsigned int roundIndex, unsigned int contestIndex, const QString& weight,
                    const QString& sideAName, const QString& sideAClub, const QString& sideBName,
                    const QString& sideBClub, int yukoSideA = -1, int wazaariSideA = -1,
                    int ipponSideA = -1, int shidoSideA = -1, int hansokumakeSideA = -1,
                    int yukoSideB = -1, int wazaariSideB = -1, int ipponSideB = -1,
                    int shidoSideB = -1, int hansokumakeSideB = -1);
    Ipponboard::Contest const& GetContest(unsigned int roundIndex, unsigned int contestIndex) const;
    void SetAthleteName(Ipponboard::ContestSide whos, const QString& name);

    void SetWeights(QStringList const& weights);
    void CopyAndSwitchGuestAthletes();
    PCompetitionModel GetCompetitionScoreModel(int which = 0);

    void SetMatSignal(const QString&);
    QString const& GetMatSignal() const;

  private slots:
    void update_main_time();
    void update_hold_time();

  private:
    //	void AddPoint_( Ipponboard::ContestSide whos, Ipponboard::Score::Point point );
    //	void RemovePoint_( Ipponboard::ContestSide whos, Ipponboard::Score::Point point );
    //	void StartStopTimer_( Ipponboard::ETimer timer );
    //	void UpdatePointsFromHoldTimer_();
    //	void CorrectState_();
    void update_views() const;
    //	bool IsTimeLeft_() const
    //	{
    //		return  *m_pTimeMain > QTime(0,0,1) &&
    //				*m_pTimeMain < QTime(23,0,0);
    //	}
    void reset();
    void reset_timer_value(Ipponboard::ETimer timer);

    inline Ipponboard::Contest& current_contest()
    {
        return m_Competition.at(m_navigator.currentRound())->at(m_navigator.currentContest());
    }

    inline Ipponboard::Contest const& current_contest() const
    {
        return m_Competition.at(m_navigator.currentRound())->at(m_navigator.currentContest());
    }

    Ipponboard::CompetitionMode m_mode;
    Ipponboard::Competition m_Competition;
    std::vector<std::shared_ptr<CompetitionModel>> m_CompetitionModels;
    CompetitionNavigator m_navigator;
    CompetitionRepository m_repository;

    std::unique_ptr<Ipponboard::IpponboardSM> m_pSM;
    Ipponboard::EState m_State;
    TimerService m_timerService;
    QTime m_mainTime;
    QTime m_holdTime; // needed when side is not chosen yet
    std::set<IView*> m_views;
    std::set<IGoldenScoreView*> m_goldenScoreViews;
    QString m_Message;
    QString m_matSignal;
    bool m_isSonoMama;
    QTime m_roundTime;
    QString m_weight_class;
    std::bitset<eOption_MAX> m_options;
    bool m_isAutoAdjustPoints{ true };
    QString m_labelHome;
    QString m_labelGuest;
    mutable std::unique_ptr<QSoundEffect> m_gongEffect;
    void reset_timers();
    std::shared_ptr<AbstractRules> m_rules;
    void applyContestChange();
};
} // namespace Ipponboard

#endif // BASE__CONTROLLER_H_
