#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#define BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS
#define BOOST_MPL_LIMIT_VECTOR_SIZE	50
#define BOOST_MPL_LIMIT_MAP_SIZE	50

#include <iostream>
#include <string>
#include <boost/msm/back/state_machine.hpp>
#include <boost/msm/front/state_machine_def.hpp>

#include "enums.h"
#include "score.h"
#include "icontrollercore.h"

namespace msm = boost::msm;

namespace Ipponboard
{
// forwards
class Controller;

static char const* const state_names[] = { "Stopped", "Running", "Ended", "Holding" };
static char const* const point_type_names[] = { "Ippon", "Wazaari", "Yuko", "Shido", "Hansokumake" };
static char const* const fighter_type_names[] = { "blue", "white" };
static char const* const timer_type_names[] = { "main", "hold", "all" };

class IpponboardSM_ : public msm::front::state_machine_def<IpponboardSM_>
{
public:
	IpponboardSM_( IControllerCore* core ) : m_pCore(core) {}
	IpponboardSM_() : m_pCore(0) {}

	void SetCore( IControllerCore* core ) { m_pCore = core; }
	virtual ~IpponboardSM_() {}

	//
	// events
	//
	struct Reset {};
	struct Finish {};

	struct Hajime_Matte	{ enum { type = eTimer_Main }; };
	//struct Osaekomi_Toketa { enum { type = eTimer_Holdi }; }; //FIXME!!
//	struct osaekomi_type_blue
//	{ enum { type = eTimer_Hold_Blue }; };
//	struct osaekomi_type_white
//	{ enum { type = eTimer_Hold_White }; };

	struct OsaekomiToketa
	{
		OsaekomiToketa( Ipponboard::ETimer t )
			: timer(t) {}
		Ipponboard::ETimer timer;
	};


	template<typename T>
	struct PointEvent
	{
		PointEvent( Ipponboard::EFighter f )
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
	{ enum { type = Ipponboard::eTimer_Holdi }; }; //FIXME!
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
	void reset(Reset const& /*evt*/)
	{
		m_pCore->ResetFight_();
	}

	template<class T>
	void stop_timer(T const& /*evt*/)
	{
		m_pCore->StopTimer_( ETimer(T::type) );
	}
	template<>
	void stop_timer( OsaekomiToketa const& evt )
	{
		m_pCore->StopTimer_( evt.timer );
	}
	template<>
	void stop_timer( TimeEndedEvent const& /*evt*/ )
	{
		m_pCore->StopTimer_( eTimer_Main );
	}
	template<>
	void stop_timer( Finish const& /*evt*/)
	{
		// Finish will be created if current fight should be saved
		// --> stop timers
		m_pCore->StopTimer_( eTimer_Holdi ); //FIXME!
		m_pCore->StopTimer_( eTimer_Main ); // will save main time
	}

	template<class T>
	void start_timer(T const& /*evt*/);
//	{
//		m_pCore->StartTimer_( ETimer(T::type) );
//	}
	template<>
	void start_timer(Hajime_Matte const& /*evt*/)
	{
		m_pCore->StartTimer_( eTimer_Main );
		m_pCore->ResetTimer_( eTimer_Holdi ); //FIXME!
	}
	template<>
	void start_timer( OsaekomiToketa const& evt )
	{
		//m_pCore->ResetTimer_( eTimer_Hold );
		m_pCore->StartTimer_( evt.timer );
	}
	void yoshi( OsaekomiToketa const& evt )
	{
		m_pCore->StartTimer_( eTimer_Main );
		m_pCore->StartTimer_( evt.timer );
	}

	template<class T>
	void add_point(PointEvent<T> const& evt)
	{
		if( T::revoke )
			Score_(evt.tori).Remove(EPoint(T::type));
		else
			Score_(evt.tori).Add(EPoint(T::type));
	}

	template<>
	void add_point(PointEvent<ippon_type> const& evt)
	{
		Score_(evt.tori).Add(ePoint_Ippon);
		m_pCore->StopTimer_(eTimer_Main);
		m_pCore->StopTimer_(eTimer_Holdi); //FIXME!
	}


	template<>
	void add_point(PointEvent<shido_type> const& evt)
	{
		EFighter uke = GetUkeFromTori(evt.tori);
		if( 3 == Score_(evt.tori).Shido() )
		{
			Score_(uke).Remove(ePoint_Wazaari);
			Score_(uke).Add(ePoint_Ippon);
		}
		else if( 2 == Score_(evt.tori).Shido() )
		{
			Score_(uke).Remove(ePoint_Yuko);
			Score_(uke).Add(ePoint_Wazaari);
		}
		else if( 1 == Score_(evt.tori).Shido() )
		{
			Score_(uke).Add(ePoint_Yuko);
		}
		Score_(evt.tori).Add(ePoint_Shido);
	}

	template<>
	void add_point(PointEvent<revoke_shido_hm_type> const& evt)
	{
		EFighter uke = GetUkeFromTori(evt.tori);
		if( Score_(evt.tori).Hansokumake() )
		{
			Score_(uke).Remove(ePoint_Ippon);
			Score_(evt.tori).Remove(ePoint_Hansokumake);
		}
		else
		{
			if( 4 == Score_(evt.tori).Shido() )
			{
				Score_(uke).Remove(ePoint_Ippon);
				Score_(uke).Add(ePoint_Wazaari);
			}
			else if( 3 == Score_(evt.tori).Shido() )
			{
				Score_(uke).Remove(ePoint_Wazaari);
				Score_(uke).Add(ePoint_Yuko);
			}
			else if( 2 == Score_(evt.tori).Shido() )
			{
				Score_(uke).Remove(ePoint_Yuko);
			}
			Score_(evt.tori).Remove(ePoint_Shido);
		}
	}

	template<>
	void add_point(PointEvent<hansokumake_type> const& evt)
	{
		EFighter uke = GetUkeFromTori(evt.tori);
		Score_(uke).Add(ePoint_Ippon);
		Score_(evt.tori).Add(ePoint_Hansokumake);

		m_pCore->StopTimer_(eTimer_Main);
		m_pCore->StopTimer_(eTimer_Holdi); //FIXME!
	}

	void add_point(HoldTimeEvent const& evt);

	template <class T>
	void add_point_stop_timer(T const& evt)
	{
		add_point(evt);
		m_pCore->StopTimer_( eTimer_Holdi ); //FIXME!
		m_pCore->StopTimer_( eTimer_Main );
	}

	//------------------
	// guard conditions
	//------------------
	template<class T>
	bool time_is_left(T const& /*evt*/)
	{
		return 0 != m_pCore->GetTime_(eTimer_Main);
	}
	template<class T>
	bool time_is_up(T const& /*evt*/)
	{
		return 0 == m_pCore->GetTime_(eTimer_Main);
	}
	bool has_wazaari(Wazaari const& evt);
	bool has_2wazaari(RevokeWazaari const& evt);
	bool has_no_wazaari(Wazaari const& evt);
	bool has_25s(HoldTimeEvent const& evt);
	bool has_20s(HoldTimeEvent const& evt);
	bool has_20s_and_wazaari(HoldTimeEvent const& evt);
	bool has_15s(HoldTimeEvent const& evt);
	bool is_sonomama(OsaekomiToketa const& evt);
	bool has_enough_shido(Shido const& evt);
	bool can_take_shido(Shido const& evt);

	typedef Ipponboard::IpponboardSM_ sm; // makes transition table cleaner

	// Transition table for board
	struct transition_table : boost::mpl::vector<
	//		Start		Event			Next	  Action				Guard
	//	  +---------+---------------+-----------+-------------------+-----------------------+
	a_row < Stopped , Hajime_Matte	, Running	, &sm::start_timer							>,
	  row < Stopped , Shido			, Stopped	, &sm::add_point	, &sm::has_enough_shido	>,
	  row < Stopped , Shido			, Stopped	, &sm::add_point	, &sm::can_take_shido	>,
	a_row < Stopped , Hansokumake	, Stopped	, &sm::add_point							>,
	a_row < Stopped , Reset         , Stopped	, &sm::reset								>,
	 _row < Stopped , Finish		, Stopped												>,
	  //row < Stopped , Osaekomi_Toketa, Holding	, &sm::yoshi		, &sm::is_sonomama		>,
	a_row < Stopped , OsaekomiToketa, Holding	, &sm::yoshi								>,	// JUST FOR CONVENIENCE !!!
	a_row < Stopped , RevokeShidoHM	, Stopped	, &sm::add_point							>,
	a_row < Stopped	, RevokeWazaari	, Stopped	, &sm::add_point							>,
	a_row < Stopped	, RevokeYuko	, Stopped	, &sm::add_point							>,
	a_row < Stopped , Ippon			, Stopped	, &sm::add_point							>,	// just to correct values...
	a_row < Stopped , RevokeIppon	, Stopped	, &sm::add_point							>,	// just to correct values...
	  row < Stopped , Wazaari		, Stopped	, &sm::add_point	, &sm::has_wazaari		>,	// just to correct values...
	a_row < Stopped , Wazaari		, Stopped	, &sm::add_point							>,	// just to correct values...
	a_row < Stopped , Yuko			, Stopped	, &sm::add_point							>,	// just to correct values...
	//	  +---------+---------------+-----------+-------------------+-----------------------+
	a_row < Running , Hajime_Matte	, Stopped	, &sm::stop_timer					 		>,
	a_row < Running , TimeEndedEvent, Stopped	, &sm::stop_timer							>,
	a_row < Running , Ippon			, Stopped	, &sm::add_point							>,
	a_row < Running , Wazaari		, Running	, &sm::add_point							>,
	  row < Running , Wazaari		, Stopped	, &sm::add_point_stop_timer, &sm::has_wazaari		>,
	a_row < Running , Yuko			, Running	, &sm::add_point							>,
	a_row < Running	, Reset			, Stopped	, &sm::reset								>,
	a_row < Running , Finish		, Stopped	, &sm::stop_timer							>,
	a_row < Running , OsaekomiToketa, Holding	, &sm::start_timer							>,
	a_row < Running	, RevokeWazaari	, Running	, &sm::add_point							>,
	a_row < Running	, RevokeYuko	, Running	, &sm::add_point							>,
	a_row < Running , RevokeShidoHM	, Running	, &sm::add_point							>,	// just to correct values...
	a_row < Running , Hansokumake	, Stopped	, &sm::add_point							>,	// just to correct values...
	  row < Running , Shido			, Stopped	, &sm::add_point_stop_timer	, &sm::has_enough_shido	>,	// just to correct values...
	  row < Running , Shido			, Running	, &sm::add_point			, &sm::can_take_shido	>,	// just to correct values...
	//	  +---------+---------------+-----------+-------------------+-----------------------+
	  row < Holding ,OsaekomiToketa, Running	, &sm::stop_timer			, &sm::time_is_left		>,
	  row < Holding ,OsaekomiToketa, Stopped	, &sm::stop_timer			, &sm::time_is_up		>,
	a_row < Holding ,Hajime_Matte	, Stopped	, &sm::stop_timer									>,
	a_row < Holding ,Reset			, Stopped	, &sm::reset										>,
	a_row < Holding ,Finish			, Stopped	, &sm::stop_timer									>,
	a_row < Holding ,Hansokumake	, Stopped	, &sm::add_point									>,
	a_row < Holding ,Ippon			, Stopped	, &sm::add_point									>,
	a_row < Holding ,Wazaari		, Holding	, &sm::add_point									>,	// just to correct values...
	a_row < Holding ,Yuko			, Holding	, &sm::add_point									>,	// just to correct values...
	a_row < Holding ,Shido			, Holding	, &sm::add_point									>,	// just to correct values...
	a_row < Holding ,RevokeWazaari	, Holding	, &sm::add_point									>,	// just to correct values...
	a_row < Holding ,RevokeYuko		, Holding	, &sm::add_point									>,	// just to correct values...
	a_row < Holding ,RevokeShidoHM	, Holding	, &sm::add_point									>,	// just to correct values...
	  row < Holding ,HoldTimeEvent	, Holding	, &sm::add_point			, &sm::has_15s			>,
	  row < Holding ,HoldTimeEvent	, Holding	, &sm::add_point			, &sm::has_20s			>,
	  row < Holding ,HoldTimeEvent	, Stopped	, &sm::add_point_stop_timer	, &sm::has_20s_and_wazaari>,
	  row < Holding ,HoldTimeEvent	, Stopped	, &sm::add_point_stop_timer	, &sm::has_25s			>
  //	  +---------+---------------+-----------+-------------------+----------------------+
	> {};
	// Replaces the default no-transition response.
	template <class FSM, class Event>
	void no_transition(Event const&, FSM&, int /*state*/)
	{
//		e;
//		std::cout << "no transition from state " << state
//			<< " on event " << typeid(e).name() << std::endl;
	}

private:
	inline Score& Score_(EFighter who)
		{ return m_pCore->GetScore_(who); }
	inline const Score& Score_(EFighter who) const
		{ return m_pCore->GetScore_(who); }

	IControllerCore* m_pCore;
};

typedef msm::back::state_machine<IpponboardSM_> IpponboardSM;

} // namespace Ipponboard

#endif // STATEMACHINE_H
