// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#ifndef TOURNAMENTMODE_H
#define TOURNAMENTMODE_H

#include "Enums.h"

#include <QString>
#include <QStringList>
#include <vector>

class QSettings;
class TestTournamentMode;

namespace Ipponboard
{

class TournamentMode
{
	friend class TestTournamentMode;

public:
	TournamentMode();

    typedef std::vector<TournamentMode> List;
    typedef std::vector<std::pair<QString,int>> OverridesList;

	// ascii strings
    static const char* TemplateDirName() { return "templates"; }
	static QString const& str_Title;
	static QString const& str_SubTitle;
	static QString const& str_Weights;
	static QString const& str_Template;
    static QString const& str_Options;
	static QString const& str_Rounds;
	static QString const& str_FightTimeInSeconds;
	static QString const& str_FightTimeOverrides;
	static QString const& str_WeightsAreDoubled;
    static QString const& str_none;

	static bool ReadModes(
        QString const& filename,
        TournamentMode::List& modes,
		QString& errorMsg);

    static bool WriteModes(
        QString const& filename,
        List const& modes,
        QString& errorMsg);

	bool operator< (TournamentMode const& other) const;

	QString FullTitle() const;
	int FightsPerRound() const;
	int GetFightDuration(QString const& weight) const;
    bool IsOptionSet(EOption o) const;
    void SetOption(EOption o, bool checked);
    QString GetFightTimeOverridesString() const;
    static bool ExtractFightTimeOverrides(QString const& overridesString, OverridesList& overrides);

private:
	static bool parse_current_group(
		QSettings const& config,
		TournamentMode& tm,
		QString& errorMsg);

    static bool verify_child_keys(QStringList const& childKeys, QString& errorMsg);

public: // nothing to encapsulate here
	QString name;
	QString title;
	QString subTitle;
	QString weights;
	QString listTemplate;
    QString options;
    OverridesList fightTimeOverrides;
	int nRounds;
	int fightTimeInSeconds; // TODO: rename to duration!
    bool weightsAreDoubled;
};

}  // namespace Ipponboard

#endif // TOURNAMENTMODE_H
