#ifndef RULESET_H
#define RULESET_H

#include "Score.h"
#include <QString>

namespace Ipponboard
{

class AbstractRules
{
public:
    AbstractRules();

    virtual const char* Name() const = 0;

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

    virtual bool IsAwaseteIppon(Score const& s) const
    {
        return s.Wazaari() == GetMaxWazaariCount();
    }

    virtual bool IsShidosCountAsPoints() const { return false; }
    virtual bool HasUnlimitedGoldenScore() const { return true; }
    virtual bool HasYukoSupport() const { return true; }
    virtual bool IsScoreLess(const Score& lhs, const Score& rhs) const;
    virtual int GetMaxShidoCount() const { return 3; }
    virtual int GetMaxWazaariCount() const { return 2; }
    virtual int GetOsaekomiValue(Ipponboard::Score::Point p) const = 0;

    template<typename T>
    bool IsOfType() const { return dynamic_cast<const T*>(this) != nullptr; }

private:
    bool _isAutoIncementPoints { false };
    bool _isCountSubscores { false };
};


class ClassicRules : public AbstractRules
{
public:
    ClassicRules();

    virtual const char* Name() const final
    {
        return "Classic";
    }

    virtual bool HasUnlimitedGoldenScore() const final
    {
        return false;
    }

    virtual bool IsShidosCountAsPoints() const final
    {
        return true;
    }

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

    virtual const char* Name() const final
    {
        return "IJF-2013";
    }

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

    virtual const char* Name() const final
    {
        return "IJF-2017";
    }

    virtual bool HasYukoSupport() const final
    {
        return false;
    }

    virtual bool IsAwaseteIppon(Score const&) const final
    {
        return false;
    }

    virtual int GetMaxShidoCount() const final
    {
        return 2;
    }

    virtual int GetOsaekomiValue(Score::Point p) const final
    {
       switch(p)
       {
        case Score::Point::Ippon: return 20;
        case Score::Point::Wazaari: return 10;
        default: return -1;
       }
    }

    virtual int GetMaxWazaariCount() const final
    {
        return INT32_MAX;
    }
};

class RulesFactory
{
public:
    static std::shared_ptr<AbstractRules> Create(QString name)
    {
        if (name == ClassicRules().Name())
        {
            return std::make_shared<ClassicRules>();
        }

        if (name == Rules2013().Name())
        {
            return std::make_shared<Rules2013>();
        }

        // default
        return std::make_shared<Rules2017>();
    }
};

} // namespace
#endif // RULESET_H
