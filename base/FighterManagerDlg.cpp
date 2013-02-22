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

#include <boost/foreach.hpp>

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
    QString dlgMsg = tr("Enter \"first name;last name;club;weight;category\" of the new fighter");

    QString data = QInputDialog::getText(this,
                                         dlgTitle,
                                         dlgMsg,
										 QLineEdit::Normal,
										 QString(),
                                         &ok);
    auto dataParts = data.split(';');

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
                dataParts[1]);  // last
        fighter.club = dataParts[2];
        fighter.weight = dataParts[3];
        fighter.category = dataParts[4];

        m_manager.m_fighters.insert(fighter);

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
        contents.append(f.weight);
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
    auto selectedItems = ui->treeWidget_fighters->selectedItems();

    //QTreeWidgetItem* pItem = ui->treeWidget_fighters->currentItem();

    //if (pItem)
    BOOST_FOREACH(QTreeWidgetItem* pItem, selectedItems)
    {
        Ipponboard::Fighter currentFighter(
                    pItem->text(eColumn_firstName),
                    pItem->text(eColumn_lastName));
        currentFighter.club = pItem->text(eColumn_club);
        currentFighter.weight = pItem->text(eColumn_weight);
        currentFighter.category = pItem->text(eColumn_category);

        ui->treeWidget_fighters->takeTopLevelItem(
            ui->treeWidget_fighters->indexOfTopLevelItem(pItem));

        m_manager.RemoveFighter(currentFighter);

		delete pItem;
	}
}

//---------------------------------------------------------
void FighterManagerDlg::on_treeWidget_fighters_itemChanged(
	QTreeWidgetItem* pItem, int column)
//---------------------------------------------------------
{
    if (pItem)
    {
        QString firstName = pItem->text(eColumn_firstName);
        QString lastName = pItem->text(eColumn_lastName);
        QString club = pItem->text(eColumn_club);
        QString weight = pItem->text(eColumn_weight);
        QString category = pItem->text(eColumn_category);

        Ipponboard::Fighter changedFighter(firstName, lastName);
        changedFighter.club = club;
        changedFighter.weight = weight;
        changedFighter.category = category;

        qDebug("enum value: %i", column);

        switch (column)
        {
            case eColumn_firstName:
                firstName = m_tmpData;
                break;
            case eColumn_lastName:
                lastName = m_tmpData;
                break;
            case eColumn_club:
                club = m_tmpData;
                break;
            case eColumn_weight:
                weight = m_tmpData;
                break;
            case eColumn_category:
                category = m_tmpData;
                break;
            default:
                qDebug("ERROR: invalid enum value: %i", column);
                break;
        }

        Ipponboard::Fighter originalFighter(firstName, lastName);
        originalFighter.club = club;
        originalFighter.weight = weight;
        originalFighter.category = category;

        if (!m_manager.RemoveFighter(originalFighter))
        {
            qDebug("error: original fighter not found!");
        }

        if (!m_manager.AddFighter(changedFighter))
        {
            ui->treeWidget_fighters->takeTopLevelItem(
                ui->treeWidget_fighters->indexOfTopLevelItem(pItem));

            // due to duplicate entry
            qDebug("removed changed entry due to duplicate: %s %s",
                   changedFighter.first_name.toAscii().data(),
                   changedFighter.last_name.toAscii().data());
        }
    }
}

void FighterManagerDlg::on_treeWidget_fighters_itemClicked(QTreeWidgetItem *item, int column)
{
    m_tmpData = item->text(column);
    qDebug("data: %s", m_tmpData.toLatin1().data());
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
