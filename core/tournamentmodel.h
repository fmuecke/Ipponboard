#ifndef BASE__TOURNAMENTMODEL_H_
#define BASE__TOURNAMENTMODEL_H_

#include <QAbstractTableModel>
#include <QLineEdit>
#include "tournament.h"

class TournamentModel : public QAbstractTableModel
{
    Q_OBJECT
public:

    enum EColumns
    {
        eCol_weight = 0,
        eCol_name1,
        eCol_ippon1,
        eCol_wazaari1,
        eCol_yuko1,
        eCol_shido1,
        eCol_hansokumake1,
        eCol_won1,
        eCol_score1,
        eCol_name2,
        eCol_ippon2,
        eCol_wazaari2,
        eCol_yuko2,
        eCol_shido2,
        eCol_hansokumake2,
        eCol_won2,
        eCol_score2,
        eCol_time,
        eCol_time_remaining,
        eCol_MAX
    };

    explicit TournamentModel(Ipponboard::PTournamentRound pTournament, QObject* parent = 0);
    virtual ~TournamentModel();

    void SetNumRows(int rows)
    {

        beginResetModel();
        m_nRows = rows;
        endResetModel();
    }
    int GetNumRows() const { return m_nRows; }

    /* QAbstractTableModel (required) */
    int rowCount(const QModelIndex& parent) const;
    int columnCount(const QModelIndex& parent) const;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);
    Qt::ItemFlags flags(const QModelIndex& index) const;
    /* QAbstractTableModel (optional) */
    QSize span(const QModelIndex& index) const;

    void SetExternalDisplays(QLineEdit* pEditWins, QLineEdit* pEditScore)
    {
        m_pEditWins = pEditWins;
        m_pEditScore = pEditScore;
    }
    void SetIntermediateModel(TournamentModel* pModel)
    { m_pIntermediateModel = pModel; }

    void SetDataChanged();

    std::pair<unsigned, unsigned> GetTotalWins() const;
    std::pair<unsigned, unsigned> GetTotalScore() const;

private slots:

private:
    Ipponboard::PTournamentRound m_pTournamentRound;
    TournamentModel* m_pIntermediateModel;
    int m_nRows;
    QString m_HeaderData[eCol_MAX];
    int m_HeaderSizes[eCol_MAX];
    QLineEdit* m_pEditWins;
    QLineEdit* m_pEditScore;
};

typedef std::shared_ptr<TournamentModel> PTournamentModel;

#endif  // BASE__TOURNAMENTMODEL_H_
