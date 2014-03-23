#ifndef BASE__ICONTROLLERCORE_H_
#define BASE__ICONTROLLERCORE_H_

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
	virtual void start_timer(ETimer) = 0;
	virtual void stop_timer(ETimer) = 0;
	virtual void reset_fight() = 0;
	virtual void reset_timer(ETimer) = 0;
	virtual Score& get_score(Ipponboard::EFighter who) = 0;
	virtual const Score& get_score(Ipponboard::EFighter who) const = 0;
	virtual int get_time(ETimer) const = 0;
	virtual bool is_sonomama() const = 0;
	virtual bool is_golden_score() const = 0;
	virtual bool is_option(Ipponboard::EOption option) const = 0;
};

}
#endif  // BASE__ICONTROLLERCORE_H_
