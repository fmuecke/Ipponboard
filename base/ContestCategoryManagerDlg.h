// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__FIGHTCATEGORYMANAGERDLG_H_
#define BASE__FIGHTCATEGORYMANAGERDLG_H_

#include "ContestCategoryManager.h"

#include <QDialog>
#include <QTreeWidgetItem>

// forwards
namespace Ui
{
class ContestCategoryManagerDlg;
}
//class WeightClassManager;
class QListWidgetItem;

class ContestCategoryManagerDlg : public QDialog
{
    Q_OBJECT

  public:
    explicit ContestCategoryManagerDlg(Ipponboard::ContestCategoryMgr::Ptr pMgr,
                                       QWidget* parent = nullptr);

    virtual ~ContestCategoryManagerDlg();

  protected:
    void changeEvent(QEvent* e);

  private:
    enum
    {
        eColumn_Name = 0,
        eColumn_Time,
        eColumn_GS,
        eColumn_Weights
    };
    void load_values();

    Ui::ContestCategoryManagerDlg* ui;
    Ipponboard::ContestCategoryMgr::Ptr m_pClassMgr;
    //Ipponboard::WeightClass m_currentClass;

  private slots:
    void on_treeWidget_classes_itemChanged(QTreeWidgetItem* item, int column);
    void on_pushButton_remove_pressed();
    void on_pushButton_add_pressed();
};

#endif // BASE__FIGHTCATEGORYMANAGERDLG_H_
