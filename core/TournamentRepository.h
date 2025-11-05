#pragma once

#include "Tournament.h"
#include "TournamentModel.h"

#include <QStringList>
#include <memory>

namespace Ipponboard
{

class AbstractRules;
class TournamentMode;

class TournamentRepository
{
  public:
    TournamentRepository(Tournament& tournament,
                         std::vector<std::shared_ptr<TournamentModel>>& models);

    Fight& fight(unsigned int round, unsigned int index);
    Fight const& fight(unsigned int round, unsigned int index) const;

    void setFight(unsigned int round_index, unsigned int fight_index, const QString& weight,
                  const QString& first_player_name, const QString& first_player_club,
                  const QString& second_player_name, const QString& second_player_club, int yuko1,
                  int wazaari1, int ippon1, int shido1, int hansokumake1, int yuko2, int wazaari2,
                  int ippon2, int shido2, int hansokumake2,
                  std::shared_ptr<AbstractRules> const& rules, const QString& emptyName);

    void clearAllFights(std::shared_ptr<AbstractRules> const& rules, const TournamentMode& mode,
                        const QString& emptyName);

    void saveFight(unsigned int round, unsigned int fight, int elapsedSeconds);

    void resetFightData(unsigned int round, unsigned int fight,
                        std::shared_ptr<AbstractRules> const& rules, const TournamentMode& mode);

    void setWeights(const QStringList& weights, const TournamentMode& mode);

  private:
    Tournament& m_tournament;
    std::vector<std::shared_ptr<TournamentModel>>& m_models;
};

} // namespace Ipponboard
