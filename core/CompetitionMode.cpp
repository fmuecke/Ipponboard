// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "CompetitionMode.h"

#include "../util/path_helpers.h"
#include "Rules.h"

#include <QDebug>
#include <QDir>
#include <QFile>
#include <QRegularExpression>
#include <QSettings>
#include <QString>
#include <QStringList>
#include <QUuid>

using namespace Ipponboard;

QString const& CompetitionMode::str_TemplateDirName("templates");
QString const& CompetitionMode::str_Title("Title");
QString const& CompetitionMode::str_SubTitle("SubTitle");
QString const& CompetitionMode::str_Weights("Weights");
QString const& CompetitionMode::str_Template("Template");
QString const& CompetitionMode::str_TimeOverrides("TimeOverrides");
QString const& CompetitionMode::str_Options("Options");
QString const& CompetitionMode::str_Rounds("Rounds");
QString const& CompetitionMode::str_Rules("Rules");
QString const& CompetitionMode::str_TimeInSeconds("TimeInSeconds");
QString const& CompetitionMode::str_WeightsAreDoubled("WeightsAreDoubled");
QString const& CompetitionMode::str_Option_AllSubscoresCount("AllSubscoresCount");

CompetitionMode::CompetitionMode()
    : id("SingeCompetition"),
      title("Single Competition"),
      subTitle("Ipponboard"),
      weights(),
      listTemplate(),
      options(),
      contestTimeOverrides(),
      nRounds(1),
      timeInSeconds(240),
      weightsAreDoubled(false)
{
}

bool CompetitionMode::ReadModes(const QString& filename, CompetitionMode::List& modes,
                                QString& errorMsg)
{
    errorMsg.clear();
    qInfo() << "Reading competition modes from:" << filename;

    QFile file(filename);

    if (!file.exists())
    {
        errorMsg = QString("%1 not found!").arg(filename);
        qWarning() << errorMsg;
        return false;
    }

    QSettings config(filename, QSettings::IniFormat, nullptr);
    QStringList groups = config.childGroups();

    if (groups.isEmpty())
    {
        errorMsg = QString("%1 does not contain any mode definitions!").arg(filename);
        qWarning() << errorMsg;
        return false;
    }

    CompetitionMode::List _modes;

    for (QString const& group : groups)
    {
        CompetitionMode mode;

        config.beginGroup(group);
        bool readSuccess = parse_current_group(config, mode, filename, errorMsg);
        config.endGroup();

        if (!readSuccess)
        {
            errorMsg = QString("Error parsing file %1. %2").arg(filename, errorMsg);
            qWarning() << errorMsg;
            return false;
        }

        _modes.push_back(mode);
    }

    std::sort(begin(_modes), end(_modes));

    // all Ok, swap to internal
    modes.swap(_modes);

    return true;
}

bool CompetitionMode::WriteModes(const QString& filename, CompetitionMode::List const& modes,
                                 QString& errorMsg)
{
    errorMsg.clear();

    qInfo() << "Writing competition modes to:" << filename;
    QFile file(filename);

    if (file.exists() && !file.remove())
    {
        errorMsg = QString("Can not write to %1!").arg(filename);
        qWarning() << errorMsg;
        return false;
    }

    QSettings config(filename, QSettings::IniFormat, nullptr);

    for (auto const& mode : modes)
    {
        config.beginGroup(mode.id);

        config.setValue(str_Title, mode.title);
        config.setValue(str_SubTitle, mode.subTitle);
        config.setValue(str_Weights, mode.weights);
        config.setValue(str_Template, mode.listTemplate);
        config.setValue(str_Rounds, mode.nRounds);
        config.setValue(str_TimeInSeconds, mode.timeInSeconds);
        config.setValue(str_WeightsAreDoubled, mode.weightsAreDoubled);
        config.setValue(str_Options, mode.options);
        config.setValue(str_Rules, mode.rules);
        config.setValue(str_TimeOverrides, mode.GetTimeOverridesString());

        config.endGroup();
    }

    return true;
}

CompetitionMode CompetitionMode::Default()
{
    CompetitionMode mode;

    mode.id = QUuid::createUuid().toString();
    mode.id = mode.id.mid(1, mode.id.length() - 2); // remove "{}"
    mode.title = "*new*";
    mode.weights = "-66;-73;-81;-90;+90";
    mode.timeInSeconds = 240;
    mode.nRounds = 2;
    mode.weightsAreDoubled = true;
    //mode.listTemplate = m_pUi->comboBox_template->itemText(0);
    mode.rules = RulesFactory::GetDefaultName();

    return mode;
}

bool CompetitionMode::operator<(CompetitionMode const& other) const
{
    return Description() < other.Description();
}

QString CompetitionMode::Description() const
{
    return subTitle.isEmpty() ? title : QString("%1 - %2").arg(title, subTitle);
}

int CompetitionMode::ContestsPerRound() const
{
    if (weights.isEmpty())
    {
        return 1;
    }

    const auto nWeights = weights.split(';').count();
    return weightsAreDoubled ? nWeights * 2 : nWeights;
}

int CompetitionMode::GetContestDuration(const QString& weight) const
{
    for (auto it = begin(contestTimeOverrides); it != end(contestTimeOverrides); ++it)
    {
        if (weight.contains(it->first))
        {
            return it->second;
        }
    }

    return timeInSeconds;
}

bool CompetitionMode::IsOptionSet(QString const& option) const
{
    if (options.isEmpty())
    {
        return false;
    }

    return options.contains(option);
}

void CompetitionMode::SetOption(QString const& option, bool checked)
{
    if (checked)
    {
        if (!options.contains(option))
        {
            options.append(";").append(option);
        }
    }
    else
    {
        if (options.contains(option))
        {
            options.replace(option, QString());
        }
    }

    options.replace(QStringLiteral(";;"), QStringLiteral(";"));
    options.remove(QRegularExpression(QStringLiteral("^;")));
    options.remove(QRegularExpression(QStringLiteral(";$")));
}

QString CompetitionMode::GetTimeOverridesString() const
{
    QString ret;

    for (auto const& p : contestTimeOverrides)
    {
        if (!ret.isEmpty())
        {
            ret += ";";
        }

        ret += QString("%1:%2").arg(p.first, QString::number(p.second));
    }

    return ret;
}

bool CompetitionMode::ExtractTimeOverrides(const QString& overridesString, OverridesList& overrides)
{
    static const QRegularExpression overridesPattern(
        QStringLiteral("^(?:\\w+:\\d+;)*(?:\\w+:\\d+)$"));
    if (!overridesPattern.match(overridesString).hasMatch())
    {
        return false;
    }

    OverridesList result;
    QStringList splittedTimes = overridesString.split(';');

    for (QString const& s : splittedTimes)
    {
        if (!s.contains(':'))
        {
            return false;
        }

        QStringList override = s.split(':');
        std::pair<QString, int> overridePair = std::make_pair(override[0], override[1].toUInt());
        result.push_back(overridePair);
    }

    overrides.swap(result);
    return true;
}

bool CompetitionMode::parse_current_group(QSettings const& config, CompetitionMode& mode,
                                          QString const& configFilePath, QString& errorMsg)
{
    if (!verify_child_keys(config.childKeys(), errorMsg))
    {
        errorMsg = QString("Error in section [%1]: %2").arg(config.group(), errorMsg);
        return false;
    }

    const QString err = "The key [%1] in section [%2] is empty";
    const QString errInvalid = "The key [%1] in section [%2] is invalid";

    mode.id = config.group();
    mode.title = config.value(CompetitionMode::str_Title).toString();
    mode.subTitle = config.value(CompetitionMode::str_SubTitle).toString();
    mode.weights = config.value(CompetitionMode::str_Weights).toString();
    mode.listTemplate = config.value(CompetitionMode::str_Template).toString();
    mode.nRounds = config.value(CompetitionMode::str_Rounds).toUInt();
    mode.nRounds =
        mode.nRounds > 2
            ? 2
            : mode.nRounds; // restrict to two rounds for now as the lists do not handle more
    mode.timeInSeconds = config.value(CompetitionMode::str_TimeInSeconds).toUInt();
    mode.weightsAreDoubled = config.value(CompetitionMode::str_WeightsAreDoubled, false).toBool();
    mode.options = config.value(CompetitionMode::str_Options, QString()).toString();
    mode.rules = config.value(CompetitionMode::str_Rules, mode.rules).toString();
    const QString contestTimeOverridesString =
        config.value(CompetitionMode::str_TimeOverrides).toString();

    if (mode.weights.isEmpty())
    {
        errorMsg = err.arg(CompetitionMode::str_Weights, config.group());
        return false;
    }

    if (mode.listTemplate.isEmpty())
    {
        errorMsg = err.arg(CompetitionMode::str_Template, config.group());
        return false;
    }
    else
    {
        const QString templateReference =
            mode.listTemplate.contains(QChar('/')) ||
                    mode.listTemplate.contains(QDir::separator()) ||
                    mode.listTemplate.startsWith(QStringLiteral(":/"))
                ? mode.listTemplate
                : QDir(CompetitionMode::str_TemplateDirName).filePath(mode.listTemplate);
        const QString templateFile = fm::ResolveConfigOwnedAsset(configFilePath, templateReference);
        QFile listTemplate(templateFile);

        if (!listTemplate.exists())
        {
            errorMsg = QString("The list template for [%2] could not be found: \"%1\"")
                           .arg(templateFile, config.group());

            return false;
        }
    }

    if (!mode.options.isEmpty())
    {
        // remove no longer supported options
        mode.options.replace("AutoIncrementPoints", QString());
        mode.options.replace("Use2013Rules", QString());
    }

    if (mode.nRounds == 0)
    {
        errorMsg = err.arg(CompetitionMode::str_Rounds, config.group());
        return false;
    }

    if (mode.timeInSeconds == 0)
    {
        errorMsg = err.arg(CompetitionMode::str_TimeInSeconds, config.group());
        return false;
    }

    if (mode.title.isEmpty())
    {
        errorMsg = err.arg(CompetitionMode::str_Title, config.group());
        return false;
    }

    if (!contestTimeOverridesString.isEmpty())
    {
        if (!ExtractTimeOverrides(contestTimeOverridesString, mode.contestTimeOverrides))
        {
            errorMsg = errInvalid.arg(CompetitionMode::str_TimeOverrides, config.group());
            return false;
        }

        if (mode.contestTimeOverrides.empty())
        {
            errorMsg = errInvalid.arg(CompetitionMode::str_TimeOverrides, config.group());
            return false;
        }
    }

    return true;
}

bool CompetitionMode::verify_child_keys(QStringList const& childKeys, QString& errorMsg)
{
    QStringList mandatoryKeys;
    mandatoryKeys << str_Title << str_Weights << str_Template << str_Rounds << str_TimeInSeconds;

    QStringList optionalKeys;
    optionalKeys << str_TimeOverrides << str_SubTitle << str_WeightsAreDoubled << str_Rules
                 << str_Options;

    for (QString const& key : childKeys)
    {
        // check manadatory keys
        auto pos = std::find(mandatoryKeys.begin(), mandatoryKeys.end(), key);

        if (pos != mandatoryKeys.end())
        {
            mandatoryKeys.erase(pos);
            continue;
        }

        // check optional keys
        pos = std::find(optionalKeys.begin(), optionalKeys.end(), key);

        if (pos != optionalKeys.end())
        {
            optionalKeys.erase(pos);
            continue;
        }

        errorMsg = QString("Key [%1] is not recognized.").arg(key);
        return false;
    }

    if (!mandatoryKeys.empty())
    {
        errorMsg = QString("Mandatory key [%1] is not set.").arg(mandatoryKeys[0]);
        return false;
    }

    return true;
}
