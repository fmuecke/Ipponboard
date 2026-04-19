// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef TOURNAMENTMODE_H
#define TOURNAMENTMODE_H

#include "Enums.h"
#include "Rules.h"

#include <QString>
#include <QStringList>
#include <vector>

class QSettings;
struct IpponboardTest;

namespace Ipponboard
{

class CompetitionMode
{
    friend IpponboardTest;

  public:
    CompetitionMode();

    typedef std::vector<CompetitionMode> List;
    typedef std::vector<std::pair<QString, int>> OverridesList;

    // ascii strings
    static QString const& str_TemplateDirName;
    static QString const& str_Title;
    static QString const& str_SubTitle;
    static QString const& str_Weights;
    static QString const& str_Template;
    static QString const& str_Options;
    static QString const& str_Rules;
    static QString const& str_Rounds;
    static QString const& str_TimeInSeconds;
    static QString const& str_TimeOverrides;
    static QString const& str_WeightsAreDoubled;
    static QString const& str_none;
    static QString const& str_Option_AllSubscoresCount;

    static bool ReadModes(QString const& filename, CompetitionMode::List& modes, QString& errorMsg);

    static bool WriteModes(QString const& filename, List const& modes, QString& errorMsg);

    static CompetitionMode Default();

    bool operator<(CompetitionMode const& other) const;

    QString Description() const;
    int FightsPerRound() const;
    int GetFightDuration(QString const& weight) const;
    bool IsOptionSet(QString const& option) const;
    void SetOption(QString const& option, bool checked);
    QString GetTimeOverridesString() const;
    static bool ExtractTimeOverrides(QString const& overridesString, OverridesList& overrides);

  private:
    static bool parse_current_group(QSettings const& config, CompetitionMode& tm,
                                    QString const& configFilePath, QString& errorMsg);

    static bool verify_child_keys(QStringList const& childKeys, QString& errorMsg);

  public: // nothing to encapsulate here
    QString id;
    QString title;
    QString subTitle;
    QString weights;
    QString listTemplate;
    QString options;
    QString rules{ RulesFactory::GetDefaultName() };
    OverridesList contestTimeOverrides;
    int nRounds;
    int timeInSeconds; // TODO: rename to duration!
    bool weightsAreDoubled;
};

} // namespace Ipponboard

#endif // TOURNAMENTMODE_H
