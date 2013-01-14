#ifndef BASE__ICONTROLLER_H_
#define BASE__ICONTROLLER_H_

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
	virtual void RegisterView(IView* pView) = 0;
	virtual int GetScore(Ipponboard::EFighter whos, Ipponboard::EPoint point) const = 0;
	virtual void DoAction(Ipponboard::EAction action, Ipponboard::EFighter who = Ipponboard::eFighter1, bool doRevoke = false) = 0;
	virtual Ipponboard::EState GetCurrentState() const	= 0;
	virtual Ipponboard::EFighter GetLead() const = 0;
	virtual Ipponboard::EFighter GetLastHolder() const = 0;
	virtual const QString GetTimeText(Ipponboard::ETimer timer) const = 0;
	virtual const QString GetFighterName(Ipponboard::EFighter) const = 0;
	virtual const QString GetFighterLastName(Ipponboard::EFighter) const = 0;
	virtual const QString GetFighterFirstName(Ipponboard::EFighter) const = 0;
	virtual const QString GetFighterClub(Ipponboard::EFighter) const = 0;
	virtual const QString& GetWeight() const = 0;
	virtual const QString GetMessage() const = 0;
	virtual const int GetTeamScore(Ipponboard::EFighter) const = 0;
	virtual void SetTimerValue(Ipponboard::ETimer timer, const QString& value) = 0;
	virtual void SetRoundTime(const QString& value) = 0;
	virtual int GetRound() const = 0;
	virtual void SetWeightClass(QString const& c) = 0;
	virtual QString const& GetCategoryName() const = 0;
	virtual void SetGoldenScore(bool isGS) = 0;
	virtual bool IsGoldenScore() const = 0;
    virtual void SetOption(Ipponboard::EOption option, bool isSet) = 0;
};

}
#endif  // BASE__ICONTROLLER_H_
