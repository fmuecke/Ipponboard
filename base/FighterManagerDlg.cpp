// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

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
	QWidget* parent)
	: QDialog(parent)
	, ui(new Ui::FighterManagerDlg)
	, m_manager(manager)
	, m_filter()
	, m_formatStr(Ipponboard::FighterManager::DefaultExportFormat())
//---------------------------------------------------------
{
	ui->setupUi(this);

	// hide settings buton
	//TODO: improove!
	ui->pushButton_settings->hide();

	// set columns
	auto headerItem = ui->treeWidget_fighters->headerItem();
	headerItem->setText(eColumn_club, tr("Club/Team"));
	headerItem->setText(eColumn_category, tr("Category"));
	headerItem->setText(eColumn_weight, tr("Weight"));
	headerItem->setText(eColumn_firstName, tr("First Name"));
	headerItem->setText(eColumn_lastName, tr("Last Name"));

	// adjust column widths
	ui->treeWidget_fighters->setColumnWidth(eColumn_club, 150);
	ui->treeWidget_fighters->setColumnWidth(eColumn_category, 60);
	ui->treeWidget_fighters->setColumnWidth(eColumn_weight, 50);
	ui->treeWidget_fighters->setColumnWidth(eColumn_firstName, 100);
	ui->treeWidget_fighters->setColumnWidth(eColumn_lastName, 100);
	ui->treeWidget_fighters->header()->setResizeMode(eColumn_firstName, QHeaderView::Stretch);
	ui->treeWidget_fighters->header()->setResizeMode(eColumn_lastName, QHeaderView::Stretch);

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

Ipponboard::Fighter FighterManagerDlg::itemToFighter(QTreeWidgetItem* pItem) const
{
	if (!pItem) throw std::exception("treeitem is null");

	Ipponboard::Fighter f(pItem->text(eColumn_firstName), pItem->text(eColumn_lastName));

	f.club = pItem->text(eColumn_club);
	f.weight = pItem->text(eColumn_weight);
	f.category = pItem->text(eColumn_category);

	return std::move(f);
}


QStringList FighterManagerDlg::fighterToItemData(Ipponboard::Fighter f) const
{
	QStringList contents;

	for (int i = 0; i < eColumn_MAX; ++i) contents.append("");

	contents[eColumn_club] = f.club;
	contents[eColumn_category] = f.category;
	contents[eColumn_weight] = f.weight;
	contents[eColumn_firstName] = f.first_name;
	contents[eColumn_lastName] = f.last_name;

	return std::move(contents);
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
	auto f = m_manager.AddNewFighter();
	auto contents = fighterToItemData(f);

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

	std::for_each(begin(m_manager.m_fighters), end(m_manager.m_fighters),
				  [&](Ipponboard::Fighter const & f)
	{
		bool skipItem = true;

		if (hasFilter)
		{
			switch (m_filter.first)
			{
			case eColumn_firstName:
				if (m_filter.second == f.first_name)
					skipItem = false;

				break;

			case eColumn_lastName:
				if (m_filter.second == f.last_name)
					skipItem = false;

				break;

			case eColumn_club:
				if (m_filter.second == f.club)
					skipItem = false;

				break;

			case eColumn_weight:
				if (m_filter.second == f.weight)
					skipItem = false;

				break;

			case eColumn_category:
				if (m_filter.second == f.category)
					skipItem = false;
				break;

			default:
				break;
			}
		}
		else
		{
			skipItem = false;
		}

		if (!skipItem)
		{
			auto contents = fighterToItemData(f);
			QTreeWidgetItem* pItem =
				new QTreeWidgetItem(contents, QTreeWidgetItem::UserType);
			pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
			ui->treeWidget_fighters->addTopLevelItem(pItem);
		}
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
		//TODO: make this right
		on_pushButton_settings_pressed();

		QString errorMsg;

		if (m_manager.ImportFighters(fileName, m_formatStr, errorMsg))
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
		//TODO: make this right
		on_pushButton_settings_pressed();

		QString errorMsg;

		if (m_manager.ExportFighters(fileName, m_formatStr, errorMsg))
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
	auto removeItems = false;
	if (selectedItems.count() == 1 && QMessageBox::Yes == QMessageBox::question(
		this,
		QCoreApplication::applicationName(),
		tr("Really remove fighter \"%1 %2\"?").arg(selectedItems.at(0)->text(eColumn_firstName), selectedItems.at(0)->text(eColumn_lastName)),
		QMessageBox::Yes | QMessageBox::No))
	{
		removeItems = true;
	}
	else if (selectedItems.count() > 1 && QMessageBox::Yes == QMessageBox::question(
		 this,
		 QCoreApplication::applicationName(),
		 tr("Really remove the %1 selected fighters?").arg(selectedItems.count()),
		 QMessageBox::Yes | QMessageBox::No))
	{
		removeItems = true;
	}

	if (removeItems)
	{
		for (QTreeWidgetItem * pItem : selectedItems)
		{
			auto fighter = itemToFighter(pItem);
			m_manager.RemoveFighter(fighter);

			ui->treeWidget_fighters->takeTopLevelItem(
				ui->treeWidget_fighters->indexOfTopLevelItem(pItem));

			delete pItem;
		}
	}
}

//---------------------------------------------------------
void FighterManagerDlg::on_treeWidget_fighters_itemChanged(
	QTreeWidgetItem* pItem, int column)
//---------------------------------------------------------
{
	if (pItem)
	{
		auto updatedFighterData = itemToFighter(pItem);

		// clone item and get original value
		Ipponboard::Fighter originalFighterData(updatedFighterData);
		qDebug("enum value: %i", column);

		switch (column)
		{
		case eColumn_firstName:
			originalFighterData.first_name = m_currentCellData;
			break;

		case eColumn_lastName:
			originalFighterData.last_name = m_currentCellData;
			break;

		case eColumn_club:
			originalFighterData.club = m_currentCellData;
			break;

		case eColumn_weight:
			originalFighterData.weight = m_currentCellData;
			break;

		case eColumn_category:
			originalFighterData.category = m_currentCellData;
			break;

		default:
			qDebug("ERROR: invalid enum value: %i", column);
			break;
		}

		if (!m_manager.RemoveFighter(originalFighterData))
		{
			qDebug("error: original fighter not found!");
		}

		if (!m_manager.AddFighter(updatedFighterData))
		{
			ui->treeWidget_fighters->takeTopLevelItem(ui->treeWidget_fighters->indexOfTopLevelItem(pItem));

			// due to duplicate entry
			QMessageBox::information(
						this,
						QApplication::applicationName(),
						tr("The new entry is identical to an already existing one and was therefore discarded: %1 %2")
						.arg(updatedFighterData.first_name)
						.arg(updatedFighterData.last_name));
		}
	}
}

void FighterManagerDlg::on_treeWidget_fighters_itemClicked(QTreeWidgetItem* item, int column)
{
	m_currentCellData = item->text(column);
	qDebug("data: %s", m_currentCellData.toLatin1().data());
}

void FighterManagerDlg::on_pushButton_settings_pressed()
{
	bool ok(false);
	QString dlgTitle = tr("Specify import/export format");
	QString dlgMsg = tr("Use valid specifiers and some kind of separator (;,:|/ etc.)"
						"\nValid specifiers are: %1")
					 .arg(Ipponboard::FighterManager::GetSpecifiererDescription());

	QString data = QInputDialog::getText(this,
										 dlgTitle,
										 dlgMsg,
										 QLineEdit::Normal,
										 m_formatStr,
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
		m_formatStr = data;
	}
}
