#ifndef BASE__ICONTROLLER_H_
#define BASE__ICONTROLLER_H_

#include <QString>
#include "Enums.h"
#include "Score.h"


namespace Ipponboard
{

// forwards
class IView;
class IGoldenScoreView;
class AbstractRules;

class IController
{
public:
	virtual void RegisterView(IView* pView) = 0;
    virtual void RegisterView(IGoldenScoreView* pView) = 0;
	virtual int GetScore(Ipponboard::FighterEnum whos, Ipponboard::Score::Point point) const = 0;
	virtual void DoAction(Ipponboard::EAction action, Ipponboard::FighterEnum who = Ipponboard::FighterEnum::First, bool doRevoke = false) = 0;
	virtual Ipponboard::EState GetCurrentState() const	= 0;
	virtual Ipponboard::FighterEnum GetLead() const = 0;
	virtual Ipponboard::FighterEnum GetLastHolder() const = 0;
	virtual QString GetTimeText(Ipponboard::ETimer timer) const = 0;
	virtual QString GetFighterName(Ipponboard::FighterEnum) const = 0;
	virtual QString GetFighterLastName(Ipponboard::FighterEnum) const = 0;
	virtual QString GetFighterFirstName(Ipponboard::FighterEnum) const = 0;
	virtual QString GetFighterClub(Ipponboard::FighterEnum) const = 0;
	virtual QString const& GetWeight() const = 0;
	virtual QString GetMessage() const = 0;
	virtual int GetTeamScore(Ipponboard::FighterEnum) const = 0;
	virtual void SetTimerValue(Ipponboard::ETimer timer, const QString& value) = 0;
	virtual void SetFightTime(const QString& value) = 0;
	//FIXME: virtual int GetRound() const = 0;
	virtual void SetWeightClass(QString const& c) = 0;
	virtual QString const& GetCategoryName() const = 0;
	virtual void SetGoldenScore(bool isGS) = 0;
	virtual bool IsGoldenScore() const = 0;
    virtual std::shared_ptr<AbstractRules> GetRules() const = 0;
    virtual void SetRules(std::shared_ptr<AbstractRules> rules) = 0;
    virtual bool IsAutoAdjustPoints() const = 0;
    virtual void SetAutoAdjustPoints(bool isActive) = 0;
	virtual void SetOption(Ipponboard::EOption option, bool isSet) = 0;
	virtual bool GetOption(Ipponboard::EOption option) const = 0;
	virtual QString GetHomeLabel() const = 0;
	virtual QString GetGuestLabel() const = 0;
	virtual void SetLabels(QString const& home, QString const& guest) = 0;
};

}
#endif  // BASE__ICONTROLLER_H_
