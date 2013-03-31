#include "FightCategory.h"
#include <QTime>

using namespace Ipponboard;

QStringList FightCategory::GetWeightsList() const
{
	return QString::fromStdString(weights).split(';');
}

QString FightCategory::GetRoundTimeStr() const
{
	return QTime().addSecs(round_time_secs).toString("m:ss");
}
QString FightCategory::GetGoldenScoreTimeStr() const
{
	return QTime().addSecs(golden_score_time_secs).toString("m:ss");
}

void FightCategory::SetRoundTime(QString const& str)
{
	round_time_secs =
		QTime().secsTo(QTime::fromString(str, "m:ss"));
}

void FightCategory::SetGoldenScoreTime(QString const& str)
{
	golden_score_time_secs =
		QTime().secsTo(QTime::fromString(str, "m:ss"));
}
