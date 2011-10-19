#ifndef BASE__FIGHTCATEGORYMANAGERDLG_H_
#define BASE__FIGHTCATEGORYMANAGERDLG_H_

#include <QDialog>
#include <QTreeWidgetItem>
#include "fightcategorymanager.h"

// forwards
namespace Ui { class FightCategoryManagerDlg; }
//class WeightClassManager;
class QListWidgetItem;

class FightCategoryManagerDlg : public QDialog
{
	Q_OBJECT

public:
	explicit FightCategoryManagerDlg(
			Ipponboard::FightCategoryMgr* mgr,
			QWidget* parent = 0 );

	virtual ~FightCategoryManagerDlg();

protected:
	void changeEvent(QEvent *e);

private:
	enum { eColumn_Name=0, eColumn_Time, eColumn_GS, eColumn_Weights };
	void load_values();

	Ui::FightCategoryManagerDlg *ui;
	Ipponboard::FightCategoryMgr* m_pClassMgr;
	std::string m_originalClasses;
	//Ipponboard::WeightClass m_currentClass;

private slots:
	void on_treeWidget_classes_itemChanged(QTreeWidgetItem* item, int column);
	void on_pushButton_down_pressed();
	void on_pushButton_up_pressed();
	void on_buttonBox_rejected();
	void on_buttonBox_accepted();
	void on_pushButton_remove_pressed();
	void on_pushButton_add_pressed();
};

#endif  // BASE__FIGHTCATEGORYMANAGERDLG_H_
