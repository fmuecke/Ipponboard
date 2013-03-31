#include "Score.h"

using namespace Ipponboard;

//=========================================================
void Score::Add(EPoint point)
//=========================================================
{
	++_points[point];

	correct_points();
}

//=========================================================
void Score::Remove(EPoint point)
//=========================================================
{
	--_points[point];

	correct_points();
}

//=========================================================
void Score::OverwriteValue(Ipponboard::EPoint point, int value)
//=========================================================
{
	if (value >= 0)
	{
		_points[point] = value;

		// correct points
		if (_points[ePoint_Ippon] > 1)
			_points[ePoint_Ippon] = 1;

		if (_points[ePoint_Wazaari] > 2)
			_points[ePoint_Wazaari] = 2;

		if (_points[ePoint_Shido] > 4)
			_points[ePoint_Shido] = 4;

		if (_points[ePoint_Hansokumake] > 1)
			_points[ePoint_Hansokumake] = 1;
	}
}

//=========================================================
bool Score::IsAwaseteIppon() const
//=========================================================
{
	return _points[ePoint_Wazaari] == 2;
}

//=========================================================
void Score::Clear()
//=========================================================
{
	_points[ePoint_Ippon      ] = 0;
	_points[ePoint_Wazaari    ] = 0;
	_points[ePoint_Yuko       ] = 0;
	_points[ePoint_Shido      ] = 0;
	_points[ePoint_Hansokumake] = 0;
}


void Score::correct_points()
{
	// validate upper bound
	if (_points[ePoint_Ippon] > 1)
		_points[ePoint_Ippon] = 1;

	if (_points[ePoint_Wazaari] > 2)
		_points[ePoint_Wazaari] = 2;

	if (_points[ePoint_Shido] > 4)
		_points[ePoint_Shido] = 4;

	if (_points[ePoint_Hansokumake] > 1)
		_points[ePoint_Hansokumake] = 1;

	// validate lower bound
	if (_points[ePoint_Ippon] < 0)
		_points[ePoint_Ippon] = 0;

	if (_points[ePoint_Wazaari] < 0)
		_points[ePoint_Wazaari] = 0;

	if (_points[ePoint_Yuko] < 0)
		_points[ePoint_Yuko] = 0;

	if (_points[ePoint_Shido] < 0)
		_points[ePoint_Shido] = 0;

	if (_points[ePoint_Hansokumake] < 0)
		_points[ePoint_Hansokumake] = 0;
}