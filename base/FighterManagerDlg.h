// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
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
        eColumn_lastName,
        eColumn_firstName,
        eColumn_year,
        eColumn_nation,
        eColumn_category,
        eColumn_weight,
		eColumn_MAX
	};

	explicit FighterManagerDlg(
		Ipponboard::FighterManager& manager,
        QStringList const& clubs,
		QWidget* parent = nullptr);

	virtual ~FighterManagerDlg();

	void SetFilter(EColumn column, QString const& value);

protected:
	void changeEvent(QEvent* e);


private slots:
	void on_treeWidget_fighters_itemChanged(QTreeWidgetItem* item, int column);
	void on_treeWidget_fighters_itemClicked(QTreeWidgetItem* item, int column);
    void on_treeWidget_fighters_itemSelectionChanged();
	void on_pushButton_import_pressed();
	void on_pushButton_export_pressed();
	void on_pushButton_remove_pressed();
	void on_pushButton_add_pressed();

private:
	void populate_view();

	Ui::FighterManagerDlg* ui;
	Ipponboard::FighterManager& m_manager;
	QString m_tmpData;
	std::pair<EColumn, QString> m_filter;
	QString m_formatStr;
    QStringList m_clubs;
};

#endif // BASE_FIGHTERMANAGERDLG_H
