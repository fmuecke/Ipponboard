// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE_FIGHTERMANAGERDLG_H
#define BASE_FIGHTERMANAGERDLG_H

#include <QDialog>
#include <QString>
#include <QTreeWidgetItem>

// forwards
namespace Ui { class FighterManagerDlg; }
namespace Ipponboard
{
class Fighter;
class FighterManager;
}
class QListWidgetItem;


class FighterManagerDlg : public QDialog
{
	Q_OBJECT

public:
	enum EColumn
	{
		eColumn_club = 0,
		eColumn_category,
		eColumn_firstName,
		eColumn_lastName,
		eColumn_weight,
		eColumn_MAX
	};

	explicit FighterManagerDlg(
		Ipponboard::FighterManager& manager,
        QWidget* parent = nullptr);

	virtual ~FighterManagerDlg();

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
    void on_toolButton_openFolder_pressed();
    void on_toolButton_reload_pressed();
    void on_toolButton_save_pressed();

private:
	void populate_view();
	Ipponboard::Fighter itemToFighter(QTreeWidgetItem* pItem) const;
	QStringList fighterToItemData(Ipponboard::Fighter) const;

	Ui::FighterManagerDlg* ui;
	Ipponboard::FighterManager& m_manager;
	QString m_currentCellData;
	std::pair<EColumn, QString> m_filter;
};

#endif // BASE_FIGHTERMANAGERDLG_H
