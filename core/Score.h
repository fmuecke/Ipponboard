#ifndef BASE__SCORE_H_
#define BASE__SCORE_H_

#include "Enums.h"

namespace Ipponboard
{

class Score
{
public:

	//// for testing purpose
	//Score(int ippon, int wazaari, int yuko, int hansokumake, int shido)
	//{
	//	_points[ePoint_Hansokumake] = hansokumake;
	//	_points[ePoint_Shido] = shido;
	//	_points[ePoint_Ippon] = ippon;
	//	_points[ePoint_Wazaari] = wazaari;
	//	_points[ePoint_Yuko] = yuko;
	//}

	Score()
	{
		Clear();
	}

	Score& Add(Ipponboard::EPoint point);
	Score& Remove(Ipponboard::EPoint point);
	Score& OverwriteValue(Ipponboard::EPoint point, int value);

	// convenience functions
	bool Ippon() const { return _points[ePoint_Ippon] != 0; }
	int Wazaari() const { return _points[ePoint_Wazaari]; }
	int Yuko() const { return _points[ePoint_Yuko]; }
	int Shido() const { return _points[ePoint_Shido]; }
	bool Hansokumake() const  { return _points[ePoint_Hansokumake] != 0; }

	bool IsAwaseteIppon() const;
	void Clear();

    bool IsLess(const Score& rhs, RuleSet ruleSet) const
	{
		if (_points[ePoint_Hansokumake] != rhs._points[ePoint_Hansokumake])
		{
			return _points[ePoint_Hansokumake] > rhs._points[ePoint_Hansokumake];
		}
		else
		{
			if (_points[ePoint_Ippon] != rhs._points[ePoint_Ippon])
			{
				return _points[ePoint_Ippon] < rhs._points[ePoint_Ippon];
			}
			else
			{
				if (_points[ePoint_Wazaari] != rhs._points[ePoint_Wazaari])
				{
					return _points[ePoint_Wazaari] < rhs._points[ePoint_Wazaari];
				}
				else
				{
					if (_points[ePoint_Yuko] != rhs._points[ePoint_Yuko])
					{
						return _points[ePoint_Yuko] < rhs._points[ePoint_Yuko];
					}
					else
					{
						if (e2013RuleSet == ruleSet)
						{
							return _points[ePoint_Shido] > rhs._points[ePoint_Shido];
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

private:
	void correct_points();

	int _points[ePoint_MAX];
};

} // namespace ipponboard
#endif  // BASE__SCORE_H_
