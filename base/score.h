#ifndef SCORE_H
#define SCORE_H

#include "enums.h"
namespace Ipponboard
{


class Score
{
public:

	Score()
	{
		Clear();
	}

	void Add( Ipponboard::EPoint point );
	void Remove( Ipponboard::EPoint point );
	int Get( Ipponboard::EPoint point ) const;

	// convenience functions
	bool Ippon() const { return _points[ePoint_Ippon] != 0; }
	int Wazaari() const { return _points[ePoint_Wazaari]; }
	int Yuko() const { return _points[ePoint_Yuko]; }
	int Shido() const { return _points[ePoint_Shido]; }
	bool Hansokumake() const  { return _points[ePoint_Hansokumake] != 0; }

	bool IsAwaseteIppon() const;
	void Clear();

private:
	int _points[ePoint_MAX];
};

} // namespace ipponboard
#endif // SCORE_H
