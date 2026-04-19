// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ContestCategory.h"

#include <QTime>

using namespace Ipponboard;

QStringList ContestCategory::GetWeightsList() const
{
    return QString::fromStdString(weights).split(';');
}

QString ContestCategory::GetRoundTimeStr() const
{
    return QTime(0, 0, 0, 0).addSecs(round_time_secs).toString("m:ss");
}
QString ContestCategory::GetGoldenScoreTimeStr() const
{
    return QTime(0, 0, 0, 0).addSecs(golden_score_time_secs).toString("m:ss");
}

void ContestCategory::SetRoundTime(QString const& str)
{
    round_time_secs = QTime(0, 0, 0, 0).secsTo(QTime::fromString(str, "m:ss"));
}

void ContestCategory::SetGoldenScoreTime(QString const& str)
{
    golden_score_time_secs = QTime(0, 0, 0, 0).secsTo(QTime::fromString(str, "m:ss"));
}
