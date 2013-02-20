// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
#include "fightermanagerdlg.h"
#include "ui_fightermanagerdlg.h"

#include "FighterManager.h"
#include "../core/fighter.h"
//#include "../util/path_helpers.h"
#include <QInputDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExp>
#include <QPlainTextEdit>

//using namespace Ipponboard;

//---------------------------------------------------------
FighterManagerDlg::FighterManagerDlg(
        Ipponboard::FighterManager& manager,
        QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FighterManagerDlg)
    , m_manager(manager)
//---------------------------------------------------------
{
    ui->setupUi(this);

    // adjust column widths
    ui->treeWidget_fighters->setColumnWidth(eColumn_firstName, 100);
    ui->treeWidget_fighters->setColumnWidth(eColumn_lastName, 100);
    ui->treeWidget_fighters->setColumnWidth(eColumn_club, 150);
    ui->treeWidget_fighters->setColumnWidth(eColumn_weight, 50);
    ui->treeWidget_fighters->setColumnWidth(eColumn_category, 50);

    populate_view();
}

//---------------------------------------------------------
FighterManagerDlg::~FighterManagerDlg()
//---------------------------------------------------------
{
    delete ui;
}

//---------------------------------------------------------
void FighterManagerDlg::changeEvent(QEvent* e)
//---------------------------------------------------------
{
	QDialog::changeEvent(e);

	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;

	default:
		break;
	}
}

//---------------------------------------------------------
void FighterManagerDlg::on_pushButton_add_pressed()
//---------------------------------------------------------
{
    bool ok(false);
    QString dlgTitle = tr("Add new fighter");
    QString dlgMsg = tr("Enter \"first name/last name/club/weight/category\" of the new fighter");

    QString data = QInputDialog::getText(this,
                                         dlgTitle,
                                         dlgMsg,
										 QLineEdit::Normal,
										 QString(),
                                         &ok);
    auto dataParts = data.split('/');

    while (ok && dataParts.size() != 5)
	{
		QMessageBox::critical(this,
							  tr(""),
                              tr("Invalid format. Please correct your input."));

        data = QInputDialog::getText(this,
                                     dlgTitle,
                                     dlgMsg,
									 QLineEdit::Normal,
                                     data,
									 &ok);
        dataParts = data.split('/');
    }

	if (ok)
	{
        Ipponboard::Fighter fighter(
                dataParts[0],  // first
                dataParts[1],  // last
                dataParts[2],  // weight
                dataParts[3],  // category
                dataParts[4]); // club
        m_manager.m_fighters.push_back(fighter);

		QStringList contents;
        contents.append(dataParts[0]);
        contents.append(dataParts[1]);
        contents.append(dataParts[2]);
        contents.append(dataParts[3]);
        contents.append(dataParts[4]);
        QTreeWidgetItem* pItem =
			new QTreeWidgetItem(contents, QTreeWidgetItem::UserType);
		pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->treeWidget_fighters->addTopLevelItem(pItem);
	}
}

//---------------------------------------------------------
void FighterManagerDlg::populate_view()
//---------------------------------------------------------
{
    ui->treeWidget_fighters->clear();

    std::for_each(begin(m_manager.m_fighters), end(m_manager.m_fighters),
                  [&](Ipponboard::Fighter const& f)
    {
        QStringList contents;
        contents.append(f.first_name);
        contents.append(f.last_name);
        contents.append(f.club);
        contents.append(f.weight_class);
        contents.append(f.category);

        QTreeWidgetItem* pItem =
            new QTreeWidgetItem(contents, QTreeWidgetItem::UserType);
        pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->treeWidget_fighters->addTopLevelItem(pItem);
    });
}

//---------------------------------------------------------
void FighterManagerDlg::on_pushButton_import_pressed()
//---------------------------------------------------------
{
    const QString fileName = QFileDialog::getOpenFileName(this,
                    tr("Select CSV file with fighters"),
                    QCoreApplication::applicationDirPath(),
                    tr("CSV files (*.csv);;Text files (*.txt)"), nullptr, QFileDialog::ReadOnly);

    if (!fileName.isEmpty())
    {
        QString errorMsg;
        if (m_manager.ImportFighters(fileName, errorMsg))
        {
            QMessageBox::information(
                        this,
                        QCoreApplication::applicationName(),
                        errorMsg);

            //TODO?: update_fighter_name_completer(m_pUi->comboBox_weight->currentText());
        }
        else
        {
            QMessageBox::critical(
                        this,
                        QCoreApplication::applicationName(),
                        errorMsg);
        }
    }

    populate_view();

    /*const int index = ui->treeWidget_fighters->indexOfTopLevelItem(
                          ui->treeWidget_fighters->currentItem());

	if (index > 0)
	{
		QTreeWidgetItem* pItem =
            ui->treeWidget_fighters->takeTopLevelItem(index);

        ui->treeWidget_fighters->insertTopLevelItem(index - 1, pItem);
        ui->treeWidget_fighters->setCurrentItem(pItem);

		// update data
		m_pClassMgr->MoveCategoryUp(pItem->text(eColumn_Name));
	}
	*/
}

//---------------------------------------------------------
void FighterManagerDlg::on_pushButton_export_pressed()
//---------------------------------------------------------
{
    const QString fileName = QFileDialog::getSaveFileName(this,
                    tr("Name CSV file to store fighters in"),
                    QCoreApplication::applicationDirPath(),
                    tr("CSV files (*.csv);;Text files (*.txt)"));

    if (!fileName.isEmpty())
    {
        QString errorMsg;
        if (m_manager.ExportFighters(fileName, errorMsg))
        {
            QMessageBox::information(
                        this,
                        QCoreApplication::applicationName(),
                        errorMsg);
        }
        else
        {
            QMessageBox::critical(
                        this,
                        QCoreApplication::applicationName(),
                        errorMsg);
        }
    }
}

//---------------------------------------------------------
void FighterManagerDlg::on_pushButton_remove_pressed()
//---------------------------------------------------------
{
    QTreeWidgetItem* pItem = ui->treeWidget_fighters->currentItem();

	if (pItem)
	{
        Ipponboard::Fighter currentFighter(
                    pItem->text(eColumn_firstName),
                    pItem->text(eColumn_lastName),
                    pItem->text(eColumn_club),
                    pItem->text(eColumn_weight),
                    pItem->text(eColumn_category));

        ui->treeWidget_fighters->takeTopLevelItem(
            ui->treeWidget_fighters->indexOfTopLevelItem(pItem));

        //TODO: move inside manager
        m_manager.m_fighters.erase(std::find(
            begin(m_manager.m_fighters),
            end(m_manager.m_fighters),
            currentFighter));

		delete pItem;
	}
}

//---------------------------------------------------------
void FighterManagerDlg::on_treeWidget_fighters_itemChanged(
	QTreeWidgetItem* pItem, int column)
//---------------------------------------------------------
{
    bool matches(false);

    /*FightCategory cat(pItem->text(eColumn_Name));
	cat.SetRoundTime(pItem->text(eColumn_Time));
	cat.SetGoldenScoreTime(pItem->text(eColumn_GS));
	cat.SetWeights(pItem->text(eColumn_Weights));

	if (eColumn_Name == column)
	{
		// get original data
		FightCategory old;

        for (int i(0); i < ui->treeWidget_fighters->topLevelItemCount(); ++i)
		{
			const QTreeWidgetItem* pCheckItem =
                ui->treeWidget_fighters->topLevelItem(i);

			if (pCheckItem == pItem)
			{
				m_pClassMgr->GetCategory(i, old);

                // Resetting the name to the old one (below) will trigger
				// itemChanged!
				if (old.ToString() == cat.ToString())
					return;

				break;
			}
		}

		// check if we do have more than one class with that name
        for (int i(0); i < ui->treeWidget_fighters->topLevelItemCount(); ++i)
		{
			const QTreeWidgetItem* pCheckItem =
                ui->treeWidget_fighters->topLevelItem(i);

			if (pCheckItem != pItem &&
					pCheckItem->text(eColumn_Name) == pItem->text(eColumn_Name))
			{
				QMessageBox::critical(
					this,
					QCoreApplication::applicationName(),
					tr("This name is already taken!"));

				// set previous text
				pItem->setText(eColumn_Name, old.ToString());

				return;
			}
		}

        m_pClassMgr->RenameCategory(old.ToString(), cat.ToString());
		return;
	}
	else if (eColumn_Time == column)
	{
		QRegExp regex("[1-6]{0,1}[0-9][:][0-5][0-9]");
		matches = regex.exactMatch(pItem->text(column));
	}
	else if (eColumn_GS == column)
	{
		QRegExp regex("[1-6]{0,1}[0-9][:][0-5][0-9]");
		matches = regex.exactMatch(pItem->text(column));
	}
	else if (column == eColumn_Weights)
	{
		QRegExp regex("([-+]{0,1}[0-9]{1,3}[;])*[-+]{0,1}[0-9]{1,3}");
		matches = regex.exactMatch(pItem->text(eColumn_Weights));
	}

	QBrush brush(pItem->foreground(column));

	if (matches)
	{
		brush.setColor(Qt::black);
		m_pClassMgr->UpdateCategory(cat);
	}
	else
	{
		brush.setColor(Qt::red);
	}

	pItem->setForeground(column, brush);
	
	*/
}

void FighterManagerDlg::on_pushButton_settings_pressed()
{
    bool ok(false);
    QString dlgTitle = tr("Specify import/export format");
    QString dlgMsg = tr("Use valid specifiers and some some kind of separator (;,:|/ etc.)"
                        "\nValid specifiers are: %1")
            .arg(Ipponboard::FighterManager::GetSpecifiererDescription());

    QString data = QInputDialog::getText(this,
                                         dlgTitle,
                                         dlgMsg,
                                         QLineEdit::Normal,
                                         m_manager.GetExportFormat(),
                                         &ok);

    QString separator;
    bool isValidSeparator = Ipponboard::FighterManager::DetermineSeparator(data, separator);

    QStringList dataParts;
    if (isValidSeparator)
    {
        dataParts = data.split(separator);
    }

    // at least 3 parts must be set (first, last, ...)
    while (ok && (!isValidSeparator || dataParts.size() < 3))
    {
        QMessageBox::critical(this,
                              tr(""),
                              tr("Invalid format. Please correct your input."));

        data = QInputDialog::getText(this,
                                     dlgTitle,
                                     dlgMsg,
                                     QLineEdit::Normal,
                                     data,
                                     &ok);
        isValidSeparator =
                Ipponboard::FighterManager::DetermineSeparator(data, separator);

        if (isValidSeparator)
        {
            dataParts = data.split(separator);
        }
    }

    if (ok && isValidSeparator)
    {
        m_manager.SetExportFormat(data);
    }
}
