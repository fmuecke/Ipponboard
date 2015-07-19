#pragma once

#include "../core/Score.h"
#include "../core/Enums.h"
#include "../core/Fight.h"

using Ipponboard::Fight;
using Ipponboard::Score;


TEST_CASE("Fighter with less Shidos wins if points are equal")
{
	auto score = Score().Add(ePoint_Yuko).Add(ePoint_Yuko);
	auto scoreWithShido = Score(score).Add(ePoint_Shido);
	auto scoreWithThreeShido = Score(score).Add(ePoint_Shido).Add(ePoint_Shido).Add(ePoint_Shido);

	Fight fight;
	fight.ruleSet = e2013RuleSet;

	fight.scores[0] = score;
	fight.scores[1] = score;
	REQUIRE_FALSE(fight.HasWon(eFighter2));
	REQUIRE_FALSE(fight.HasWon(eFighter1));

	fight.scores[0] = scoreWithShido;
	fight.scores[1] = scoreWithShido;
	REQUIRE_FALSE(fight.HasWon(eFighter2));
	REQUIRE_FALSE(fight.HasWon(eFighter1));

	fight.scores[0] = scoreWithThreeShido;
	fight.scores[1] = scoreWithThreeShido;
	REQUIRE_FALSE(fight.HasWon(eFighter2));
	REQUIRE_FALSE(fight.HasWon(eFighter1));

	fight.scores[0] = score;
	fight.scores[1] = scoreWithThreeShido;
	REQUIRE_FALSE(fight.HasWon(eFighter2));
	REQUIRE(fight.HasWon(eFighter1));

	fight.scores[0] = scoreWithShido;
	fight.scores[1] = scoreWithThreeShido;
	REQUIRE_FALSE(fight.HasWon(eFighter2));
	REQUIRE(fight.HasWon(eFighter1));
}
