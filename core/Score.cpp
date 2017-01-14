#include "Score.h"
#include "Rules.h"

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
void Score::Clear()
//=========================================================
{
	for (auto i = 0; i < static_cast<int>(Point::_MAX); ++i)
	{
		_points[i] = 0;
    }
}

void Score::correct_point(Score::Point p)
{
    switch(p)
    {
    case Point::Ippon:
        if (Value(Point::Ippon) > 1)
            _points[static_cast<int>(Point::Ippon)] = 1;
        if (Value(Point::Ippon) < 0)
            _points[static_cast<int>(Point::Ippon)] = 0;
        break;

    case Point::Wazaari:
        if (Value(Point::Wazaari) > 2)
            _points[static_cast<int>(Point::Wazaari)] = 2;
        if (Value(Point::Wazaari) < 0)
            _points[static_cast<int>(Point::Wazaari)] = 0;
        break;

    case Point::Yuko:
        if (Value(Point::Yuko) < 0)
            _points[static_cast<int>(Point::Yuko)] = 0;
        break;

    case Point::Shido:
        if (Value(Point::Shido) > 4)
            _points[static_cast<int>(Point::Shido)] = 4;
        if (Value(Point::Shido) < 0)
            _points[static_cast<int>(Point::Shido)] = 0;
        break;

    case Point::Hansokumake:
        if (Value(Point::Hansokumake) > 1)
            _points[static_cast<int>(Point::Hansokumake)] = 1;
        if (Value(Point::Hansokumake) < 0)
            _points[static_cast<int>(Point::Hansokumake)] = 0;
        break;

      default:
        break;
    }
}


void Score::correct_points()
{
    correct_point(Point::Ippon);
    correct_point(Point::Wazaari);
    correct_point(Point::Yuko);
    correct_point(Point::Hansokumake);
    correct_point(Point::Shido);
}
