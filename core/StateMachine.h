#ifndef BASE__STATEMACHINE_H_
#define BASE__STATEMACHINE_H_

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE	50
#define BOOST_MPL_LIMIT_MAP_SIZE	50

// workaround for following error:
//   boost/type_traits/detail/has_binary_operator.hp(50): Parse error at "BOOST_JOIN"
#ifndef Q_MOC_RUN

#include <iostream>
#include <string>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#include "Enums.h"
#include "Score.h"
#include "iControllerCore.h"

namespace msm = boost::msm;

namespace Ipponboard
{
// forwards
class Controller;

static char const* const state_names[] = { "Stopped", "Running", "Ended", "Holding" };
static char const* const point_type_names[] = { "Ippon", "Wazaari", "Yuko", "Shido", "Hansokumake" };
static char const* const fighter_type_names[] = { "first", "second" };
static char const* const timer_type_names[] = { "main", "hold", "all" };

class IpponboardSM_ : public msm::front::state_machine_def<IpponboardSM_>
{
public:
	IpponboardSM_(IControllerCore* core) : m_pCore(core) {}
	IpponboardSM_() : m_pCore(0) {}

	void SetCore(IControllerCore* core) { m_pCore = core; }
	virtual ~IpponboardSM_() {}

	//
	// events
	//
	struct Reset {};
	struct Finish {};

	struct Hajime_Mate	{ enum { type = eTimer_Main }; };
	struct Osaekomi_Toketa { enum { type = eTimer_Hold }; };

	template<typename T>
	struct PointEvent
	{
		PointEvent(Ipponboard::EFighter f)
			: tori(f) {}
		Ipponboard::EFighter tori;
	};

	struct ippon_type
	{ enum { type = Ipponboard::ePoint_Ippon, revoke = false }; };
	struct wazaari_type
	{ enum { type = Ipponboard::ePoint_Wazaari, revoke = false }; };
	struct yuko_type
	{ enum { type = Ipponboard::ePoint_Yuko, revoke = false }; };
	struct shido_type
	{ enum { type = Ipponboard::ePoint_Shido, revoke = false }; };
	struct hansokumake_type
	{ enum { type = Ipponboard::ePoint_Hansokumake, revoke = false }; };
	struct revoke_ippon_type
	{ enum { type = Ipponboard::ePoint_Ippon, revoke = true }; };
	struct revoke_wazaari_type
	{ enum { type = Ipponboard::ePoint_Wazaari, revoke = true }; };
	struct revoke_yuko_type
	{ enum { type = Ipponboard::ePoint_Yuko, revoke = true }; };
	struct revoke_shido_hm_type	{};

	typedef PointEvent<ippon_type	   > Ippon;
	typedef PointEvent<wazaari_type	   > Wazaari;
	typedef PointEvent<yuko_type	   > Yuko;
	typedef PointEvent<shido_type	   > Shido;
	typedef PointEvent<hansokumake_type> Hansokumake;

	typedef PointEvent<revoke_ippon_type   > RevokeIppon;
	typedef PointEvent<revoke_wazaari_type > RevokeWazaari;
	typedef PointEvent<revoke_yuko_type    > RevokeYuko;
	typedef PointEvent<revoke_shido_hm_type> RevokeShidoHM;


	template<typename T>
	struct TimeEvent
	{
		TimeEvent(int s, Ipponboard::EFighter f) : secs(s), tori(f) {}
		int secs;
		Ipponboard::EFighter tori;
	};
	struct hold_timer_type
	{
		enum
		{
			type = Ipponboard::eTimer_Hold
		};
	};
	typedef TimeEvent<hold_timer_type> HoldTimeEvent;

	struct TimeEndedEvent {};

	//
	// states
	//

	// Stopped
	struct Stopped : public msm::front::state<>
	{
	};

	// Running
	struct Running : public msm::front::state<>
	{
	};

	// Ended
	struct Ended : public msm::front::state<>
	{
	};

	// Holding
	struct Holding : public msm::front::state<>
	{
	};


	// Initial state
	typedef Stopped initial_state;

	//--------------------
	// transition actions
	//--------------------
	void reset(Reset const& /*evt*/);
	void save(Finish const& /*evt*/);

	void stop_timer(Osaekomi_Toketa const& /*evt*/);
	void stop_timer(TimeEndedEvent const& /*evt*/);
	void stop_timer(Hajime_Mate const& /*evt*/);
	void stop_timer(Finish const& /*evt*/);
	template<typename T>
	void stop_timer(T const& /*evt*/)
	{
		m_pCore->stop_timer(ETimer(T::type));
	}

	void add_point(Ippon const& evt);
	void add_point(Shido const& evt);
	void add_point(PointEvent<revoke_shido_hm_type> const& evt);
	void add_point(Hansokumake const& evt);
	void add_point(HoldTimeEvent const& evt);
	template<typename T>
	void add_point(PointEvent<T> const& evt)
	{
		if (T::revoke)
			Score_(evt.tori).Remove(EPoint(T::type));
		else
			Score_(evt.tori).Add(EPoint(T::type));
	}
	template <typename T>
	void add_point_stop_timer(T const& evt)
	{
		add_point(evt);
		m_pCore->stop_timer(eTimer_Hold);
		m_pCore->stop_timer(eTimer_Main);
	}

	void start_timer(Hajime_Mate const& /*evt*/);
	void start_timer(Osaekomi_Toketa const& /*evt*/);
	//template<typename T>
	//void start_timer(T const& /*evt*/);
	//not used currently...
	//{
	//	m_pCore->StartTimer_( ETimer(T::type) );
	//}

	void yoshi(Osaekomi_Toketa const& /*evt*/);

	//------------------
	// guard conditions
	//------------------
	template<class T>
	bool time_is_left(T const& /*evt*/)
	{
		return 0 != m_pCore->get_time(eTimer_Main);
	}

	template<class T>
	bool time_is_up(T const& /*evt*/)
	{
		return 0 == m_pCore->get_time(eTimer_Main);
	}

	bool has_wazaari(Wazaari const& evt);
	bool has_2wazaari(RevokeWazaari const& evt);
	bool has_no_wazaari(Wazaari const& evt);
	bool has_IpponTime(HoldTimeEvent const& evt);
	bool has_WazaariTime(HoldTimeEvent const& evt);
	bool has_AwaseteTime(HoldTimeEvent const& evt);
	bool has_YukoTime(HoldTimeEvent const& evt);
	bool is_sonomama(Osaekomi_Toketa const& evt);
	bool has_enough_shido(Shido const& evt);
	bool can_take_shido(Shido const& evt);

	typedef Ipponboard::IpponboardSM_ sm; // makes transition table cleaner

	// Transition table for board
	struct transition_table : boost::mpl::vector <
			//		Start		Event			Next	  Action				Guard
			//	  +---------+---------------+-----------+-------------------+-----------------------+
			a_row < Stopped , Hajime_Mate	, Running	, &sm::start_timer							>,
			row < Stopped , Shido			, Stopped	, &sm::add_point	, &sm::has_enough_shido	>,
			row < Stopped , Shido			, Stopped	, &sm::add_point	, &sm::can_take_shido	>,
			a_row < Stopped , Hansokumake	, Stopped	, &sm::add_point							>,
			a_row < Stopped , Reset         , Stopped	, &sm::reset								>,
			a_row < Stopped , Finish		, Stopped   , &sm::save									>,
			//row < Stopped , Osaekomi_Toketa, Holding	, &sm::yoshi		, &sm::is_sonomama		>,
			a_row < Stopped , Osaekomi_Toketa, Holding	, &sm::yoshi								>,	// JUST FOR CONVENIENCE !!!
			a_row < Stopped , RevokeShidoHM	, Stopped	, &sm::add_point							>,
			a_row < Stopped	, RevokeWazaari	, Stopped	, &sm::add_point							>,
			a_row < Stopped	, RevokeYuko	, Stopped	, &sm::add_point							>,
			a_row < Stopped , Ippon			, Stopped	, &sm::add_point							>,	// just to correct values...
			a_row < Stopped , RevokeIppon	, Stopped	, &sm::add_point							>,	// just to correct values...
			row < Stopped , Wazaari		, Stopped	, &sm::add_point	, &sm::has_wazaari		>,	// just to correct values...
			a_row < Stopped , Wazaari		, Stopped	, &sm::add_point							>,	// just to correct values...
			a_row < Stopped , Yuko			, Stopped	, &sm::add_point							>,	// just to correct values...
			//	  +---------+---------------+-----------+-------------------+-----------------------+
			a_row < Running , Hajime_Mate	, Stopped	, &sm::stop_timer					 		>,
			a_row < Running , TimeEndedEvent, Stopped	, &sm::stop_timer							>,
			a_row < Running , Ippon			, Stopped	, &sm::add_point							>,
			a_row < Running , Wazaari		, Running	, &sm::add_point							>,
			row < Running , Wazaari		, Stopped	, &sm::add_point_stop_timer, &sm::has_wazaari		>,
			a_row < Running , Yuko			, Running	, &sm::add_point							>,
			a_row < Running	, Reset			, Stopped	, &sm::reset								>,
			a_row < Running , Finish		, Stopped	, &sm::stop_timer							>,
			a_row < Running , Osaekomi_Toketa, Holding	, &sm::start_timer							>,
			a_row < Running	, RevokeWazaari	, Running	, &sm::add_point							>,
			a_row < Running	, RevokeYuko	, Running	, &sm::add_point							>,
			a_row < Running , RevokeShidoHM	, Running	, &sm::add_point							>,	// just to correct values...
			a_row < Running , Hansokumake	, Stopped	, &sm::add_point							>,	// just to correct values...
			row < Running , Shido			, Stopped	, &sm::add_point_stop_timer	, &sm::has_enough_shido	>,	// just to correct values...
			row < Running , Shido			, Running	, &sm::add_point			, &sm::can_take_shido	>,	// just to correct values...
			//	  +---------+---------------+-----------+-------------------+-----------------------+
			row < Holding , Osaekomi_Toketa, Running	, &sm::stop_timer			, &sm::time_is_left		>,
			row < Holding , Osaekomi_Toketa, Stopped	, &sm::stop_timer			, &sm::time_is_up		>,
			a_row < Holding , Hajime_Mate	, Stopped	, &sm::stop_timer									>,
			a_row < Holding , Reset			, Stopped	, &sm::reset										>,
			a_row < Holding , Finish			, Stopped	, &sm::stop_timer									>,
			a_row < Holding , Hansokumake	, Stopped	, &sm::add_point									>,
			a_row < Holding , Ippon			, Stopped	, &sm::add_point									>,
			a_row < Holding , Wazaari		, Holding	, &sm::add_point									>,	// just to correct values...
			a_row < Holding , Yuko			, Holding	, &sm::add_point									>,	// just to correct values...
			a_row < Holding , Shido			, Holding	, &sm::add_point									>,	// just to correct values...
			a_row < Holding , RevokeWazaari	, Holding	, &sm::add_point									>,	// just to correct values...
			a_row < Holding , RevokeYuko		, Holding	, &sm::add_point									>,	// just to correct values...
			a_row < Holding , RevokeShidoHM	, Holding	, &sm::add_point									>,	// just to correct values...
			//
			// Note: Transitions are processed bottom up!
			row < Holding , HoldTimeEvent	, Holding	, &sm::add_point			, &sm::has_YukoTime			>,
			row < Holding , HoldTimeEvent	, Holding	, &sm::add_point			, &sm::has_WazaariTime			>,
			row < Holding , HoldTimeEvent	, Stopped	, &sm::add_point_stop_timer	, &sm::has_AwaseteTime>,
			row < Holding , HoldTimeEvent	, Stopped	, &sm::add_point_stop_timer	, &sm::has_IpponTime			>
			//	  +---------+---------------+-----------+-------------------+----------------------+
			> {};
	// Replaces the default no-transition response.
	template <typename FSM, typename Event>
	void no_transition(Event const&, FSM&, int /*state*/)
	{
		//		e;
		//		std::cout << "no transition from state " << state
		//			<< " on event " << typeid(e).name() << std::endl;
	}

private:
	inline Score& Score_(EFighter who)
	{ return m_pCore->get_score(who); }
	inline const Score& Score_(EFighter who) const
	{ return m_pCore->get_score(who); }

	IControllerCore* m_pCore;
};

typedef msm::back::state_machine<IpponboardSM_> IpponboardSM;

} // namespace Ipponboard

#endif  // Q_MOC_RUN

#endif  // BASE__STATEMACHINE_H_
