// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__TOURNAMENT_H_
#define BASE__TOURNAMENT_H_

#include "Contest.h"
#include "Score.h"

#include <QString>
#include <memory>
#include <vector>

namespace Ipponboard
{

typedef std::vector<Contest> ContestRound;
typedef std::shared_ptr<ContestRound> PContestRound;
typedef std::vector<PContestRound> Competition;

} // namespace Ipponboard

#endif // BASE__TOURNAMENT_H_
