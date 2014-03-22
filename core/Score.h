#ifndef BASE__SCORE_H_
#define BASE__SCORE_H_

#include "Enums.h"

namespace Ipponboard
{

class Score
{
public:
    static void Test_Compare()
    {
        if (!(Score(0, 0, 0, 0, 1).IsLess(Score(0, 0, 0, 0, 0), e2013RuleSet)))
            throw 1;

        if ( (Score(0, 0, 0, 0, 1).IsLess(Score(0, 0, 0, 0, 1), e2013RuleSet)))
            throw 2;

        if (!(Score(0, 0, 0, 0, 2).IsLess(Score(0, 0, 0, 0, 1), e2013RuleSet)))
            throw 3;

        if (!(Score(0, 0, 0, 0, 1).IsLess(Score(0, 0, 1, 0, 2), e2013RuleSet)))
            throw 4;

        if (!(Score(0, 0, 0, 0, 2).IsLess(Score(0, 0, 1, 0, 2), e2013RuleSet)))
            throw 5;

        if (!(Score(0, 0, 0, 0, 0).IsLess(Score(0, 0, 1, 0, 2), e2013RuleSet)))
            throw 6;
		
		// classic rules
        if ( (Score(0, 0, 0, 0, 1).IsLess(Score(0, 0, 0, 0, 0), eClassicRules)))
            throw 1;

        if ( (Score(0, 0, 0, 0, 1).IsLess(Score(0, 0, 0, 0, 1), eClassicRules)))
            throw 2;

        if (!(Score(0, 0, 0, 0, 2).IsLess(Score(0, 0, 1, 0, 1), eClassicRules)))
            throw 3;

        if (!(Score(0, 0, 0, 0, 1).IsLess(Score(0, 0, 1, 0, 2), eClassicRules)))
            throw 4;

        if (!(Score(0, 0, 0, 0, 2).IsLess(Score(0, 0, 1, 0, 2), eClassicRules)))
            throw 5;

        if (!(Score(0, 0, 0, 0, 0).IsLess(Score(0, 0, 1, 0, 2), eClassicRules)))
            throw 6;
		
        //TODO: further tests
    }

    Score()
	{
		Clear();
	}

	void Add(Ipponboard::EPoint point);
	void Remove(Ipponboard::EPoint point);
	void OverwriteValue(Ipponboard::EPoint point, int value);

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
    // for testing only
    Score(int ippon, int wazaari, int yuko, int hansokumake, int shido)
    {
        _points[ePoint_Hansokumake] = hansokumake;
        _points[ePoint_Shido] = shido;
        _points[ePoint_Ippon] = ippon;
        _points[ePoint_Wazaari] = wazaari;
        _points[ePoint_Yuko] = yuko;
    }

	void correct_points();

	int _points[ePoint_MAX];
};

} // namespace ipponboard
#endif  // BASE__SCORE_H_
