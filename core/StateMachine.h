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

class IpponboardSM_
{
  public:
    IpponboardSM_(IControllerCore* core) : m_pCore(core) {}
    IpponboardSM_() : m_pCore(nullptr) {}

    void SetCore(IControllerCore* core) { m_pCore = core; }
    virtual ~IpponboardSM_() = default;

    struct Reset
    {
    };
    struct Finish
    {
    };

    struct Hajime_Mate
    {
        enum
        {
            type = eTimer_Main
        };
    };
    struct Osaekomi_Toketa
    {
        enum
        {
            type = eTimer_Hold
        };
    };

    template <typename T> struct PointEvent
    {
        PointEvent(Ipponboard::FighterEnum f) : tori(f) {}
        Ipponboard::FighterEnum tori;
    };

    struct ippon_type
    {
        enum
        {
            type = static_cast<short>(Ipponboard::Score::Point::Ippon),
            revoke = false
        };
    };
    struct wazaari_type
    {
        enum
        {
            type = static_cast<short>(Ipponboard::Score::Point::Wazaari),
            revoke = false
        };
    };
    struct yuko_type
    {
        enum
        {
            type = static_cast<short>(Ipponboard::Score::Point::Yuko),
            revoke = false
        };
    };
    struct shido_type
    {
        enum
        {
            type = static_cast<short>(Ipponboard::Score::Point::Shido),
            revoke = false
        };
    };
    struct hansokumake_type
    {
        enum
        {
            type = static_cast<short>(Ipponboard::Score::Point::Hansokumake),
            revoke = false
        };
    };
    struct revoke_ippon_type
    {
        enum
        {
            type = static_cast<short>(Ipponboard::Score::Point::Ippon),
            revoke = true
        };
    };
    struct revoke_wazaari_type
    {
        enum
        {
            type = static_cast<short>(Ipponboard::Score::Point::Wazaari),
            revoke = true
        };
    };
    struct revoke_yuko_type
    {
        enum
        {
            type = static_cast<short>(Ipponboard::Score::Point::Yuko),
            revoke = true
        };
    };
    struct revoke_shido_hm_type
    {
    };

    using Ippon = PointEvent<ippon_type>;
    using Wazaari = PointEvent<wazaari_type>;
    using Yuko = PointEvent<yuko_type>;
    using Shido = PointEvent<shido_type>;
    using Hansokumake = PointEvent<hansokumake_type>;

    using RevokeIppon = PointEvent<revoke_ippon_type>;
    using RevokeWazaari = PointEvent<revoke_wazaari_type>;
    using RevokeYuko = PointEvent<revoke_yuko_type>;
    using RevokeShidoHM = PointEvent<revoke_shido_hm_type>;

    template <typename T> struct TimeEvent
    {
        TimeEvent(int s, Ipponboard::FighterEnum f) : secs(s), tori(f) {}
        int secs;
        Ipponboard::FighterEnum tori;
    };
    struct hold_timer_type
    {
        enum
        {
            type = Ipponboard::eTimer_Hold
        };
    };
    using HoldTimeEvent = TimeEvent<hold_timer_type>;

    struct TimeEndedEvent
    {
    };

    void reset(Reset const& evt);
    void save(Finish const& evt);

    void stop_timer(Osaekomi_Toketa const& evt);
    void stop_timer(TimeEndedEvent const& evt);
    void stop_timer(Hajime_Mate const& evt);
    void stop_timer(Finish const& evt);
    template <typename T> void stop_timer(T const&) { m_pCore->stop_timer(ETimer(T::type)); }

    void add_point(Ippon const& evt);
    void add_point(Shido const& evt);
    void add_point(PointEvent<revoke_shido_hm_type> const& evt);
    void add_point(Hansokumake const& evt);
    void add_point(HoldTimeEvent const& evt);
    template <typename T> void add_point(PointEvent<T> const& evt)
    {
        if (T::revoke)
        {
            Score_(evt.tori).Remove(Score::Point(T::type));
        }
        else
        {
            Score_(evt.tori).Add(Score::Point(T::type));
        }
    }
    template <typename T> void add_point_stop_timer(T const& evt)
    {
        add_point(evt);
        m_pCore->stop_timer(eTimer_Hold);
        m_pCore->stop_timer(eTimer_Main);
    }

    void start_timer(Hajime_Mate const& evt);
    void start_timer(Osaekomi_Toketa const& evt);

    void yoshi(Osaekomi_Toketa const& evt);

    template <class T> bool time_is_left(T const&) { return 0 != m_pCore->get_time(eTimer_Main); }

    template <class T> bool time_is_up(T const&) { return 0 == m_pCore->get_time(eTimer_Main); }

    bool wazaari_is_match_point(Wazaari const& evt);
    bool can_add_wazaari(Wazaari const& evt);
    bool has_max_wazaari(RevokeWazaari const& evt);
    bool has_IpponTime(HoldTimeEvent const& evt);
    bool has_WazaariTime(HoldTimeEvent const& evt);
    bool has_AwaseteTime(HoldTimeEvent const& evt);
    bool has_YukoTime(HoldTimeEvent const& evt);
    bool is_sonomama(Osaekomi_Toketa const& evt);
    bool shido_is_match_point(Shido const& evt);
    bool can_take_shido(Shido const& evt);

  private:
    inline Score& Score_(FighterEnum who) { return m_pCore->get_score(who); }
    inline const Score& Score_(FighterEnum who) const { return m_pCore->get_score(who); }

    IControllerCore* m_pCore;
};

class IpponboardSM : public IpponboardSM_
{
  public:
    IpponboardSM() { start(); }
    explicit IpponboardSM(IControllerCore* core) : IpponboardSM_(core) { start(); }

    void start() noexcept;

    void process_event(Reset const& evt);
    void process_event(Finish const& evt);
    void process_event(Hajime_Mate const& evt);
    void process_event(Osaekomi_Toketa const& evt);
    void process_event(Ippon const& evt);
    void process_event(Wazaari const& evt);
    void process_event(Yuko const& evt);
    void process_event(Shido const& evt);
    void process_event(Hansokumake const& evt);
    void process_event(RevokeIppon const& evt);
    void process_event(RevokeWazaari const& evt);
    void process_event(RevokeYuko const& evt);
    void process_event(RevokeShidoHM const& evt);
    void process_event(HoldTimeEvent const& evt);
    void process_event(TimeEndedEvent const& evt);

    [[nodiscard]] EState current_state() const noexcept { return m_currentState; }

  private:
    [[nodiscard]] EState state() const noexcept;
    void set_state(EState state) noexcept;

    EState m_currentState{ eState_TimerStopped };
};

} // namespace Ipponboard

#endif // BASE__STATEMACHINE_H_
