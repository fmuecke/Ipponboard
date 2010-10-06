#ifndef CLASSMANAGERDLG_H
#define CLASSMANAGERDLG_H

#include <QDialog>
#include "classmanager.h"

// forwards
namespace Ui { class ClassManagerDlg; }
//namespace Ipponboard { class TournamentClassManager; }
class QListWidgetItem;

class ClassManagerDlg : public QDialog
{
	Q_OBJECT

public:
	explicit ClassManagerDlg(
			Ipponboard::TournamentClassManager* mgr, QWidget* parent = 0 );
	~ClassManagerDlg();

protected:
	void changeEvent(QEvent *e);

private:
	void loadValues_(QString const& name);

	Ui::ClassManagerDlg *ui;
	Ipponboard::TournamentClassManager* m_pClassMgr;
	std::string m_originalClasses;
	Ipponboard::TournamentClass m_currentClass;

private slots:
	void on_pushButton_down_pressed();
	void on_pushButton_up_pressed();
	void on_comboBox_class_currentIndexChanged(int index);
	void on_timeEdit_goldenScore_timeChanged(QTime time);
	void on_timeEdit_round_timeChanged(QTime time);
	void on_pushButton_remove_weight_pressed();
	void on_pushButton_add_weight_pressed();
	void on_buttonBox_rejected();
	void on_buttonBox_accepted();
	void on_pushButton_remove_pressed();
	void on_pushButton_add_pressed();
};

#endif // CLASSMANAGERDLG_H
