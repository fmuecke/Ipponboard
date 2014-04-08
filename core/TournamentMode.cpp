// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#include "TournamentMode.h"
#include "EnumStrings.h"

#include <QString>
#include <QStringList>
#include <QFile>
#include <QSettings>
#include <regex>

using namespace Ipponboard;

QString const& TournamentMode::str_Title("Title");
QString const& TournamentMode::str_SubTitle("SubTitle");
QString const& TournamentMode::str_Weights("Weights");
QString const& TournamentMode::str_Template("Template");
QString const& TournamentMode::str_FightTimeOverrides("FightTimeOverrides");
QString const& TournamentMode::str_Options("Options");
QString const& TournamentMode::str_Rounds("Rounds");
QString const& TournamentMode::str_FightTimeInSeconds("FightTimeInSeconds");
QString const& TournamentMode::str_WeightsAreDoubled("WeightsAreDoubled");

TournamentMode::TournamentMode()
	: name("SingeTournament")
	, title("Single Tournament")
	, subTitle("Ipponboard")
	, weights()
	, listTemplate()
    , options()
	, fightTimeOverrides()
	, nRounds(1)
	, fightTimeInSeconds(300)
	, weightsAreDoubled(false)
{
}

bool TournamentMode::ReadModes(
	const QString& filename,
    TournamentMode::List& modes,
	QString& errorMsg)
{
	errorMsg.clear();

	QFile file(filename);

	if (!file.exists())
	{
		errorMsg = QString("%1 not found!").arg(filename);
		return false;
	}

    QSettings config(filename, QSettings::IniFormat, nullptr);
    config.setIniCodec("UTF-8");
	QStringList groups = config.childGroups();

	if (groups.isEmpty())
	{
		errorMsg = QString("%1 does not contain any mode definitions!").arg(filename);
		return false;
	}

    TournamentMode::List _modes;
    for (QString const & group : groups)
	{
        TournamentMode mode;

		config.beginGroup(group);
        bool readSuccess = parse_current_group(config, mode, errorMsg);
		config.endGroup();

		if (!readSuccess)
		{
			errorMsg = QString("Error parsing file %1. %2").arg(filename, errorMsg);
			return false;
		}

        _modes.push_back(mode);
	}

    std::sort(begin(_modes), end(_modes));

	// all Ok, swap to internal
    modes.swap(_modes);

    return true;
}

bool TournamentMode::WriteModes(const QString &filename, TournamentMode::List const& modes, QString &errorMsg)
{
    errorMsg.clear();

    QFile file(filename);
    if (file.exists() && !file.remove())
    {
        errorMsg = QString("Can not write to %1!").arg(filename);
        return false;
    }

    QSettings config(filename, QSettings::IniFormat, nullptr);
    config.setIniCodec("UTF-8");

    for (auto const& mode : modes)
    {
        config.beginGroup(mode.name);

        config.setValue(str_Title, mode.title);
        config.setValue(str_SubTitle, mode.subTitle);
        config.setValue(str_Weights, mode.weights);
        config.setValue(str_Template, mode.listTemplate);
        config.setValue(str_Rounds, mode.nRounds);
        config.setValue(str_FightTimeInSeconds, mode.fightTimeInSeconds);
        config.setValue(str_WeightsAreDoubled, mode.weightsAreDoubled);
        config.setValue(str_Options, mode.options);
        config.setValue(str_FightTimeOverrides, mode.GetFightTimeOverridesString());

        config.endGroup();
    }

    return true;
}

bool TournamentMode::operator<(TournamentMode const& other) const
{
	return FullTitle() < other.FullTitle();
}

QString TournamentMode::FullTitle() const
{
	return subTitle.isEmpty() ?
		   title :
		   QString("%1 - %2").arg(title, subTitle);
}

int TournamentMode::FightsPerRound() const
{
	if (weights.isEmpty())
	{
		return 1;
	}

	const auto nWeights = weights.split(';').count();
	return weightsAreDoubled ? nWeights * 2 : nWeights;
}

int TournamentMode::GetFightDuration(const QString& weight) const
{
	for (auto it = begin(fightTimeOverrides); it != end(fightTimeOverrides); ++it)
	{
		if (weight.contains(it->first))
		{
			return it->second;
		}
	}

    return fightTimeInSeconds;
}

bool TournamentMode::IsOptionSet(EOption o) const
{
    if (options.isEmpty())
    {
        return false;
    }

    return options.contains(EnumToString(o));
}

void TournamentMode::SetOption(EOption o, bool checked)
{
    auto option = EnumToString(o);

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
            options.replace(";;", ";");
        }
    }
}

QString TournamentMode::GetFightTimeOverridesString() const
{
    QString ret;

    for (auto const& p : fightTimeOverrides)
    {
        if (!ret.isEmpty())
        {
            ret += ";";
        }

        ret += QString("%1:%2").arg(p.first, QString::number(p.second));
    }

    return ret;
}

bool TournamentMode::ExtractFightTimeOverrides(const QString& overridesString, OverridesList& overrides)
{
    if (!std::regex_match(overridesString.toStdString(), std::regex("(\\w+:\\d+;)*(\\w+:\\d+)$")))
    {
        return false;
    }

    OverridesList result;
    QStringList splittedTimes = overridesString.split(';');
    for (QString const & s : splittedTimes)
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

bool TournamentMode::parse_current_group(
	QSettings const& config,
    TournamentMode& mode,
	QString& errorMsg)
{
	if (!verify_child_keys(config.childKeys(), errorMsg))
	{
		errorMsg = QString("Error in section [%1]: %2").arg(config.group(), errorMsg);
		return false;
	}

	const QString err = "The key [%1] in section [%2] is empty";
	const QString errInvalid = "The key [%1] in section [%2] is invalid";

    mode.name = config.group();
    mode.title = config.value(TournamentMode::str_Title).toString();
    mode.subTitle = config.value(TournamentMode::str_SubTitle).toString();
    mode.weights = config.value(TournamentMode::str_Weights).toString();
    mode.listTemplate = config.value(TournamentMode::str_Template).toString();
    mode.nRounds = config.value(TournamentMode::str_Rounds).toUInt();
    mode.fightTimeInSeconds = config.value(TournamentMode::str_FightTimeInSeconds).toUInt();
    mode.weightsAreDoubled = config.value(TournamentMode::str_WeightsAreDoubled, false).toBool();
    mode.options = config.value(TournamentMode::str_Options, QString()).toString();
	const QString fightTimeOverridesString = config.value(TournamentMode::str_FightTimeOverrides).toString();

    if (mode.weights.isEmpty())
	{
		errorMsg = err.arg(TournamentMode::str_Weights, config.group());
		return false;
	}

    if (mode.listTemplate.isEmpty())
	{
		errorMsg = err.arg(TournamentMode::str_Template, config.group());
		return false;
	}
	else
	{
        QString templateFile = QString("%1/%2").arg(TemplateDirName(), mode.listTemplate);
        QFile listTemplate(templateFile);

		if (!listTemplate.exists())
		{
			errorMsg = QString("The list template for [%2] is not valid: \"%1\"")
                       .arg(templateFile, config.group());

			return false;
		}
	}

    if (!mode.options.isEmpty())
    {
        // nothing to do
    }

    if (mode.nRounds == 0)
	{
		errorMsg = err.arg(TournamentMode::str_Rounds, config.group());
		return false;
	}

    if (mode.fightTimeInSeconds == 0)
	{
		errorMsg = err.arg(TournamentMode::str_FightTimeInSeconds, config.group());
		return false;
	}

    if (mode.title.isEmpty())
	{
		errorMsg = err.arg(TournamentMode::str_Title, config.group());
		return false;
	}

	if (!fightTimeOverridesString.isEmpty())
	{
        if (!ExtractFightTimeOverrides(fightTimeOverridesString, mode.fightTimeOverrides))
        {
            errorMsg = errInvalid.arg(TournamentMode::str_FightTimeOverrides, config.group());
            return false;
        }

        if (mode.fightTimeOverrides.empty())
		{
			errorMsg = errInvalid.arg(TournamentMode::str_FightTimeOverrides, config.group());
			return false;
		}
	}

	return true;
}

bool TournamentMode::verify_child_keys(QStringList const& childKeys, QString& errorMsg)
{
	QStringList mandatoryKeys;
	mandatoryKeys
			<< str_Title
			<< str_Weights
			<< str_Template
			<< str_Rounds
			<< str_FightTimeInSeconds;

	QStringList optionalKeys;
	optionalKeys
			<< str_SubTitle
			<< str_FightTimeOverrides
            << str_WeightsAreDoubled
            << str_Options;

    for (QString const & key : childKeys)
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
