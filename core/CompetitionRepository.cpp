#include "CompetitionRepository.h"

#include "CompetitionMode.h"
#include "Contest.h"

namespace Ipponboard
{

CompetitionRepository::CompetitionRepository(Competition& competition,
                                             std::vector<std::shared_ptr<CompetitionModel>>& models)
    : m_competition(competition), m_models(models)
{
}

Contest& CompetitionRepository::contest(unsigned int round, unsigned int index)
{
    return m_competition.at(round)->at(index);
}

Contest const& CompetitionRepository::contest(unsigned int round, unsigned int index) const
{
    return m_competition.at(round)->at(index);
}

void CompetitionRepository::setContest(
    unsigned int roundIndex, unsigned int contestIndex, const QString& weight,
    const QString& sideAName, const QString& sideAClub, const QString& sideBName,
    const QString& sideBClub, int yukoSideA, int wazaariSideA, int ipponSideA, int shidoSideA,
    int hansokumakeSideA, int yukoSideB, int wazaariSideB, int ipponSideB, int shidoSideB,
    int hansokumakeSideB, std::shared_ptr<AbstractRules> const& rules, const QString& emptyName)
{
    Contest& target = contest(roundIndex, contestIndex);
    target.weight = weight;
    target.SetSecondsElapsed(0);
    target.rules = rules;

    auto sideA = ContestSide::SideA;
    auto sideB = ContestSide::SideB;
    auto Yuko = Score::Point::Yuko;
    auto Wazaari = Score::Point::Wazaari;
    auto Ippon = Score::Point::Ippon;
    auto Shido = Score::Point::Shido;
    auto Hansokumake = Score::Point::Hansokumake;

    target.GetAthlete(sideA).name = sideAName.isEmpty() ? emptyName : sideAName;
    target.GetAthlete(sideA).club = sideAClub;
    target.GetScore(sideA).Clear();

    while (yukoSideA != -1 && yukoSideA > 0)
    {
        target.GetScore(sideA).Add(Yuko);
        --yukoSideA;
    }

    while (wazaariSideA != -1 && wazaariSideA > 0)
    {
        target.GetScore(sideA).Add(Wazaari);
        --wazaariSideA;
    }

    if (ipponSideA > 0)
    {
        target.GetScore(sideA).Add(Ippon);
    }

    while (shidoSideA != -1 && shidoSideA > 0)
    {
        target.GetScore(sideA).Add(Shido);
        --shidoSideA;
    }

    if (hansokumakeSideA > 0)
    {
        target.GetScore(sideA).Add(Hansokumake);
    }

    target.GetAthlete(sideB).name = sideBName.isEmpty() ? emptyName : sideBName;
    target.GetAthlete(sideB).club = sideBClub;
    target.GetScore(sideB).Clear();

    while (yukoSideB != -1 && yukoSideB > 0)
    {
        target.GetScore(sideB).Add(Yuko);
        --yukoSideB;
    }

    while (wazaariSideB != -1 && wazaariSideB > 0)
    {
        target.GetScore(sideB).Add(Wazaari);
        --wazaariSideB;
    }

    if (ipponSideB > 0)
    {
        target.GetScore(sideB).Add(Ippon);
    }

    while (shidoSideB != -1 && shidoSideB > 0)
    {
        target.GetScore(sideB).Add(Shido);
        --shidoSideB;
    }

    if (hansokumakeSideB > 0)
    {
        target.GetScore(sideB).Add(Hansokumake);
    }

    m_models[roundIndex]->SetDataChanged();
}

void CompetitionRepository::clearAllFights(std::shared_ptr<AbstractRules> const& rules,
                                           const CompetitionMode& mode, const QString& emptyName)
{
    if (m_competition.empty())
    {
        return;
    }

    for (unsigned int round = 0; round < m_competition.size(); ++round)
    {
        for (unsigned int contestIndex = 0; contestIndex < m_competition.front()->size();
             ++contestIndex)
        {
            setContest(round,
                       contestIndex,
                       QString(),
                       QString(),
                       QString(),
                       QString(),
                       QString(),
                       0,
                       0,
                       0,
                       0,
                       0,
                       0,
                       0,
                       0,
                       0,
                       0,
                       rules,
                       emptyName);
            Contest& storedContest = contest(round, contestIndex);
            storedContest.SetRoundTime(mode.GetFightDuration(storedContest.weight));
            storedContest.SetSecondsElapsed(0);
            storedContest.SetGoldenScore(false);
            storedContest.is_saved = false;
        }
    }
}

void CompetitionRepository::saveContest(unsigned int round, unsigned int contestIndex,
                                        int elapsedSeconds)
{
    Contest& current = contest(round, contestIndex);
    current.SetSecondsElapsed(elapsedSeconds);
    current.is_saved = true;
}

void CompetitionRepository::resetContestData(unsigned int round, unsigned int contestIndex,
                                             std::shared_ptr<AbstractRules> const& rules,
                                             const CompetitionMode& mode)
{
    Contest& current = contest(round, contestIndex);
    current.GetScore(ContestSide::SideA).Clear();
    current.GetScore(ContestSide::SideB).Clear();
    current.SetSecondsElapsed(0);
    current.SetGoldenScore(false);
    current.is_saved = false;
    current.rules = rules;
    current.SetRoundTime(mode.GetFightDuration(current.weight));
}

void CompetitionRepository::setWeights(const QStringList& weights, const CompetitionMode& mode)
{
    if (m_competition.empty())
    {
        return;
    }

    const auto contestsPerRound = static_cast<int>(m_competition.front()->size());

    auto applyWeight = [&](Contest& contest, const QString& weight)
    {
        contest.weight = weight;
        contest.SetRoundTime(mode.GetFightDuration(weight));
    };

    if (weights.count() == contestsPerRound)
    {
        for (auto& round : m_competition)
        {
            for (int contestIndex = 0; contestIndex < contestsPerRound; ++contestIndex)
            {
                applyWeight(round->at(contestIndex), weights.at(contestIndex));
            }
        }
    }
    else
    {
        for (auto& round : m_competition)
        {
            for (int contestIndex = 0; contestIndex < contestsPerRound; ++contestIndex)
            {
                const QString& weight = weights.at(contestIndex / 2);
                applyWeight(round->at(contestIndex), weight);
            }
        }
    }

    for (auto const& model : m_models)
    {
        model->SetDataChanged();
    }
}

} // namespace Ipponboard
