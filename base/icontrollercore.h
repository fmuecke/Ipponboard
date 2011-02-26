#ifndef I_SCORE_CONTROLLER_CORE_INCLUDED
#define I_SCORE_CONTROLLER_CORE_INCLUDED

#include <QString>
#include "enums.h"
#include "score.h"


namespace Ipponboard
{

// forwards
class IView;

class IControllerCore
{

	friend class IpponboardSM_;

private:
	virtual void start_timer( ETimer ) = 0;
	virtual void stop_timer( ETimer ) = 0;
	virtual void reset_fight() = 0;
	virtual void reset_timer( ETimer ) = 0;
	virtual Score& get_score( Ipponboard::EFighter who ) = 0;
	virtual const Score& get_score( Ipponboard::EFighter who ) const = 0;
	virtual const int get_time( ETimer ) const = 0;
	virtual bool is_sonomama() const = 0;
};

}
#endif // I_SCORE_CONTROLLER_INCLUDED
