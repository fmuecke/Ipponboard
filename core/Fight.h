// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#ifndef __CORE_FIGHT_H
#define __CORE_FIGHT_H

#include "Score.h"
#include "Enums.h"
#include "Rules.h"

#include "QString"
#include <memory>

namespace Ipponboard
{

struct SimpleFighter
{
	QString name;
	QString club;
};

class Fight
{
private:
	enum
	{
		eScore_Ippon = 10,
		eScore_Wazaari = 7,
		eScore_Yuko = 5,
		eScore_Hantai = 1,
		eScore_Shido = 1,
		eScore_Hikewake = 0,
		eScore_Lost = 0
	};

public:
    Fight();

	Score const& GetScore(FighterEnum fighter) const
	{ 
		return scores[static_cast<int>(fighter)]; 
	}

	Score& GetScore(FighterEnum fighter)
	{
		return scores[static_cast<int>(fighter)];
	}

	SimpleFighter const& GetFighter(FighterEnum fighter) const
	{
		return fighters[static_cast<int>(fighter)];
	}

	SimpleFighter GetFighter(FighterEnum fighter)
	{
		return fighters[static_cast<int>(fighter)];
	}

	Score scores[2];
	SimpleFighter fighters[2];
	QString weight;
	int time_in_seconds;
	int max_time_in_seconds;
	bool is_saved;
    std::shared_ptr<AbstractRules> ruleSet;

    QString GetTimeFaught() const;
    QString GetTimeRemaining() const;
    bool HasWon(FighterEnum who) const;
    int ScorePoints(FighterEnum who) const;
};

} // namespace Ipponboard


#endif // __CORE_FIGHT_H
