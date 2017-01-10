#ifndef BASE__SCORE_H_
#define BASE__SCORE_H_

#include "Enums.h"

namespace Ipponboard
{

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
	bool Hansokumake() const  { return Value(Point::Hansokumake) != 0; }

	bool IsAwaseteIppon() const;
	void Clear();

    bool IsLess(const Score& rhs, RuleSet ruleSet) const
	{
		if (Value(Point::Hansokumake) != rhs.Value(Point::Hansokumake))
		{
			return Value(Point::Hansokumake) > rhs.Value(Point::Hansokumake);
		}
		else
		{
			if (Value(Point::Ippon) != rhs.Value(Point::Ippon))
			{
				return Value(Point::Ippon) < rhs.Value(Point::Ippon);
			}
			else
			{
				if (Value(Point::Wazaari) != rhs.Value(Point::Wazaari))
				{
					return Value(Point::Wazaari) < rhs.Value(Point::Wazaari);
				}
				else
				{
					if (Value(Point::Yuko) != rhs.Value(Point::Yuko))
					{
						return Value(Point::Yuko) < rhs.Value(Point::Yuko);
					}
					else
					{
						if (e2013RuleSet == ruleSet)
						{
							return Value(Point::Shido) > rhs.Value(Point::Shido);
						}
						else
						{
							// shidos are not compared as they result in
							// concrete points
						}
					}
				}
			}
		}

        return false;
	}

protected:
    virtual void correct_point(Point p);

private:
	void correct_points();

	int _points[static_cast<int>(Point::_MAX)];
};

} // namespace ipponboard
#endif  // BASE__SCORE_H_
