// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__FIGHTCATEGORYMANAGERDLG_H_
#define BASE__FIGHTCATEGORYMANAGERDLG_H_

#include "FightCategoryManager.h"

#include <QDialog>
#include <QTreeWidgetItem>

// forwards
namespace Ui { class FightCategoryManagerDlg; }
//class WeightClassManager;
class QListWidgetItem;

class FightCategoryManagerDlg : public QDialog
{
	Q_OBJECT

public:
	explicit FightCategoryManagerDlg(
		Ipponboard::FightCategoryMgr::Ptr pMgr,
		QWidget* parent = nullptr);

	virtual ~FightCategoryManagerDlg();

	virtual void accept() override;
	virtual void reject() override;

protected:
	void changeEvent(QEvent* e);

private:
	enum { eColumn_Name = 0, eColumn_Time, eColumn_GS, eColumn_Weights };
	void load_values();
	bool isModifiedData {false};
	void close_dialog();

	Ui::FightCategoryManagerDlg* ui;
	Ipponboard::FightCategoryMgr::Ptr m_pClassMgr;
	//Ipponboard::WeightClass m_currentClass;

private slots:
	void on_treeWidget_classes_itemChanged(QTreeWidgetItem* item, int column);
	void on_pushButton_remove_pressed();
	void on_pushButton_add_pressed();
};

#endif  // BASE__FIGHTCATEGORYMANAGERDLG_H_
