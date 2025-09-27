// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__TOURNAMENT_H_
#define BASE__TOURNAMENT_H_

#include "Score.h"
#include "Fight.h"

#include <QString>
#include <vector>
#include <memory>

namespace Ipponboard
{

typedef std::vector<Fight> TournamentRound;
typedef std::shared_ptr< TournamentRound > PTournamentRound;
typedef std::vector< PTournamentRound > Tournament;

} // namespace Ipponboard


#endif  // BASE__TOURNAMENT_H_
