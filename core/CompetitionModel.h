// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__TOURNAMENTMODEL_H_
#define BASE__TOURNAMENTMODEL_H_

#include "Competition.h"

#include <QAbstractTableModel>
#include <QLineEdit>

class CompetitionModel : public QAbstractTableModel
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
        eCol_time_remaining,
        eCol_time,
        eCol_MAX
    };

    explicit CompetitionModel(Ipponboard::PCompetitionRound pCompetition, QObject* parent = 0);
    virtual ~CompetitionModel();

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
    void SetIntermediateModel(CompetitionModel* pModel) { m_pIntermediateModel = pModel; }

    void SetDataChanged();

    std::pair<unsigned, unsigned> GetTotalWins() const;
    std::pair<unsigned, unsigned> GetTotalScore() const;

  private slots:

  private:
    static QString display_string(bool isFightSaved, int number);

    Ipponboard::PCompetitionRound m_pCompetitionRound;
    CompetitionModel* m_pIntermediateModel;
    int m_nRows;
    QString m_HeaderData[eCol_MAX];
    int m_HeaderSizes[eCol_MAX];
    QLineEdit* m_pEditWins;
    QLineEdit* m_pEditScore;
};

typedef std::shared_ptr<CompetitionModel> PCompetitionModel;

#endif // BASE__TOURNAMENTMODEL_H_
