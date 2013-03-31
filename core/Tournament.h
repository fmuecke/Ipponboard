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
