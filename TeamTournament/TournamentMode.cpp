// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#include "TournamentMode.h"

#include <QString>
#include <QStringList>
#include <QFile>
#include <QSettings>

QString const& TournamentMode::str_Title("Title");
QString const& TournamentMode::str_SubTitle("SubTitle");
QString const& TournamentMode::str_Weights("Weights");
QString const& TournamentMode::str_Template("Template");
QString const& TournamentMode::str_FightTimeOverrides("FightTimeOverrides");
QString const& TournamentMode::str_Rounds("Rounds");
QString const& TournamentMode::str_FightsPerRound("FightsPerRound");
QString const& TournamentMode::str_FightTimeInSeconds("FightTimeInSeconds");
QString const& TournamentMode::str_WeightsAreDoubled("WeightsAreDoubled");

TournamentMode::TournamentMode()
	: name()
	, title()
	, subTitle()
	, weights()
	, listTemplate()
	, fightTimeOverrides()
	, nRounds(0)
	, nFightsPerRound(0)
	, fightTimeInSeconds(0)
	, weightsAreDoubled(false)
{
}


bool TournamentMode::ReadModes(
	const QString& filename,
	std::vector<TournamentMode>& modes,
	QString& errorMsg)
{
	errorMsg.clear();

	QFile file(filename);

	if (!file.exists())
	{
		errorMsg = QString("%1 not found!").arg(filename);

		return false;
	}

	QString err = "Empty key [%1] in section [%2] in file [%3]";

	QSettings config(filename, QSettings::IniFormat, nullptr);
	QStringList groups = config.childGroups();

	std::vector<TournamentMode> _modes;
	Q_FOREACH(QString const & group, groups)
	{
		config.beginGroup(group);

		TournamentMode tm;
		tm.name = group;
		tm.title = config.value(TournamentMode::str_Title).toString();
		tm.subTitle = config.value(TournamentMode::str_SubTitle).toString();
		tm.weights = config.value(TournamentMode::str_Weights).toString();
		tm.listTemplate = config.value(TournamentMode::str_Template).toString();
		tm.nRounds = config.value(TournamentMode::str_Rounds).toUInt();
		tm.nFightsPerRound = config.value(TournamentMode::str_FightsPerRound).toUInt();
		tm.fightTimeInSeconds = config.value(TournamentMode::str_FightTimeInSeconds).toUInt();
		tm.weightsAreDoubled = config.value(TournamentMode::str_WeightsAreDoubled, false).toBool();
		tm.fightTimeOverrides = config.value(TournamentMode::str_FightTimeOverrides).toString();

		if (tm.weights.isEmpty())
		{
			errorMsg = err.arg(TournamentMode::str_Weights, group, filename);
			return false;
		}

		if (tm.listTemplate.isEmpty())
		{
			errorMsg = err.arg(TournamentMode::str_Template, group, filename);
			return false;
		}
		else
		{
			QFile listTemplate(tm.listTemplate);

			if (!listTemplate.exists())
			{
				errorMsg = QString("The list template for [%2] is not valid: \"%1\"")
						   .arg(tm.listTemplate, group);

				return false;
			}
		}

		if (tm.nRounds == 0)
		{
			errorMsg = err.arg(TournamentMode::str_Rounds, group, filename);
			return false;
		}

		if (tm.nFightsPerRound == 0)
		{
			errorMsg = err.arg(TournamentMode::str_FightsPerRound, group, filename);
			return false;
		}

		if (tm.fightTimeInSeconds == 0)
		{
			errorMsg = err.arg(TournamentMode::str_FightTimeInSeconds, group, filename);
			return false;
		}

		if (tm.title.isEmpty())
		{
			errorMsg = err.arg(TournamentMode::str_Title, group, filename);
			return false;
		}

		if (!tm.fightTimeOverrides.isEmpty())
		{
			// TODO parse and validate...
		}

		// plausibility checks
		auto nWeights = tm.weights.split(';').count();
		nWeights = tm.weightsAreDoubled ? nWeights * 2 : nWeights;

		if (nWeights != tm.nFightsPerRound)
		{
			errorMsg = QString("Number of weights (%1) does not match number of fights (%2) for [%3]")
					   .arg(QString::number(nWeights), QString::number(tm.nFightsPerRound), group);

			return false;
		}

		config.endGroup();

		_modes.push_back(tm);
	}

	std::sort(begin(modes), end(modes));

	// all Ok, swap to internal
	modes.swap(_modes);
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
