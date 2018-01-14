#include "Rules.h"
#include "Fight.h"

using namespace Ipponboard;

const char* const Rules2018::StaticName = "IJF-2018";
const char* const Rules2017::StaticName = "IJF-2017";
const char* const Rules2017U15::StaticName = "IJF-2017 U15";
const char* const Rules2013::StaticName = "IJF-2013";
const char* const ClassicRules::StaticName = "Classic";

AbstractRules::AbstractRules()
{}

int Ipponboard::AbstractRules::CompareScore(const Fight& f) const
{
	using Point = Score::Point;

	auto lhs = f.GetScore(FighterEnum::First);
	auto rhs = f.GetScore(FighterEnum::Second);

	if (lhs.Value(Point::Hansokumake) < rhs.Value(Point::Hansokumake))
	{
		return -1;
	}

	if ((lhs.Value(Point::Hansokumake) > rhs.Value(Point::Hansokumake)))
	{
		return 1;
	}

	if (lhs.Value(Point::Ippon) > rhs.Value(Point::Ippon))
	{
		return -1;
	}

	if (lhs.Value(Point::Ippon) < rhs.Value(Point::Ippon))
	{
		return 1;
	}

	if (lhs.Value(Point::Wazaari) > rhs.Value(Point::Wazaari))
	{
		return -1;
	}

	if (lhs.Value(Point::Wazaari) < rhs.Value(Point::Wazaari))
	{
		return 1;
	}

	if (lhs.Value(Point::Yuko) > rhs.Value(Point::Yuko))
	{
		return -1;
	}

	if (lhs.Value(Point::Yuko) < rhs.Value(Point::Yuko))
	{
		return 1;
	}

	// shidos are not compared as they result in concrete points
	if (!IsOption_ShidoAddsPoint() && (IsOption_ShidoScoreCounts() || f.IsGoldenScore()))
	{
		if (lhs.Value(Point::Shido) < rhs.Value(Point::Shido))
		{
			return -1;
		}

		if (lhs.Value(Point::Shido) > rhs.Value(Point::Shido))
		{
			return 1;
		}
	}

	return 0;
}
