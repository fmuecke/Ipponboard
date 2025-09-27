// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__ICONTROLLERCORE_H_
#define BASE__ICONTROLLERCORE_H_

#include <QString>
#include "Enums.h"
#include "Score.h"

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
	virtual void save_fight() = 0;
	virtual void reset_fight() = 0;
	virtual void reset_timer(ETimer) = 0;
	virtual Score& get_score(Ipponboard::FighterEnum who) = 0;
	virtual Score const& get_score(Ipponboard::FighterEnum who) const = 0;
	virtual int get_time(ETimer) const = 0;
	virtual bool is_sonomama() const = 0;
	virtual bool is_golden_score() const = 0;
	virtual bool is_option(Ipponboard::EOption option) const = 0;
	virtual bool is_auto_adjust() const = 0;
	virtual std::shared_ptr<AbstractRules> GetRules() const = 0;
};

} // namespace Ipponboard
#endif // BASE__ICONTROLLERCORE_H_
