#ifndef BASE__SCORE_H_
#define BASE__SCORE_H_

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

	bool operator<(const Score& rhs) const
	{
		if( _points[ePoint_Hansokumake] < rhs._points[ePoint_Hansokumake] )
			return true;

		if( _points[ePoint_Ippon] < rhs._points[ePoint_Ippon] )
			return true;

		if( _points[ePoint_Wazaari] < rhs._points[ePoint_Wazaari] )
			return true;

		if( _points[ePoint_Yuko] < rhs._points[ePoint_Yuko] )
			return true;

		// Note: shidos are not compared as they result in
		// concrete points
		//
		return false;
	}

private:
	int _points[ePoint_MAX];
};

} // namespace ipponboard
#endif	// BASE__SCORE_H_
