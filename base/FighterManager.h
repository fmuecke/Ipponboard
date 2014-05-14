// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
#ifndef _BASE_FIGHTERMANAGER_H
#define _BASE_FIGHTERMANAGER_H

#include "../core/Fighter.h"

#include <QString>
#include <QStringList>
#include <boost/noncopyable.hpp>
#include <array>
#include <set>

class QString;
class QStringList;

namespace Ipponboard
{

class FighterManager : public boost::noncopyable
{
public:
    FighterManager();

    static char const* const str_firstname;
    static char const* const str_lastname;
    static char const* const str_club;
    static char const* const str_weight;
    static char const* const str_category;
    static char const* const str_nation;
    static char const* const str_year;

    static const std::array<char const* const, 7> Specifiers;

	static QString GetSpecifiererDescription();
	static bool IsFormatSatisfying(QString const& formatStr);
	static bool DetermineSeparator(QString const& str, QString& sep);

	static QString DefaultExportFormat();

    bool ImportFighters(QString const& fileName, QString const& formatStr, QString const& separator, QString const& encoding, QString& errorMsg);
    bool ImportFighters(QString const& fileName, QString const& formatStr, QString const& encoding, QString& errorMsg);
	bool ExportFighters(QString const& fileName, QString const& formatStr, QString& errorMsg);

	bool AddFighter(Ipponboard::Fighter f);
	bool RemoveFighter(Ipponboard::Fighter f);
	QStringList GetClubFighterNames(QString const& filter) const;

//private:
	std::set<Ipponboard::Fighter> m_fighters; //TODO: encapsulate
};

}  // namespace Ipponboard

#endif // _BASE_FIGHTERMANAGER_H
