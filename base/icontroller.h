#ifndef I_SCORE_CONTROLLER_INCLUDED
#define I_SCORE_CONTROLLER_INCLUDED

#include <QString>
#include "enums.h"
#include "score.h"


namespace Ipponboard
{

// forwards
class IView;

class IController
{
public:
	enum EOsaekomiVals
	{
		eOsaekomiVal_Yuko = 15,
		eOsaekomiVal_Wazaari = 20,
		eOsaekomiVal_Ippon = 25
	};

	virtual void RegisterView( IView* pView ) = 0;
	virtual int GetScore( Ipponboard::EFighter whos, Ipponboard::EPoint point ) const = 0;
	virtual void DoAction( Ipponboard::EAction action, Ipponboard::EFighter who = Ipponboard::eFighter_Blue, bool doRevoke = false ) = 0;
	virtual Ipponboard::EState GetCurrentState() const	= 0;
	virtual Ipponboard::EFighter GetLead() const = 0;
    virtual Ipponboard::EFighter GetLastHolder() const = 0;
	virtual const QString GetTimeText( Ipponboard::ETimer timer ) const = 0;
	virtual const QString GetFighterName( Ipponboard::EFighter ) const = 0;
	virtual const QString GetFighterLastName( Ipponboard::EFighter ) const = 0;
	virtual const QString GetFighterFirstName( Ipponboard::EFighter ) const = 0;
	virtual const QString GetFighterClub( Ipponboard::EFighter ) const = 0;
	virtual const QString& GetWeight() const = 0;
	virtual const QString GetMessage() const = 0;
	virtual const int GetTeamScore( Ipponboard::EFighter ) const = 0;
	virtual void SetTimerValue( Ipponboard::ETimer timer, const QString& value ) = 0;
	virtual void ResetTimerValue( Ipponboard::ETimer timer ) = 0;
	virtual void SetRoundTime( const QString& value ) = 0;
	virtual int GetRound() const = 0;
	virtual void SetWeightClass(QString const& c) = 0;
	virtual QString const& GetWeightClass() const = 0;
};

}
#endif // I_SCORE_CONTROLLER_INCLUDED
