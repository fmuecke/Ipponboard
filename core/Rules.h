#ifndef RULESET_H
#define RULESET_H

#include "Score.h"

namespace Ipponboard
{

class AbstractRules
{
public:
    AbstractRules();

    virtual void SetAutoIncementPoints(bool autoIncrement)
    {
        _isAutoIncementPoints = autoIncrement;
    }

    virtual void SetCountSubscores(bool countSubscores)
    {
        _isCountSubscores = countSubscores;
    }

    virtual bool IsAutoIncrementPoints() const
    {
        return _isAutoIncementPoints;
    }

    virtual bool IsCountSubscores() const
    {
        return _isCountSubscores;
    }

    virtual int GetOsaekomiValue(Ipponboard::Score::Point p) const = 0;

private:
    bool _isAutoIncementPoints { false };
    bool _isCountSubscores { false };
};



class ClassicRules : public AbstractRules
{
public:
    ClassicRules();

    virtual int GetOsaekomiValue(Score::Point p) const final
    {
       switch(p)
       {
        case Score::Point::Ippon: return 25;
        case Score::Point::Wazaari: return 20;
        case Score::Point::Yuko: return 15;
        default: return -1;
       }
    }
};

class Rules2013 : public AbstractRules
{
public:
    Rules2013();

    virtual int GetOsaekomiValue(Score::Point p) const final
    {
       switch(p)
       {
        case Score::Point::Ippon: return 20;
        case Score::Point::Wazaari: return 15;
        case Score::Point::Yuko: return 10;
        default: return -1;
       }
    }
};

class Rules2017 : public AbstractRules
{
public:
    Rules2017();

    virtual int GetOsaekomiValue(Score::Point p) const final
    {
       switch(p)
       {
        case Score::Point::Ippon: return 20;
        case Score::Point::Wazaari: return 10;
        default: return -1;
       }
    }
};


}

#endif // RULESET_H
