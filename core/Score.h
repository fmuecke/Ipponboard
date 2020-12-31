// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__SCORE_H_
#define BASE__SCORE_H_

#include "Enums.h"

#include <memory>

namespace Ipponboard
{
class AbstractRules;

class Score
{
public:

	enum class Point
	{
		Ippon = 0,
		Wazaari,
		Yuko,
		Shido,
		Hansokumake,
		_MAX
	};

	Score()
	{
		Clear();
	}

	Score& Add(Point point);
	Score& Remove(Point point);
	Score& SetValue(Point point, int value);
	int Value(Point point) const { return _points[static_cast<int>(point)]; }

	// convenience functions
	bool Ippon() const { return Value(Point::Ippon) != 0; }
	int Wazaari() const { return Value(Point::Wazaari); }
	int Yuko() const { return Value(Point::Yuko); }
	int Shido() const { return Value(Point::Shido); }
	bool Hansokumake() const { return Value(Point::Hansokumake) != 0; }
	void Clear();

protected:
	virtual void correct_point(Point p);

private:
	void correct_points();

	int _points[static_cast<int>(Point::_MAX)];
};
} // namespace ipponboard
#endif  // BASE__SCORE_H_
