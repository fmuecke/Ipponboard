// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "CompetitionModel.h"

#include "../util/array_helpers.h"
#include "../util/helpers.hpp"
#include "Enums.h"

#include <QSize>

using namespace Ipponboard;

enum
{
    eDefaultRowHight = 20
};

//=========================================================
CompetitionModel::CompetitionModel(Ipponboard::PContestRound pCompetition, QObject* parent)
    //=========================================================
    : QAbstractTableModel(parent),
      m_pCompetitionRound(pCompetition),
      m_pIntermediateModel(0),
      m_nRows(pCompetition->size())
      //, m_HeaderData()
      //, m_HeaderSizes()
      ,
      m_pEditWins(0),
      m_pEditScore(0)
{
    m_HeaderData[eCol_weight] = tr("Weight");
    m_HeaderData[eCol_name1] = tr("Firstname Lastname");
    m_HeaderData[eCol_yuko1] = "Y";
    m_HeaderData[eCol_wazaari1] = "W";
    m_HeaderData[eCol_ippon1] = "I";
    m_HeaderData[eCol_shido1] = "S";
    m_HeaderData[eCol_hansokumake1] = "H";
    m_HeaderData[eCol_won1] = tr("Won");
    m_HeaderData[eCol_score1] = tr("Score");
    m_HeaderData[eCol_name2] = tr("Firstname Lastname");
    m_HeaderData[eCol_yuko2] = "Y";
    m_HeaderData[eCol_wazaari2] = "W";
    m_HeaderData[eCol_ippon2] = "I";
    m_HeaderData[eCol_shido2] = "S";
    m_HeaderData[eCol_hansokumake2] = "H";
    m_HeaderData[eCol_won2] = tr("Won");
    m_HeaderData[eCol_score2] = tr("Score");
    m_HeaderData[eCol_time_remaining] = tr("Remaining"); // ⌚⌛
    m_HeaderData[eCol_time] = tr("Time");                // "\u23F1" "⌚⌛,

    // Judo: 柔道 \uE69F94 \uE98193

    m_HeaderSizes[eCol_weight] = 50;
    m_HeaderSizes[eCol_name1] = 150;
    m_HeaderSizes[eCol_yuko1] = 20;
    m_HeaderSizes[eCol_wazaari1] = 20;
    m_HeaderSizes[eCol_ippon1] = 20;
    m_HeaderSizes[eCol_shido1] = 20;
    m_HeaderSizes[eCol_hansokumake1] = 20;
    m_HeaderSizes[eCol_won1] = 40;
    m_HeaderSizes[eCol_score1] = 40;
    m_HeaderSizes[eCol_name2] = 150;
    m_HeaderSizes[eCol_yuko2] = 20;
    m_HeaderSizes[eCol_wazaari2] = 20;
    m_HeaderSizes[eCol_ippon2] = 20;
    m_HeaderSizes[eCol_shido2] = 20;
    m_HeaderSizes[eCol_hansokumake2] = 20;
    m_HeaderSizes[eCol_won2] = 40;
    m_HeaderSizes[eCol_score2] = 40;
    m_HeaderSizes[eCol_time_remaining] = 50;
    m_HeaderSizes[eCol_time] = 50;
}

//=========================================================
CompetitionModel::~CompetitionModel()
//=========================================================
{
}

//=========================================================
//QModelIndex CompetitionModel::index(int row,
//								   int column,
//								   const QModelIndex& /*parent*/) const
//=========================================================
//{
//	if (row < m_nRows && row >= 0 &&
//			column < eCol_MAX && column >= 0)
//	{
//		return createIndex(row, column, 0); //TODO: 3rd param is ptr to item
//	}
//
//	return QModelIndex();
//}

//=========================================================
//QModelIndex CompetitionModel::parent(const QModelIndex& /*child*/) const
//=========================================================
//{
//	return QModelIndex();
//}

//=========================================================
int CompetitionModel::rowCount(const QModelIndex& parent) const
//=========================================================
{
    return (parent.isValid() && parent.column() != 0) ? 0 : m_nRows;
}

//=========================================================
int CompetitionModel::columnCount(const QModelIndex& parent) const
//=========================================================
{
    return parent.isValid() ? 0 : eCol_MAX;
}

//=========================================================
QVariant CompetitionModel::data(const QModelIndex& index, int role) const
//=========================================================
{
    Q_ASSERT(m_pCompetitionRound);

    if (!index.isValid())
        return QVariant();

    switch (role)
    {
    case Qt::EditRole:
    case Qt::DisplayRole:
        if (index.row() < m_nRows && index.column() >= 0 && index.column() < eCol_MAX)
        {
            const int row = index.row();

            const Ipponboard::Contest& contest = m_pCompetitionRound->at(row);

            switch (index.column())
            {
            case eCol_weight:
                return contest.weight;

            case eCol_name1:
                return contest.GetAthlete(Ipponboard::ContestSide::SideA).name;

            case eCol_yuko1:
                return display_string(contest.is_saved,
                                      contest.GetScore(Ipponboard::ContestSide::SideA).Yuko());

            case eCol_wazaari1:
                return display_string(contest.is_saved,
                                      contest.GetScore(Ipponboard::ContestSide::SideA).Wazaari());

            case eCol_ippon1:
                return contest.GetScore(Ipponboard::ContestSide::SideA).Ippon() ? "1" : "";

            case eCol_hansokumake1:
                return contest.GetScore(Ipponboard::ContestSide::SideA).Hansokumake() ? "1" : "";

            case eCol_shido1:
                return display_string(contest.is_saved,
                                      contest.GetScore(Ipponboard::ContestSide::SideA).Shido());

            case eCol_won1:
                return display_string(contest.is_saved,
                                      contest.HasWon(Ipponboard::ContestSide::SideA));

            case eCol_score1:
                return display_string(contest.is_saved,
                                      contest.GetScorePoints(Ipponboard::ContestSide::SideA));

            case eCol_name2:
                return contest.GetAthlete(Ipponboard::ContestSide::SideB).name;

            case eCol_yuko2:
                return display_string(contest.is_saved,
                                      contest.GetScore(Ipponboard::ContestSide::SideB).Yuko());

            case eCol_wazaari2:
                return display_string(contest.is_saved,
                                      contest.GetScore(Ipponboard::ContestSide::SideB).Wazaari());

            case eCol_ippon2:
                return contest.GetScore(Ipponboard::ContestSide::SideB).Ippon() ? "1" : "";

            case eCol_hansokumake2:
                return contest.GetScore(Ipponboard::ContestSide::SideB).Hansokumake() ? "1" : "";

            case eCol_shido2:
                return display_string(contest.is_saved,
                                      contest.GetScore(Ipponboard::ContestSide::SideB).Shido());

            case eCol_won2:
                return display_string(contest.is_saved,
                                      contest.HasWon(Ipponboard::ContestSide::SideB));

            case eCol_score2:
                return display_string(contest.is_saved,
                                      contest.GetScorePoints(Ipponboard::ContestSide::SideB));

            case eCol_time_remaining:
            {
                // get time display
                return contest.GetTimeRemainingString();
            }

            case eCol_time:
            {
                // we do get and set the total score display here
                Q_ASSERT(m_pEditWins && m_pEditScore);
                std::pair<unsigned, unsigned> wins = GetTotalWins();
                std::pair<unsigned, unsigned> score = GetTotalScore();

                if (m_pIntermediateModel)
                {
                    std::pair<unsigned, unsigned> intermediate_wins =
                        m_pIntermediateModel->GetTotalWins();
                    std::pair<unsigned, unsigned> intermediate_score =
                        m_pIntermediateModel->GetTotalScore();

                    wins.first += intermediate_wins.first;
                    wins.second += intermediate_wins.second;
                    score.first += intermediate_score.first;
                    score.second += intermediate_score.second;
                }

                m_pEditWins->setText(QString::number(wins.first) + " : " +
                                     QString::number(wins.second));
                m_pEditScore->setText(QString::number(score.first) + " : " +
                                      QString::number(score.second));

                // get time display
                QString ret = contest.GetTotalTimeElapsedString();

                if (ret == QString("0:00") && !contest.is_saved)
                {
                    return QString();
                }

                return ret;
            }

            default:
                break;
            }

            return QString();
        }

    case Qt::SizeHintRole:
    {
        if (index.column() < static_cast<int>(sizeof(m_HeaderSizes)) && index.column() > 0)
        {
            return QSize(m_HeaderSizes[index.column()], eDefaultRowHight);
        }

        break;
    }

    case Qt::TextAlignmentRole:
    {
        if (eCol_name1 != index.column() && eCol_name2 != index.column())
        {
            return Qt::AlignCenter;
        }

        break;
    }

    default:
        break;
    }

    return QVariant();
}

//=========================================================
QVariant CompetitionModel::headerData(int section, Qt::Orientation orientation, int role) const
//=========================================================
{
    if (role == Qt::DisplayRole)
    {
        if (Qt::Vertical == orientation)
            return QString::number(section + 1);

        if (section < static_cast<int>(fm::array_size(m_HeaderData)))
            return m_HeaderData[section];
    }

    //	if (role == Qt::SizeHintRole && orientation == Qt::Horizontal)
    //	{
    //		if (section < sizeof(m_HeaderSizes) && section > 0)
    //			return QSize(m_HeaderSizes[section], eDefaultRowHight);
    //	}

    return QAbstractItemModel::headerData(section, orientation, role);
}

//=========================================================
bool CompetitionModel::setData(const QModelIndex& index, const QVariant& value, int role)
//=========================================================
{
    if (!index.isValid() || (flags(index) & Qt::ItemIsEditable) == 0 || role != Qt::EditRole)
    {
        return false;
    }

    bool result(false);

    if (index.row() < m_nRows && index.column() >= 0 && index.column() < eCol_MAX)
    {
        const int row = index.row();

        Ipponboard::Contest& contest = m_pCompetitionRound->at(row);

        switch (index.column())
        {
        case eCol_weight:
            contest.weight = value.toString();
            result = true;
            break;

        case eCol_name1:
            contest.GetAthlete(Ipponboard::ContestSide::SideA).name = value.toString();
            result = true;
            break;

        case eCol_yuko1:
            contest.GetScore(Ipponboard::ContestSide::SideA)
                .SetValue(Ipponboard::Score::Point::Yuko, value.toInt());
            result = true;
            break;

        case eCol_wazaari1:
            contest.GetScore(Ipponboard::ContestSide::SideA)
                .SetValue(Ipponboard::Score::Point::Wazaari, value.toInt());
            result = true;
            break;

        case eCol_ippon1:
            contest.GetScore(Ipponboard::ContestSide::SideA)
                .SetValue(Ipponboard::Score::Point::Ippon, value.toInt());
            result = true;
            break;

        case eCol_hansokumake1:
            contest.GetScore(Ipponboard::ContestSide::SideA)
                .SetValue(Ipponboard::Score::Point::Hansokumake, value.toInt());
            break;

        case eCol_shido1:
            contest.GetScore(Ipponboard::ContestSide::SideA)
                .SetValue(Ipponboard::Score::Point::Shido, value.toInt());
            break;

        case eCol_won1:
        case eCol_score1:
            // disabled!
            break;

        case eCol_name2:
            contest.GetAthlete(Ipponboard::ContestSide::SideB).name = value.toString();
            result = true;
            break;

        case eCol_yuko2:
            contest.GetScore(Ipponboard::ContestSide::SideB)
                .SetValue(Ipponboard::Score::Point::Yuko, value.toInt());
            result = true;
            break;

        case eCol_wazaari2:
            contest.GetScore(Ipponboard::ContestSide::SideB)
                .SetValue(Ipponboard::Score::Point::Wazaari, value.toInt());
            result = true;
            break;

        case eCol_ippon2:
            contest.GetScore(Ipponboard::ContestSide::SideB)
                .SetValue(Ipponboard::Score::Point::Ippon, value.toInt());
            result = true;
            break;

        case eCol_hansokumake2:
            contest.GetScore(Ipponboard::ContestSide::SideB)
                .SetValue(Ipponboard::Score::Point::Hansokumake, value.toInt());
            result = true;
            break;

        case eCol_shido2:
            contest.GetScore(Ipponboard::ContestSide::SideB)
                .SetValue(Ipponboard::Score::Point::Shido, value.toInt());
            result = true;
            break;

        case eCol_won2:
        case eCol_score2:
            // disabled!
            break;

        case eCol_time_remaining:
        {
            break;
        }

        case eCol_time:
        {
            result = contest.SetElapsedFromTotalTime(value.toString());
            break;
        }

        default:
            break;
        }
    }

    if (result)
    {
        emit dataChanged(index, index);
    }

    return result;
}

//=========================================================
Qt::ItemFlags CompetitionModel::flags(const QModelIndex& index) const
//=========================================================
{
    if (!index.isValid())
    {
        return Qt::ItemFlags();
    }

    if (index.column() == eCol_won1 || index.column() == eCol_won2 ||
        index.column() == eCol_score1 || index.column() == eCol_score2 ||
        index.column() == eCol_time_remaining)
    {
        return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
    }

    return (Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

//=========================================================
QSize CompetitionModel::span(const QModelIndex& index) const
//=========================================================
{
    if (index.column() == 2 && index.row() == 5)
    {
        return QSize(1, 2);
    }

    return QSize();
}

//=========================================================
void CompetitionModel::SetDataChanged()
//=========================================================
{
    QModelIndex idxStart = this->index(0, 0);
    QModelIndex idxEnd = this->index(eCol_MAX - 1, m_nRows - 1);

    emit dataChanged(idxStart, idxEnd);
}

//=========================================================
std::pair<unsigned, unsigned> CompetitionModel::GetTotalWins() const
//=========================================================
{
    int wins1(0);
    int wins2(0);

    for (int i(0); i < m_nRows; ++i)
    {
        wins1 += m_pCompetitionRound->at(i).HasWon(Ipponboard::ContestSide::SideA);
        wins2 += m_pCompetitionRound->at(i).HasWon(Ipponboard::ContestSide::SideB);
    }

    return std::make_pair(wins1, wins2);
}

//=========================================================
std::pair<unsigned, unsigned> CompetitionModel::GetTotalScore() const
//=========================================================
{
    int score1(0);
    int score2(0);

    for (int i(0); i < m_nRows; ++i)
    {
        score1 += m_pCompetitionRound->at(i).GetScorePoints(Ipponboard::ContestSide::SideA);
        score2 += m_pCompetitionRound->at(i).GetScorePoints(Ipponboard::ContestSide::SideB);
    }

    return std::make_pair(score1, score2);
}

QString CompetitionModel::display_string(bool isContestSaved, int number)
{
    if (isContestSaved || number != 0)
    {
        return QString::number(number);
    }

    return QString();
}
