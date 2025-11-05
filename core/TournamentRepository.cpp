#include "TournamentRepository.h"

#include "Fight.h"
#include "TournamentMode.h"

namespace Ipponboard
{

TournamentRepository::TournamentRepository(Tournament& tournament,
                                           std::vector<std::shared_ptr<TournamentModel>>& models)
    : m_tournament(tournament), m_models(models)
{
}

Fight& TournamentRepository::fight(unsigned int round, unsigned int index)
{
    return m_tournament.at(round)->at(index);
}

Fight const& TournamentRepository::fight(unsigned int round, unsigned int index) const
{
    return m_tournament.at(round)->at(index);
}

void TournamentRepository::setFight(
    unsigned int round_index, unsigned int fight_index, const QString& weight,
    const QString& first_player_name, const QString& first_player_club,
    const QString& second_player_name, const QString& second_player_club, int yuko1, int wazaari1,
    int ippon1, int shido1, int hansokumake1, int yuko2, int wazaari2, int ippon2, int shido2,
    int hansokumake2, std::shared_ptr<AbstractRules> const& rules, const QString& emptyName)
{
    Fight& target = fight(round_index, fight_index);
    target.weight = weight;
    target.SetSecondsElapsed(0);
    target.rules = rules;

    auto First = FighterEnum::First;
    auto Second = FighterEnum::Second;
    auto Yuko = Score::Point::Yuko;
    auto Wazaari = Score::Point::Wazaari;
    auto Ippon = Score::Point::Ippon;
    auto Shido = Score::Point::Shido;
    auto Hansokumake = Score::Point::Hansokumake;

    target.fighters[First].name = first_player_name.isEmpty() ? emptyName : first_player_name;
    target.fighters[First].club = first_player_club;
    target.GetScore1().Clear();

    while (yuko1 != -1 && yuko1 > 0)
    {
        target.GetScore1().Add(Yuko);
        --yuko1;
    }

    while (wazaari1 != -1 && wazaari1 > 0)
    {
        target.GetScore1().Add(Wazaari);
        --wazaari1;
    }

    if (ippon1 > 0)
    {
        target.GetScore1().Add(Ippon);
    }

    while (shido1 != -1 && shido1 > 0)
    {
        target.GetScore1().Add(Shido);
        --shido1;
    }

    if (hansokumake1 > 0)
    {
        target.GetScore1().Add(Hansokumake);
    }

    target.fighters[Second].name = second_player_name.isEmpty() ? emptyName : second_player_name;
    target.fighters[Second].club = second_player_club;
    target.GetScore2().Clear();

    while (yuko2 != -1 && yuko2 > 0)
    {
        target.GetScore2().Add(Yuko);
        --yuko2;
    }

    while (wazaari2 != -1 && wazaari2 > 0)
    {
        target.GetScore2().Add(Wazaari);
        --wazaari2;
    }

    if (ippon2 > 0)
    {
        target.GetScore2().Add(Ippon);
    }

    while (shido2 != -1 && shido2 > 0)
    {
        target.GetScore2().Add(Shido);
        --shido2;
    }

    if (hansokumake2 > 0)
    {
        target.GetScore1().Add(Hansokumake);
    }

    m_models[round_index]->SetDataChanged();
}

void TournamentRepository::clearAllFights(std::shared_ptr<AbstractRules> const& rules,
                                          const TournamentMode& mode, const QString& emptyName)
{
    if (m_tournament.empty())
    {
        return;
    }

    for (unsigned int round = 0; round < m_tournament.size(); ++round)
    {
        for (unsigned int fightIndex = 0; fightIndex < m_tournament.front()->size(); ++fightIndex)
        {
            setFight(round,
                     fightIndex,
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
            Fight& f = fight(round, fightIndex);
            f.SetRoundTime(mode.GetFightDuration(f.weight));
            f.SetSecondsElapsed(0);
            f.SetGoldenScore(false);
            f.is_saved = false;
        }
    }
}

void TournamentRepository::saveFight(unsigned int round, unsigned int fightIndex,
                                     int elapsedSeconds)
{
    Fight& current = fight(round, fightIndex);
    current.SetSecondsElapsed(elapsedSeconds);
    current.is_saved = true;
}

void TournamentRepository::resetFightData(unsigned int round, unsigned int fightIndex,
                                          std::shared_ptr<AbstractRules> const& rules,
                                          const TournamentMode& mode)
{
    Fight& current = fight(round, fightIndex);
    current.GetScore1().Clear();
    current.GetScore2().Clear();
    current.SetSecondsElapsed(0);
    current.SetGoldenScore(false);
    current.is_saved = false;
    current.rules = rules;
    current.SetRoundTime(mode.GetFightDuration(current.weight));
}

void TournamentRepository::setWeights(const QStringList& weights, const TournamentMode& mode)
{
    if (m_tournament.empty())
    {
        return;
    }

    const auto fightsPerRound = static_cast<int>(m_tournament.front()->size());

    auto applyWeight = [&](Fight& fight, const QString& weight)
    {
        fight.weight = weight;
        fight.SetRoundTime(mode.GetFightDuration(weight));
    };

    if (weights.count() == fightsPerRound)
    {
        for (auto& round : m_tournament)
        {
            for (int fightIndex = 0; fightIndex < fightsPerRound; ++fightIndex)
            {
                applyWeight(round->at(fightIndex), weights.at(fightIndex));
            }
        }
    }
    else
    {
        for (auto& round : m_tournament)
        {
            for (int fightIndex = 0; fightIndex < fightsPerRound; ++fightIndex)
            {
                const QString& weight = weights.at(fightIndex / 2);
                applyWeight(round->at(fightIndex), weight);
            }
        }
    }

    for (auto const& model : m_models)
    {
        model->SetDataChanged();
    }
}

} // namespace Ipponboard
