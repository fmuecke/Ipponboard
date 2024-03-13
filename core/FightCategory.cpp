// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/debug.h"
#include "FightCategory.h"
#include <QTime>

using namespace Ipponboard;

QStringList FightCategory::GetWeightsList() const
{
    TRACE(2, "FightCategory::GetWeightsList()");
    return QString::fromStdString(weights).split(';');
}

QString FightCategory::GetRoundTimeStr() const
{
    TRACE(2, "FightCategory::GetRoundTimeStr()");
    return QTime(0,0,0,0).addSecs(round_time_secs).toString("m:ss");
}
QString FightCategory::GetGoldenScoreTimeStr() const
{
    TRACE(2, "FightCategory::GetGoldenScoreTimeStr()");
    return QTime(0,0,0,0).addSecs(golden_score_time_secs).toString("m:ss");
}

void FightCategory::SetRoundTime(QString const& str)
{
    TRACE(2, "FightCategory::SetRoundTime(str=%s)", str.toUtf8().data());
    round_time_secs = QTime().secsTo(QTime::fromString(str, "m:ss"));
}

void FightCategory::SetGoldenScoreTime(QString const& str)
{
    TRACE(2, "FightCategory::SetRoundTime(str=%s)", str.toUtf8().data());
    golden_score_time_secs = QTime().secsTo(QTime::fromString(str, "m:ss"));
}
