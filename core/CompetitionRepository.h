#pragma once

#include "Competition.h"
#include "CompetitionModel.h"

#include <QStringList>
#include <memory>

namespace Ipponboard
{

class AbstractRules;
class CompetitionMode;

class CompetitionRepository
{
  public:
    CompetitionRepository(Competition& competition,
                          std::vector<std::shared_ptr<CompetitionModel>>& models);

    Fight& fight(unsigned int round, unsigned int index);
    Fight const& fight(unsigned int round, unsigned int index) const;

    void setFight(unsigned int round_index, unsigned int fight_index, const QString& weight,
                  const QString& first_player_name, const QString& first_player_club,
                  const QString& second_player_name, const QString& second_player_club, int yuko1,
                  int wazaari1, int ippon1, int shido1, int hansokumake1, int yuko2, int wazaari2,
                  int ippon2, int shido2, int hansokumake2,
                  std::shared_ptr<AbstractRules> const& rules, const QString& emptyName);

    void clearAllFights(std::shared_ptr<AbstractRules> const& rules, const CompetitionMode& mode,
                        const QString& emptyName);

    void saveFight(unsigned int round, unsigned int fight, int elapsedSeconds);

    void resetFightData(unsigned int round, unsigned int fight,
                        std::shared_ptr<AbstractRules> const& rules, const CompetitionMode& mode);

    void setWeights(const QStringList& weights, const CompetitionMode& mode);

  private:
    Competition& m_tournament;
    std::vector<std::shared_ptr<CompetitionModel>>& m_models;
};

} // namespace Ipponboard
