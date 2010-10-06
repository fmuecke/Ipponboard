#ifndef CLUBMANAGERDLG_H
#define CLUBMANAGERDLG_H

#include <QDialog>

// forwards
namespace Ui { class ClubManagerDlg; }
namespace Ipponboard { class ClubManager; }
class QListWidgetItem;

class ClubManagerDlg : public QDialog
{
	Q_OBJECT
public:
	ClubManagerDlg( Ipponboard::ClubManager* mgr, QWidget *parent = 0 );
	~ClubManagerDlg();

protected:
	void changeEvent(QEvent *e);

private:
	void SelectClub_( int index );
	void UpdateButtonsAndText_();

	Ui::ClubManagerDlg *ui;
	Ipponboard::ClubManager* m_pClubMgr;
	int m_SelectedClub;

private slots:
	void on_listWidget_clubs_currentRowChanged(int);
	void on_pushButton_selectLogo_released();
	void on_pushButton_remove_released();
	void on_pushButton_update_released();
	void on_pushButton_add_released();
};

#endif // CLUBMANAGERDLG_H
