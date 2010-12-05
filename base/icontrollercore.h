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
	virtual void StartTimer_( ETimer ) = 0;
	virtual void StopTimer_( ETimer ) = 0;
	virtual void ResetFight_() = 0;
	virtual void ResetTimer_( ETimer ) = 0;
	virtual Score& GetScore_( Ipponboard::EFighter who ) = 0;
	virtual const Score& GetScore_( Ipponboard::EFighter who ) const = 0;
	virtual const int GetTime_( ETimer ) const = 0;
	virtual bool IsSonoMama_() const = 0;
};

}
#endif // I_SCORE_CONTROLLER_INCLUDED
