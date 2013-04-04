#include "TournamentModel.h"
#include "Enums.h"
#include "../util/helpers.hpp"

#include <QSize>

//=========================================================
TournamentModel::TournamentModel(Ipponboard::PTournamentRound pTournament, QObject* parent)
//=========================================================
	: QAbstractTableModel(parent)
	, m_pTournamentRound(pTournament)
	, m_pIntermediateModel(0)
	, m_nRows(pTournament->size())
	//, m_HeaderData()
	//, m_HeaderSizes()
	, m_pEditWins(0)
	, m_pEditScore(0)
{
	m_HeaderData[eCol_weight]		= tr("Weight");
	m_HeaderData[eCol_name1]		= tr("Firstname Lastname");
	m_HeaderData[eCol_yuko1]		= "Y";
	m_HeaderData[eCol_wazaari1]		= "W";
	m_HeaderData[eCol_ippon1]		= "I";
	m_HeaderData[eCol_shido1]		= "S";
	m_HeaderData[eCol_hansokumake1]	= "H";
	m_HeaderData[eCol_won1]			= tr("Won");
	m_HeaderData[eCol_score1]		= tr("Score");
	m_HeaderData[eCol_name2]		= tr("Firstname Lastname");
	m_HeaderData[eCol_yuko2]		= "Y";
	m_HeaderData[eCol_wazaari2]		= "W";
	m_HeaderData[eCol_ippon2]		= "I";
	m_HeaderData[eCol_shido2]		= "S";
	m_HeaderData[eCol_hansokumake2] = "H";
	m_HeaderData[eCol_won2]			= tr("Won");
	m_HeaderData[eCol_score2]		= tr("Score");
	m_HeaderData[eCol_time_remaining] = tr("Remaining"); // ⌚⌛ 
	m_HeaderData[eCol_time]			= tr("Time");  // "\u23F1" "⌚⌛,

	// Judo: 柔道 \uE69F94 \uE98193

	m_HeaderSizes[eCol_weight]		= 50;
	m_HeaderSizes[eCol_name1]		= 180;
	m_HeaderSizes[eCol_yuko1]		= 20;
	m_HeaderSizes[eCol_wazaari1]	= 20;
	m_HeaderSizes[eCol_ippon1]		= 20;
	m_HeaderSizes[eCol_shido1]		= 20;
	m_HeaderSizes[eCol_hansokumake1] = 20;
	m_HeaderSizes[eCol_won1]		= 40;
	m_HeaderSizes[eCol_score1]		= 40;
	m_HeaderSizes[eCol_name2]		= 180;
	m_HeaderSizes[eCol_yuko2]		= 20;
	m_HeaderSizes[eCol_wazaari2]	= 20;
	m_HeaderSizes[eCol_ippon2]		= 20;
	m_HeaderSizes[eCol_shido2]		= 20;
	m_HeaderSizes[eCol_hansokumake2] = 20;
	m_HeaderSizes[eCol_won2]		= 40;
	m_HeaderSizes[eCol_score2]		= 40;
	m_HeaderSizes[eCol_time_remaining] = 50;
	m_HeaderSizes[eCol_time]		= 50;
}

//=========================================================
TournamentModel::~TournamentModel()
//=========================================================
{
}

//=========================================================
//QModelIndex TournamentModel::index(int row,
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
//QModelIndex TournamentModel::parent(const QModelIndex& /*child*/) const
//=========================================================
//{
//	return QModelIndex();
//}

//=========================================================
int TournamentModel::rowCount(const QModelIndex& parent) const
//=========================================================
{
	return (parent.isValid() && parent.column() != 0) ? 0 : m_nRows;
}

//=========================================================
int TournamentModel::columnCount(const QModelIndex& parent) const
//=========================================================
{
	return parent.isValid() ? 0 : eCol_MAX;
}

//=========================================================
QVariant TournamentModel::data(const QModelIndex& index, int role) const
//=========================================================
{
	Q_ASSERT(m_pTournamentRound);

	if (!index.isValid())
		return QVariant();

	switch (role)
	{
	case Qt::EditRole:
	case Qt::DisplayRole:
		if (index.row() < m_nRows &&
				index.column() >= 0 &&
				index.column() < eCol_MAX)
		{
			const int row = index.row();

			const Ipponboard::Fight& fight = m_pTournamentRound->at(row);

			switch (index.column())
			{
			case eCol_weight:
				return fight.weight;

			case eCol_name1:
				return fight.fighters[0].name;

			case eCol_yuko1:
				return display_string(fight.is_saved, fight.scores[0].Yuko());

			case eCol_wazaari1:
				return display_string(fight.is_saved, fight.scores[0].Wazaari());

			case eCol_ippon1:
				return fight.scores[0].Ippon() ? "1" : "";

			case eCol_hansokumake1:
				return fight.scores[0].Hansokumake() ? "1" : "";

			case eCol_shido1:
				return display_string(fight.is_saved, fight.scores[0].Shido());

			case eCol_won1:
				return display_string(fight.is_saved, fight.HasWon(Ipponboard::eFighter1));

			case eCol_score1:
				return display_string(fight.is_saved, fight.ScorePoints(Ipponboard::eFighter1));

			case eCol_name2:
				return fight.fighters[1].name;

			case eCol_yuko2:
				return display_string(fight.is_saved, fight.scores[1].Yuko());

			case eCol_wazaari2:
				return display_string(fight.is_saved, fight.scores[1].Wazaari());

			case eCol_ippon2:
				return fight.scores[1].Ippon() ? "1" : "";

			case eCol_hansokumake2:
				return fight.scores[1].Hansokumake() ? "1" : "";

			case eCol_shido2:
				return display_string(fight.is_saved, fight.scores[1].Shido());

			case eCol_won2:
				return display_string(fight.is_saved, fight.HasWon(Ipponboard::eFighter2));

			case eCol_score2:
				return display_string(fight.is_saved, fight.ScorePoints(Ipponboard::eFighter2));

			case eCol_time_remaining:
				{
					// get time display
					return fight.GetTimeRemaining();
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

					m_pEditWins->setText(QString::number(wins.first) + " : " + QString::number(wins.second));
					m_pEditScore->setText(QString::number(score.first) + " : " + QString::number(score.second));

 					// get time display
					QString ret = fight.GetTimeFaught();
					if (ret == QString("0:00") && !fight.is_saved)
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
			if (index.column() < static_cast<int>(sizeof(m_HeaderSizes)) &&
					index.column() > 0)
			{
				return QSize(m_HeaderSizes[index.column()], 20);
			}

			break;
		}

	case Qt::TextAlignmentRole:
		{
			if (eCol_name1 != index.column() &&
					eCol_name2 != index.column())
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
QVariant TournamentModel::headerData(
	int section,
	Qt::Orientation orientation,
	int role) const
//=========================================================
{
	if (role == Qt::DisplayRole)
	{
		if (Qt::Vertical == orientation)
			return QString::number(section + 1);

		if (section < fmu::ArrayLength(m_HeaderData))
			return m_HeaderData[section];
	}

//	if (role == Qt::SizeHintRole && orientation == Qt::Horizontal)
//	{
//		if (section < sizeof(m_HeaderSizes) && section > 0)
//			return QSize(m_HeaderSizes[section], 20);
//	}

	return QAbstractItemModel::headerData(section, orientation, role);
}

//=========================================================
bool TournamentModel::setData(const QModelIndex& index,
							  const QVariant& value,
							  int role)
//=========================================================
{
	if (!index.isValid() ||
			(flags(index) & Qt::ItemIsEditable) == 0 ||
			role != Qt::EditRole)
	{
		return false;
	}

	bool result(false);

	if (index.row() < m_nRows &&
			index.column() >= 0 &&
			index.column() < eCol_MAX)
	{
		const int row = index.row();

		Ipponboard::Fight& fight = m_pTournamentRound->at(row);

		switch (index.column())
		{
		case eCol_weight:
			fight.weight = value.toString();
			result = true;
			break;

		case eCol_name1:
			fight.fighters[0].name = value.toString();
			result = true;
			break;

		case eCol_yuko1:
			fight.scores[0].OverwriteValue(Ipponboard::ePoint_Yuko, value.toInt());
			result = true;
			break;

		case eCol_wazaari1:
			fight.scores[0].OverwriteValue(Ipponboard::ePoint_Wazaari, value.toInt());
			result = true;
			break;

		case eCol_ippon1:
			fight.scores[0].OverwriteValue(Ipponboard::ePoint_Ippon, value.toInt());
			result = true;
			break;

		case eCol_hansokumake1:
			fight.scores[0].OverwriteValue(Ipponboard::ePoint_Hansokumake, value.toInt());
			break;

		case eCol_shido1:
			fight.scores[0].OverwriteValue(Ipponboard::ePoint_Shido, value.toInt());
			break;

		case eCol_won1:
		case eCol_score1:
			// disabled!
			break;

		case eCol_name2:
			fight.fighters[1].name = value.toString();
			result = true;
			break;

		case eCol_yuko2:
			fight.scores[1].OverwriteValue(Ipponboard::ePoint_Yuko, value.toInt());
			result = true;
			break;

		case eCol_wazaari2:
			fight.scores[1].OverwriteValue(Ipponboard::ePoint_Wazaari, value.toInt());
			result = true;
			break;

		case eCol_ippon2:
			fight.scores[1].OverwriteValue(Ipponboard::ePoint_Ippon, value.toInt());
			result = true;
			break;

		case eCol_hansokumake2:
			fight.scores[1].OverwriteValue(Ipponboard::ePoint_Hansokumake, value.toInt());
			result = true;
			break;

		case eCol_shido2:
			fight.scores[1].OverwriteValue(Ipponboard::ePoint_Shido, value.toInt());
			result = true;
			break;

		case eCol_won2:
		case eCol_score2:
			// disabled!
			break;

		case eCol_time_remaining:
			{
				const QString s(value.toString());

				if (s.at(1) == ':')
				{
					int seconds = s.left(1).toUInt() * 60;
					seconds += s.right(s.length() - 2).toUInt();
					fight.time_in_seconds = 
						fight.max_time_in_seconds - seconds;

					result = true;
				}
			}

		case eCol_time:
			{
				const QString s(value.toString());

				if (s.at(1) == ':')
				{
					int seconds = s.left(1).toUInt() * 60;
					seconds += s.right(s.length() - 2).toUInt();
					fight.time_in_seconds = seconds;

					result = true;
				}
			}

		default:
			break;
		}
	}

	if (result)
		emit dataChanged(index, index);

	return result;
}

//=========================================================
Qt::ItemFlags TournamentModel::flags(const QModelIndex& index) const
//=========================================================
{
	if (!index.isValid())
		return 0;

	if (index.column() == eCol_won1 ||
			index.column() == eCol_won2 ||
			index.column() == eCol_score1 ||
			index.column() == eCol_score2)
	{
		return (Qt::ItemIsSelectable | Qt::ItemIsEnabled);
	}

	return (Qt::ItemIsEditable | Qt::ItemIsSelectable | Qt::ItemIsEnabled);
}

//=========================================================
QSize TournamentModel::span(const QModelIndex& index) const
//=========================================================
{
	if (index.column() == 2 && index.row() == 5)
		return QSize(1, 2);

	return QSize();
}

//=========================================================
void TournamentModel::SetDataChanged()
//=========================================================
{
	QModelIndex idxStart = this->index(0, 0);
	QModelIndex idxEnd = this->index(eCol_MAX - 1, m_nRows - 1);

	emit dataChanged(idxStart, idxEnd);
}

//=========================================================
std::pair<unsigned, unsigned> TournamentModel::GetTotalWins() const
//=========================================================
{
	int wins1(0);
	int wins2(0);

	for (int i(0); i < m_nRows; ++i)
	{
		wins1 += m_pTournamentRound->at(i).HasWon(Ipponboard::eFighter1);
		wins2 += m_pTournamentRound->at(i).HasWon(Ipponboard::eFighter2);
	}

	return std::make_pair(wins1, wins2);
}

//=========================================================
std::pair<unsigned, unsigned> TournamentModel::GetTotalScore() const
//=========================================================
{
	int score1(0);
	int score2(0);

	for (int i(0); i < m_nRows; ++i)
	{
		score1 += m_pTournamentRound->at(i).ScorePoints(Ipponboard::eFighter1);
		score2 += m_pTournamentRound->at(i).ScorePoints(Ipponboard::eFighter2);
	}

	return std::make_pair(score1, score2);
}

QString TournamentModel::display_string(bool isFightSaved, int number)
{
	if (isFightSaved || number != 0)
	{
		return QString::number(number);
	}

	return QString();
}