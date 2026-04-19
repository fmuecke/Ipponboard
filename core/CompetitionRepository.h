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

    Contest& contest(unsigned int round, unsigned int index);
    Contest const& contest(unsigned int round, unsigned int index) const;

    void setContest(unsigned int roundIndex, unsigned int contestIndex, const QString& weight,
                    const QString& sideAName, const QString& sideAClub, const QString& sideBName,
                    const QString& sideBClub, int yukoSideA, int wazaariSideA, int ipponSideA,
                    int shidoSideA, int hansokumakeSideA, int yukoSideB, int wazaariSideB,
                    int ipponSideB, int shidoSideB, int hansokumakeSideB,
                    std::shared_ptr<AbstractRules> const& rules, const QString& emptyName);

    void clearAllFights(std::shared_ptr<AbstractRules> const& rules, const CompetitionMode& mode,
                        const QString& emptyName);

    void saveContest(unsigned int round, unsigned int contest, int elapsedSeconds);

    void resetContestData(unsigned int round, unsigned int contest,
                          std::shared_ptr<AbstractRules> const& rules,
                          const CompetitionMode& mode);

    void setWeights(const QStringList& weights, const CompetitionMode& mode);

  private:
    Competition& m_competition;
    std::vector<std::shared_ptr<CompetitionModel>>& m_models;
};

} // namespace Ipponboard
