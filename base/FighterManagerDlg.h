// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
#ifndef BASE_FIGHTERMANAGERDLG_H
#define BASE_FIGHTERMANAGERDLG_H

#include <QDialog>
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
    explicit FighterManagerDlg(
            Ipponboard::FighterManager& manager,
            QWidget* parent = nullptr);

    virtual ~FighterManagerDlg();

protected:
	void changeEvent(QEvent* e);
    
private:
	enum 
	{
        eColumn_firstName = 0,
		eColumn_lastName, 
        eColumn_club,
        eColumn_weight,
        eColumn_category
    };

    void populate_view();

	Ui::FighterManagerDlg* ui;
    Ipponboard::FighterManager& m_manager;

private slots:
	void on_treeWidget_fighters_itemChanged(QTreeWidgetItem* item, int column);
	void on_pushButton_import_pressed();
	void on_pushButton_export_pressed();
    //void on_buttonBox_rejected();
    //void on_buttonBox_accepted();
    void on_pushButton_remove_pressed();
	void on_pushButton_add_pressed();
    void on_pushButton_settings_pressed();
};

#endif // BASE_FIGHTERMANAGERDLG_H
