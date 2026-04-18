// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#include <QDialog>
#include <QString>
#include <QTreeWidgetItem>

// forwards
namespace Ui
{
class AthleteManagerDlg;
}
namespace Ipponboard
{
class Athlete;
class AthleteManager;
} // namespace Ipponboard
class QListWidgetItem;

class AthleteManagerDlg : public QDialog
{
    Q_OBJECT

  public:
    enum EColumn
    {
        eColumn_club = 0,
        //eColumn_category,
        eColumn_firstName,
        eColumn_lastName,
        eColumn_weight,
        eColumn_MAX
    };

    explicit AthleteManagerDlg(Ipponboard::AthleteManager& manager, QWidget* parent = nullptr);

    virtual ~AthleteManagerDlg();

    void SetFilter(EColumn column, QString const& value);

  protected:
    void changeEvent(QEvent* e);

  private slots:
    void on_treeWidget_fighters_itemChanged(QTreeWidgetItem* item, int column);
    void on_treeWidget_fighters_itemClicked(QTreeWidgetItem* item, int column);
    void on_pushButton_import_pressed();
    void on_pushButton_export_pressed();
    //void on_buttonBox_rejected();
    //void on_buttonBox_accepted();
    void on_pushButton_remove_pressed();
    void on_pushButton_add_pressed();
    void on_pushButton_settings_pressed();

  private:
    void populate_view();

    Ui::AthleteManagerDlg* ui;
    Ipponboard::AthleteManager& m_manager;
    QString m_tmpData;
    std::pair<EColumn, QString> m_filter;
    QString m_formatStr;
};
