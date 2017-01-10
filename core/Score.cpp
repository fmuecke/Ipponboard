#include "Score.h"

using namespace Ipponboard;

//=========================================================
Score& Score::Add(Point point)
//=========================================================
{
	++_points[static_cast<int>(point)];

	correct_points();

	return *this;
}

//=========================================================
Score& Score::Remove(Point point)
//=========================================================
{
	--_points[static_cast<int>(point)];

	correct_points();

	return *this;
}

//=========================================================
Score& Score::SetValue(Ipponboard::Score::Point point, int value)
//=========================================================
{
	if (value >= 0)
	{
		_points[static_cast<int>(point)] = value;

		correct_points();
	}

	return *this;
}

//=========================================================
bool Score::IsAwaseteIppon() const
//=========================================================
{
	return Value(Point::Wazaari) == 2;
}

//=========================================================
void Score::Clear()
//=========================================================
{
	for (auto i = 0; i < static_cast<int>(Point::_MAX); ++i)
	{
		_points[i] = 0;
	}
}


void Score::correct_points()
{
	// validate upper bound
	if (Value(Point::Ippon) > 1)
		_points[static_cast<int>(Point::Ippon)] = 1;

	if (Value(Point::Wazaari) > 2)
		_points[static_cast<int>(Point::Wazaari)] = 2;

	if (Value(Point::Shido) > 4)
		_points[static_cast<int>(Point::Shido)] = 4;

	if (Value(Point::Hansokumake) > 1)
		_points[static_cast<int>(Point::Hansokumake)] = 1;

	// validate lower bound
	if (Value(Point::Ippon) < 0)
		_points[static_cast<int>(Point::Ippon)] = 0;

	if (Value(Point::Wazaari) < 0)
		_points[static_cast<int>(Point::Wazaari)] = 0;

	if (Value(Point::Yuko) < 0)
		_points[static_cast<int>(Point::Yuko)] = 0;

	if (Value(Point::Shido) < 0)
		_points[static_cast<int>(Point::Shido)] = 0;

	if (Value(Point::Hansokumake) < 0)
		_points[static_cast<int>(Point::Hansokumake)] = 0;
}