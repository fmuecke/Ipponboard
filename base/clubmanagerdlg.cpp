#include "clubmanagerdlg.h"
#include "ui_clubmanagerdlg.h"
#include "clubmanager.h"
#include <QFileDialog>
#include <QMessageBox>

using namespace Ipponboard;

//---------------------------------------------------------
ClubManagerDlg::ClubManagerDlg( ClubManager* mgr, QWidget *parent )
	: QDialog(parent)
	, ui(new Ui::ClubManagerDlg)
	, m_pClubMgr(mgr)
	, m_SelectedClub(-1)
//---------------------------------------------------------
{
	ui->setupUi(this);

	Q_ASSERT(m_pClubMgr);

	// load club data
	if( m_pClubMgr->ClubCount() > 0 )
	{
		// add clubs
		for( int i = 0; i != m_pClubMgr->ClubCount(); ++i )
		{
			Ipponboard::Club club;
			m_pClubMgr->GetClub(i, club);

			QIcon icon( club.logoFile );
			if( icon.isNull() /*icon.availableSizes().empty()*/ )
			{
				icon.addFile( ":/res/emblems/default.png" );
			}
			QListWidgetItem *item = new QListWidgetItem(
					icon, club.ToString(), ui->listWidget_clubs );
			item->setTextAlignment( Qt::AlignHCenter );
			item->setFlags( Qt::ItemIsSelectable | Qt::ItemIsEnabled );
		}
		ui->listWidget_clubs->setCurrentRow(0); //SelectClub_(0);
	}
}

//---------------------------------------------------------
ClubManagerDlg::~ClubManagerDlg()
//---------------------------------------------------------
{
	delete ui;
}

//---------------------------------------------------------
void ClubManagerDlg::changeEvent(QEvent *e)
//---------------------------------------------------------
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

//---------------------------------------------------------
void ClubManagerDlg::SelectClub_( int index )
//---------------------------------------------------------
{
	m_SelectedClub = index;

	if( m_SelectedClub != -1 )
	{
		Ipponboard::Club club;
		if( m_pClubMgr->GetClub(index, club) )
		{
			ui->lineEdit_name->setText(club.name);
			ui->lineEdit_city->setText(club.city);
			ui->lineEdit_homepage->setText(club.homepage);
			ui->plainTextEdit_address->setPlainText(club.address);
		}
	}

	UpdateButtonsAndText_();
}

//---------------------------------------------------------
void ClubManagerDlg::UpdateButtonsAndText_()
//---------------------------------------------------------
{
	if( 0 == m_pClubMgr->ClubCount() )
	{
		ui->pushButton_remove->setEnabled(false);
		ui->pushButton_update->setEnabled(false);
		ui->pushButton_selectLogo->setEnabled(false);

		ui->lineEdit_name->clear();
		ui->lineEdit_city->clear();
		ui->lineEdit_homepage->clear();
		ui->plainTextEdit_address->clear();
		ui->lineEdit_name->setEnabled(false);
		ui->lineEdit_city->setEnabled(false);
		ui->lineEdit_homepage->setEnabled(false);
		ui->plainTextEdit_address->setEnabled(false);
	}
	else
	{
		ui->lineEdit_name->setEnabled(true);
		ui->lineEdit_city->setEnabled(true);
		ui->lineEdit_homepage->setEnabled(true);
		ui->plainTextEdit_address->setEnabled(true);

		ui->pushButton_remove->setEnabled(true);
		ui->pushButton_update->setEnabled(true);
		ui->pushButton_selectLogo->setEnabled(true);
	}
	//ui->listWidget_clubs->setWrapping(true);
}

//---------------------------------------------------------
void ClubManagerDlg::on_pushButton_add_released()
//---------------------------------------------------------
{
	// add empty club
	Ipponboard::Club club;
	club.name = "--> new <--";
	m_pClubMgr->AddClub(club);

	QListWidgetItem *item = new QListWidgetItem(ui->listWidget_clubs);
	item->setIcon(QIcon(":/res/emblems/default.png"));
	item->setText(club.ToString());
	item->setTextAlignment(Qt::AlignHCenter);
	item->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

	ui->listWidget_clubs->setCurrentItem(item);
	//UpdateButtonsAndText_();
}

//---------------------------------------------------------
void ClubManagerDlg::on_pushButton_update_released()
//---------------------------------------------------------
{
	if( m_SelectedClub < 0)
		return;

	Ipponboard::Club club;
	club.name = ui->lineEdit_name->text();
	club.city = ui->lineEdit_city->text();
	club.homepage = ui->lineEdit_homepage->text();
	club.address = ui->plainTextEdit_address->toPlainText();
	// save club
	m_pClubMgr->UpdateClub(m_SelectedClub, club);
	// update club view in list
	ui->listWidget_clubs->item(m_SelectedClub)->setText(club.ToString());
}

//---------------------------------------------------------
void ClubManagerDlg::on_pushButton_remove_released()
//---------------------------------------------------------
{
	if( m_SelectedClub < 0)
		return;

	// takeItem changes current row to row+1 or row-1 (if last item in list is removed) !!
	// --> save row index
	const int currentRow = m_SelectedClub;
	QListWidgetItem* item = ui->listWidget_clubs->takeItem(currentRow);
	delete item;
	m_pClubMgr->RemoveClub( currentRow );
	if( m_pClubMgr->ClubCount() > 0 )
	{
		if( m_pClubMgr->ClubCount() == currentRow )
		{
			// was last element in list
			ui->listWidget_clubs->setCurrentRow(currentRow - 1);
		}
		else
		{
			ui->listWidget_clubs->setCurrentRow(currentRow);
		}
	}
	//UpdateButtonsAndText_();
}

//---------------------------------------------------------
void ClubManagerDlg::on_pushButton_selectLogo_released()
//---------------------------------------------------------
{
	if( m_SelectedClub < 0)
		return;

	QFileDialog::Options options;
	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName(this,
		tr("Select Club Emblem"),
		"",
		tr("PNG files (*.png);;Image files (*.png *.xpm *.jpg)"),
		&selectedFilter,
		options);

	if (!fileName.isEmpty())
	{
		QIcon icon(fileName);
		if(icon.availableSizes().empty())
		{
			QMessageBox::critical(this,
								  tr("Unsupported Image Type"),
								  tr("The specified image could not be loaded!"));
		}
		else
		{
			//TODO: implement own function for logo update
			Ipponboard::Club club;
			m_pClubMgr->GetClub(m_SelectedClub, club);
			club.logoFile = fileName;  //TODO: save logo path relative to working dir!
			m_pClubMgr->UpdateClub(m_SelectedClub, club);
			ui->listWidget_clubs->item(m_SelectedClub)->setIcon(icon);
		}
	}

}

//---------------------------------------------------------
void ClubManagerDlg::on_listWidget_clubs_currentRowChanged(int row)
//---------------------------------------------------------
{
	SelectClub_(row);
}
