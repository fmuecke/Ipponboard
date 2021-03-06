﻿// Copyright 2013 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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

	bool _isGoldenScore { false };

public:
	Fight();

	Fight(Score const& first, Score const& second) : scores { first, second }
	{}

	Score const& GetScore1() const
	{
		return scores[0];
	}

	Score& GetScore1()
	{
		return scores[0];
	}

	Score const& GetScore2() const
	{
		return scores[1];
	}

	Score& GetScore2()
	{
		return scores[1];
	}

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

	bool IsGoldenScore() const { return _isGoldenScore; }
	void SetGoldenScore(bool val) { _isGoldenScore = val; }

	int GetSecondsElapsed() const;
	void SetSecondsElapsed(int s);
	bool SetElapsedFromTotalTime(QString s);
	int GetRoundSeconds() const;
	void SetRoundTime(int secs);
	QString GetTotalTimeElapsedString() const;
	QString GetTimeRemainingString() const;

private:
	Score scores[2] { Score(), Score() };
	int seconds_elapsed { 0 };
	int round_time_seconds { 0 };

public:
	SimpleFighter fighters[2];
	QString weight;
	bool is_saved { false };
	std::shared_ptr<AbstractRules> rules; // TODO: this should be removed if possible

	// returns remaining seconds
	int GetRemainingTime() const;
	int GetGoldenScoreTime() const;
	bool HasWon(FighterEnum who) const;

	int GetScorePoints(FighterEnum who) const;
};
} // namespace Ipponboard

#endif // __CORE_FIGHT_H
