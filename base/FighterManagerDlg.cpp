// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
#include "fightermanagerdlg.h"
#include "ui_fightermanagerdlg.h"

#include "FighterManager.h"
#include "AddFighterDlg.h"
#include "FighterManagerImportDlg.h"
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
        QStringList const& clubs,
        QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::FighterManagerDlg)
	, m_manager(manager)
	, m_filter()
	, m_formatStr(Ipponboard::FighterManager::DefaultExportFormat())
    , m_clubs(clubs)
//---------------------------------------------------------
{
	ui->setupUi(this);

	// set columns
	auto headerItem = ui->treeWidget_fighters->headerItem();
	headerItem->setText(eColumn_club, tr("Club/Team"));
    headerItem->setText(eColumn_category, tr("Category"));
	headerItem->setText(eColumn_weight, tr("Weight"));
	headerItem->setText(eColumn_firstName, tr("First Name"));
	headerItem->setText(eColumn_lastName, tr("Last Name"));
    headerItem->setText(eColumn_year, tr("Year"));
    headerItem->setText(eColumn_nation, tr("Nation"));

	// adjust column widths
	ui->treeWidget_fighters->setColumnWidth(eColumn_club, 150);
    ui->treeWidget_fighters->setColumnWidth(eColumn_category, 60);
    ui->treeWidget_fighters->setColumnWidth(eColumn_year, 50);
    ui->treeWidget_fighters->setColumnWidth(eColumn_nation, 50);
    ui->treeWidget_fighters->setColumnWidth(eColumn_weight, 50);
    ui->treeWidget_fighters->setColumnWidth(eColumn_firstName, 120);
    ui->treeWidget_fighters->setColumnWidth(eColumn_lastName, 120);
    ui->treeWidget_fighters->header()->setResizeMode(eColumn_firstName, QHeaderView::Stretch);
    ui->treeWidget_fighters->header()->setResizeMode(eColumn_lastName, QHeaderView::Stretch);

    ui->pushButton_remove->setEnabled(false);

	populate_view();
}

//---------------------------------------------------------
FighterManagerDlg::~FighterManagerDlg()
//---------------------------------------------------------
{
	delete ui;
}

//---------------------------------------------------------
void FighterManagerDlg::SetFilter(FighterManagerDlg::EColumn column, const QString& value)
//---------------------------------------------------------
{
	if (column >= 0 && column < eColumn_MAX)
	{
		m_filter = std::make_pair(column, value);
		ui->treeWidget_fighters->hideColumn(m_filter.first);

		populate_view();
	}
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
    AddFighterDlg dlg(this);

    QStringList clubs;
	if (m_filter.first == eColumn_club)
    {
        clubs.append(m_filter.second);
    }
    else
    {
        //FIXME:
        //m_manager.GetClubs...
    }
    dlg.SetClubs(clubs);

    if (dlg.exec() == QDialog::Rejected)
    {
        return;
    }

    Ipponboard::Fighter fighter = dlg.GetFighter();
    m_manager.m_fighters.insert(fighter);

    QStringList contents;
    for (int i = 0; i < eColumn_MAX; ++i) contents.append("");
    contents[eColumn_club] = fighter.club;
    contents[eColumn_year] = fighter.year;
    contents[eColumn_nation] = fighter.nation;
    contents[eColumn_category] = fighter.category;
    contents[eColumn_weight] = fighter.weight;
    contents[eColumn_firstName] = fighter.first_name;
    contents[eColumn_lastName] = fighter.last_name;

    QTreeWidgetItem* pItem =
        new QTreeWidgetItem(contents, QTreeWidgetItem::UserType);
    pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
    ui->treeWidget_fighters->addTopLevelItem(pItem);
}

//---------------------------------------------------------
void FighterManagerDlg::populate_view()
//---------------------------------------------------------
{
	ui->treeWidget_fighters->clear();

	const bool hasFilter = !m_filter.second.isEmpty();

	if (hasFilter)
	{
		const QString filterInfo = QString("%0: %1").arg(
									   ui->treeWidget_fighters->headerItem()->text(m_filter.first),
									   m_filter.second);

		ui->label_filterinfo->setText(filterInfo);
		ui->label_filterinfo->show();
	}
	else
	{
		ui->label_filterinfo->hide();
	}

    for (Ipponboard::Fighter const& f : m_manager.m_fighters)
	{
		bool skipItem = true;

		if (hasFilter)
		{
            if (m_filter.first == eColumn_firstName && m_filter.second == f.first_name ||
                    m_filter.first == eColumn_lastName && m_filter.second == f.last_name ||
                    m_filter.first == eColumn_club && m_filter.second == f.club ||
                    m_filter.first == eColumn_year && m_filter.second == f.year ||
                    m_filter.first == eColumn_nation && m_filter.second == f.nation ||
                    m_filter.first == eColumn_category && m_filter.second == f.category ||
                    m_filter.first == eColumn_weight && m_filter.second == f.weight)
            {
                skipItem = false;
            }
		}
		else
		{
			skipItem = false;
		}

		if (!skipItem)
		{
			QStringList contents;

			for (int i = 0; i < eColumn_MAX; ++i) contents.append("");

			contents[eColumn_club] = f.club;
            contents[eColumn_category] = f.category;
            contents[eColumn_year] = f.year;
            contents[eColumn_nation] = f.nation;
			contents[eColumn_weight] = f.weight;
			contents[eColumn_firstName] = f.first_name;
			contents[eColumn_lastName] = f.last_name;

			QTreeWidgetItem* pItem =
				new QTreeWidgetItem(contents, QTreeWidgetItem::UserType);
			pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
			ui->treeWidget_fighters->addTopLevelItem(pItem);
		}
    }
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
        FighterManagerImportDlg dlg(fileName, m_clubs, this);
        if (dlg.exec() == QDialog::Rejected)
        {
            return;
        }

        Ipponboard::FighterManager manager;
		QString errorMsg;

        if (manager.ImportFighters(fileName, dlg.GetFormatStr(), dlg.GetEncoding(), errorMsg))
		{
            for (auto fighter : manager.m_fighters)
            {
                if (!dlg.GetSelectedClub().isEmpty())
                {
                    fighter.club = dlg.GetSelectedClub();
                }

                m_manager.AddFighter(fighter);
            }

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

	populate_view();
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

        if (m_manager.ExportFighters(fileName, Ipponboard::FighterManager::DefaultExportFormat(), "utf-8", errorMsg))
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

    if (QMessageBox::question(this, tr("Remove item(s)?"), tr("Relly remove %1 item(s)?").arg(selectedItems.size()),tr("Remove"), tr("Keep")) != 0)
    {
        return;
    }

    for (QTreeWidgetItem * pItem : selectedItems)
	{
		Ipponboard::Fighter currentFighter(
			pItem->text(eColumn_firstName),
			pItem->text(eColumn_lastName));

		currentFighter.club = pItem->text(eColumn_club);
		currentFighter.weight = pItem->text(eColumn_weight);
        currentFighter.category = pItem->text(eColumn_category);
        currentFighter.year = pItem->text(eColumn_year);
        currentFighter.nation = pItem->text(eColumn_nation);

        auto pRealItem = ui->treeWidget_fighters->takeTopLevelItem(
			ui->treeWidget_fighters->indexOfTopLevelItem(pItem));
        Q_ASSERT(pRealItem == pItem);

		m_manager.RemoveFighter(currentFighter);
        delete pRealItem;
	}
}

//---------------------------------------------------------
void FighterManagerDlg::on_treeWidget_fighters_itemChanged(
	QTreeWidgetItem* pItem, int column)
//---------------------------------------------------------
{
	if (pItem)
	{
        Ipponboard::Fighter originalFighter(
            column == eColumn_firstName ? m_tmpData : pItem->text(eColumn_firstName),
            column == eColumn_lastName ? m_tmpData : pItem->text(eColumn_lastName));

        originalFighter.club = column == eColumn_club ? m_tmpData : pItem->text(eColumn_club);
        originalFighter.weight = column == eColumn_weight ? m_tmpData : pItem->text(eColumn_weight);
        originalFighter.category = column == eColumn_category ? m_tmpData : pItem->text(eColumn_category);
        originalFighter.year = column == eColumn_year ? m_tmpData : pItem->text(eColumn_year);
        originalFighter.nation = column == eColumn_nation ? m_tmpData : pItem->text(eColumn_nation);

        Ipponboard::Fighter changedFighter(
            pItem->text(eColumn_firstName),
            pItem->text(eColumn_lastName));

        changedFighter.club = pItem->text(eColumn_club);
        changedFighter.weight = pItem->text(eColumn_weight);
        changedFighter.category = pItem->text(eColumn_category);
        changedFighter.year = pItem->text(eColumn_year);
        changedFighter.nation = pItem->text(eColumn_nation);

		if (!m_manager.RemoveFighter(originalFighter))
		{
			qDebug("error: original fighter not found!");
		}

		if (!m_manager.AddFighter(changedFighter))
		{
            // reset value to avoid duplicate entries
            pItem->setText(column, m_tmpData);
		}
	}
}

void FighterManagerDlg::on_treeWidget_fighters_itemClicked(QTreeWidgetItem* item, int column)
{
	m_tmpData = item->text(column);
    qDebug("data: %s", m_tmpData.toLatin1().data());
}

void FighterManagerDlg::on_treeWidget_fighters_itemSelectionChanged()
{
    auto selectedItems = ui->treeWidget_fighters->selectedItems();
    ui->pushButton_remove->setEnabled(selectedItems.count() > 0);
}

