#include "classmanagerdlg.h"
#include "ui_classmanagerdlg.h"
#include <QInputDialog>
#include <QMessageBox>

using namespace Ipponboard;

//---------------------------------------------------------
ClassManagerDlg::ClassManagerDlg(
		Ipponboard::WeightClassManager* mgr,
		QWidget *parent )
	: QDialog(parent)
	, ui(new Ui::ClassManagerDlg)
	, m_pClassMgr(mgr)
	, m_currentClass(WeightClassManager::str_defaultClass)
//---------------------------------------------------------
{
	ui->setupUi(this);

	Q_ASSERT(m_pClassMgr);


	m_originalClasses = m_pClassMgr->ClassesToString();

	for( int i(0); i<m_pClassMgr->ClassCount(); ++i )
	{
		if( !m_pClassMgr->GetClass(i, m_currentClass) )
		{
			Q_ASSERT(false && "index is not in list");
		}
		ui->comboBox_class->addItem(m_currentClass.ToString());
	}

	load_values(m_currentClass.ToString());
}

//---------------------------------------------------------
ClassManagerDlg::~ClassManagerDlg()
//---------------------------------------------------------
{
	delete ui;
	m_pClassMgr = nullptr;
}

//---------------------------------------------------------
void ClassManagerDlg::changeEvent(QEvent *e)
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
void ClassManagerDlg::on_pushButton_add_pressed()
//---------------------------------------------------------
{
	bool ok(false);
	QString name = QInputDialog::getText(this,
						  tr("Add new class"),
						  tr("Name of the new class"),
						  QLineEdit::Normal,
						  QString(),
						  &ok);

	while( ok && m_pClassMgr->HasClass(name) )
	{
		QMessageBox::critical(this,
			tr(""),
			tr("This class already exists. Please modify your input.") );

		name = QInputDialog::getText(this,
				tr("Add new class"),
				tr("Name of the new class"),
				QLineEdit::Normal,
				name,
				&ok);
	}

	if( ok )
	{
		m_pClassMgr->AddClass(name);

		// update combobox
		ui->comboBox_class->addItem(name);
		int index = ui->comboBox_class->findText(name);
		ui->comboBox_class->setCurrentIndex(index);
	}
}

//---------------------------------------------------------
void ClassManagerDlg::load_values(QString const& name)
//---------------------------------------------------------
{
	if( !m_pClassMgr->GetClass(name, m_currentClass) )
	{
		m_currentClass = Ipponboard::WeightClass(name);
	}

	const int index = ui->comboBox_class->findText(m_currentClass.ToString());
	ui->comboBox_class->setCurrentIndex(index);

	// set weights
	ui->listWidget_weights->clear();
	ui->listWidget_weights->addItems(m_currentClass.GetWeightList());

	// set times
	QTime time;
	time = time.addSecs(m_currentClass.GetRoundTime());
	ui->timeEdit_round->setTime( time );
	QTime gsTime;
	gsTime = gsTime.addSecs(m_currentClass.GetGoldenScoreTime());
	ui->timeEdit_goldenScore->setTime( gsTime );
}

//---------------------------------------------------------
void ClassManagerDlg::on_buttonBox_accepted()
//---------------------------------------------------------
{
	m_pClassMgr->UpdateClass(m_currentClass);
}

//---------------------------------------------------------
void ClassManagerDlg::on_buttonBox_rejected()
//---------------------------------------------------------
{
	// restore classmanager
	m_pClassMgr->ClassesFromString(m_originalClasses);
}

//---------------------------------------------------------
void ClassManagerDlg::on_pushButton_add_weight_pressed()
//---------------------------------------------------------
{
	bool ok(false);
	QString weight = QInputDialog::getText(this,
						  tr("Add new weight class"),
						  tr("Name of the new weight class"),
						  QLineEdit::Normal,
						  QString(),
						  &ok);
	if( ok && !weight.isEmpty() )
	{
		ui->listWidget_weights->addItem(weight);

		// update data
		m_currentClass.AddWeight(weight.toStdString());
	}
}

//---------------------------------------------------------
void ClassManagerDlg::on_pushButton_remove_weight_pressed()
//---------------------------------------------------------
{
	if( nullptr == ui->listWidget_weights->currentItem() )
		return;

	// update data
	m_currentClass.RemoveWeight(ui->listWidget_weights->currentItem()->text());

	QListWidgetItem* pItem =
			ui->listWidget_weights->takeItem(ui->listWidget_weights->currentRow());
	delete pItem;
}

//---------------------------------------------------------
void ClassManagerDlg::on_timeEdit_round_timeChanged(QTime time)
//---------------------------------------------------------
{
	m_currentClass.SetRoundTime( -time.secsTo(QTime(0,0,0,0)) );
}

//---------------------------------------------------------
void ClassManagerDlg::on_timeEdit_goldenScore_timeChanged(QTime time)
//---------------------------------------------------------
{
	m_currentClass.SetGoldenScoreTime( -time.secsTo(QTime(0,0,0,0)) );
}

//---------------------------------------------------------
void ClassManagerDlg::on_comboBox_class_currentIndexChanged(int index)
//---------------------------------------------------------
{
	// save current class
	m_pClassMgr->UpdateClass(m_currentClass);

	// load other class
	load_values(ui->comboBox_class->itemText(index));
}

//---------------------------------------------------------
void ClassManagerDlg::on_pushButton_up_pressed()
//---------------------------------------------------------
{
	const int index = ui->listWidget_weights->currentRow();

	if( index > 0 )
	{
		QListWidgetItem* pItem =
			ui->listWidget_weights->takeItem(index);

		ui->listWidget_weights->insertItem(index-1, pItem);
		ui->listWidget_weights->setCurrentRow(index-1);

		// update data
		m_currentClass.MoveWeightUp(index);
	}
}

//---------------------------------------------------------
void ClassManagerDlg::on_pushButton_down_pressed()
//---------------------------------------------------------
{
	const int index = ui->listWidget_weights->currentRow();

	if( index >= 0 && index < ui->listWidget_weights->count() - 1 )
	{
		QListWidgetItem* pItem =
			ui->listWidget_weights->takeItem(index);

		ui->listWidget_weights->insertItem(index+1, pItem);
		ui->listWidget_weights->setCurrentRow(index+1);

		// update data
		m_currentClass.MoveWeightUp(index+1);
	}
}

//---------------------------------------------------------
void ClassManagerDlg::on_pushButton_remove_pressed()
//---------------------------------------------------------
{
	int index = ui->comboBox_class->currentIndex();
	if( index >= 0 )
	{
		if(ui->comboBox_class->currentText() ==
		   WeightClassManager::str_defaultClass )
		{
			QMessageBox::warning(this, tr("Remove tournament class"),
								 tr("The default class can not be removed!"));

		}
		else
		{
			m_currentClass = Ipponboard::WeightClass("");

			m_pClassMgr->RemoveClass(ui->comboBox_class->currentText());

			ui->comboBox_class->removeItem(index);
		}
	}
}
