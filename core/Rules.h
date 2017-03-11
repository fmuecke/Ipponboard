#ifndef RULESET_H
#define RULESET_H

#include "Score.h"
#include <QString>
#include <QStringList>
#include <memory>

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

    virtual bool IsOption_AutoIncrementPoints() const
    {
        return _isAutoIncementPoints;
    }

    virtual bool IsOption_CountSubscores() const
    {
        return _isCountSubscores;
    }

    virtual bool IsAwaseteIppon(Score const& s) const
    {
        return IsOption_AwaseteIppon() && s.Wazaari() == GetMaxWazaariCount();
    }

    // second shido will result in yuko beeing added, 3rd will give waza-ari
    virtual bool IsOption_ShidoAddsPoint() const { return false; }
    virtual bool IsOption_ShidoScoreCounts() const { return true; }
    virtual bool IsOption_AwaseteIppon() const { return true; }
    virtual bool IsOption_HasYuko() const { return true; }
    virtual bool IsOption_OpenEndGoldenScore() const { return true; }

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
    ClassicRules() {}

    static const char* const StaticName;
    virtual const char* Name() const final { return StaticName; }
    virtual bool IsOption_OpenEndGoldenScore() const final { return false; }
    virtual bool IsOption_ShidoAddsPoint() const final { return true; }

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
    Rules2013() {}

    static const char* const StaticName;

    virtual const char* Name() const final { return StaticName; }

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
    Rules2017() {}

    static const char* const StaticName;
    virtual const char* Name() const final { return StaticName; }
    virtual bool IsOption_ShidoScoreCounts() const final { return false; }
    virtual bool IsOption_HasYuko() const final { return false; }
    virtual bool IsOption_AwaseteIppon() const { return false; }
    virtual bool IsAwaseteIppon(Score const&) const final { return false; }
    virtual int GetMaxShidoCount() const final { return 2; }

    virtual int GetOsaekomiValue(Score::Point p) const final
    {
       switch(p)
       {
        case Score::Point::Ippon: return 20;
        case Score::Point::Wazaari: return 10;
        default: return -1;
       }
    }

    virtual int GetMaxWazaariCount() const final { return INT32_MAX; }
};

class Rules2017U15 : public AbstractRules
{
public:
    Rules2017U15() {}

    static const char* const StaticName;
    virtual const char* Name() const final { return StaticName; }
    virtual bool IsOption_ShidoScoreCounts() const final { return false; }
    virtual bool IsOption_HasYuko() const final { return false; }
    virtual bool IsOption_AwaseteIppon() const { return false; }
    virtual bool IsAwaseteIppon(Score const&) const final { return false; }
    virtual int GetMaxShidoCount() const final { return 3; }

    virtual int GetOsaekomiValue(Score::Point p) const final
    {
       switch(p)
       {
        case Score::Point::Ippon: return 20;
        case Score::Point::Wazaari: return 10;
        default: return -1;
       }
    }

    virtual int GetMaxWazaariCount() const final { return INT32_MAX; }
};

class RulesFactory
{
public:
    static std::shared_ptr<AbstractRules> Create(QString name)
    {
        if (name == ClassicRules::StaticName)
        {
            return std::make_shared<ClassicRules>();
        }

        if (name == Rules2013::StaticName)
        {
            return std::make_shared<Rules2013>();
        }

        if (name == Rules2017U15::StaticName)
        {
            return std::make_shared<Rules2017U15>();
        }

        // default
        return std::make_shared<Rules2017>();
    }

    static QStringList GetNames()
    {
        auto result = QStringList();

        result.push_back(Rules2017::StaticName);
        result.push_back(Rules2017U15::StaticName);
        result.push_back(Rules2013::StaticName);
        result.push_back(ClassicRules::StaticName);

        return result;
    }

    static QString GetDefaultName()
    {
        return Rules2017::StaticName;
    }
};

} // namespace
#endif // RULESET_H
