// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "../base/FightCategoryManager.h"
#include "../base/FightCategoryManagerDlg.h"
#include "../base/FighterManagerDlg.h"
#include "../base/View.h"
#include "../base/versioninfo.h"
#include "../core/Controller.h"
#include "../core/ControllerConfig.h"
#include "../core/Fighter.h"
#include "../core/TournamentModel.h"
#include "../gamepad/gamepad.h"
#include "../util/path_helpers.h"
#include "../widgets/ScaledImage.h"
#include "../widgets/ScaledText.h"

#include <QColorDialog>
#include <QComboBox>
#include <QCompleter>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSplashScreen>
#include <QTimer>
#include <QUrl>
#include <functional>

namespace StrTags
{
static const char* const edition = "Basic Edition";
}

using namespace FMlib;
using namespace Ipponboard;

MainWindow::MainWindow(QWidget* parent)
	: MainWindowBase(parent)
	, m_pCategoryManager()
{
	m_pUi->setupUi(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::Init()
{
	m_pCategoryManager.reset(new FightCategoryMgr());

	MainWindowBase::Init();

	// init tournament classes (if there are none present)
	for (int i(0); i < m_pCategoryManager->CategoryCount(); ++i)
	{
		FightCategory t("");
		m_pCategoryManager->GetCategory(i, t);
		m_pUi->comboBox_weight_class->addItem(t.ToString());
	}

	// trigger tournament class combobox update
	on_comboBox_weight_class_currentIndexChanged(m_pUi->comboBox_weight_class->currentText());
}

void MainWindow::on_actionManageClasses_triggered()
{
	FightCategoryManagerDlg dlg(m_pCategoryManager, this);

	if (QDialog::Accepted == dlg.exec())
	{
		QString currentClass =
			m_pUi->comboBox_weight_class->currentText();

		m_pUi->comboBox_weight_class->clear();

		for (int i(0); i < m_pCategoryManager->CategoryCount(); ++i)
		{
			FightCategory t("");
			m_pCategoryManager->GetCategory(i, t);
			m_pUi->comboBox_weight_class->addItem(t.ToString());
		}

		int index = m_pUi->comboBox_weight_class->findText(currentClass);

		if (-1 == index)
		{
			index = 0;
			currentClass = m_pUi->comboBox_weight_class->itemText(index);
		}

		m_pUi->comboBox_weight_class->setCurrentIndex(index);
		on_comboBox_weight_class_currentIndexChanged(currentClass);
	}
}

void MainWindow::on_actionManageFighters_triggered()
{
	MainWindowBase::on_actionManageFighters_triggered();

	FighterManagerDlg dlg(m_fighterManager, this);
	dlg.exec();
}

void MainWindow::on_comboBox_weight_currentIndexChanged(const QString& s)
{
	update_fighter_name_completer(s);

	m_pPrimaryView->SetWeight(s);
	m_pSecondaryView->SetWeight(s);
	m_pPrimaryView->UpdateView();
	m_pSecondaryView->UpdateView();
}

void MainWindow::on_comboBox_name_first_currentIndexChanged(const QString& s)
{
	update_fighters(s);

	m_pController->SetFighterName(eFighter1, s);
}

void MainWindow::on_comboBox_name_second_currentIndexChanged(const QString& s)
{
	update_fighters(s);

	m_pController->SetFighterName(eFighter2, s);
}

void MainWindow::on_checkBox_golden_score_clicked(bool checked)
{
	const QString name = m_pUi->comboBox_weight_class->currentText();
	FightCategory t(name);
	m_pCategoryManager->GetCategory(name, t);

	m_pController->SetGoldenScore(checked);
	//> Set this before setting the time.
	//> Setting time will then update the views.

	if (checked)
	{
		if (m_pController->GetOption(Ipponboard::eOption_Use2013Rules))
		{
			m_pController->SetFightTime(QTime());
		}
		else
		{
			m_pController->SetFightTime(
				QTime().addSecs(t.GetGoldenScoreTime()));
		}
	}
	else
	{
		m_pController->SetFightTime(
			QTime().addSecs(t.GetRoundTime()));
	}
}

void MainWindow::on_comboBox_weight_class_currentIndexChanged(const QString& s)
{
	FightCategory t(s);
	m_pCategoryManager->GetCategory(s, t);

	// add weights
	m_pUi->comboBox_weight->clear();
	m_pUi->comboBox_weight->addItems(t.GetWeightsList());

	// trigger round time update
	on_checkBox_golden_score_clicked(m_pUi->checkBox_golden_score->checkState());

	m_pPrimaryView->SetCategory(s);
	m_pSecondaryView->SetCategory(s);
	m_pPrimaryView->UpdateView();
	m_pSecondaryView->UpdateView();
}

void MainWindow::update_fighter_name_completer(const QString& weight)
{
	// filter fighters for suitable
	m_CurrentFighterNames.clear();

	Q_FOREACH(const Ipponboard::Fighter & f, m_fighterManager.m_fighters)
	{
		if (f.weight == weight || f.weight.isEmpty())
		{
			const QString fullName =
				QString("%1 %2").arg(f.first_name, f.last_name);

			m_CurrentFighterNames.push_back(fullName);
		}
	}

	m_CurrentFighterNames.sort();

	m_pUi->comboBox_name_first->clear();
	m_pUi->comboBox_name_first->addItems(m_CurrentFighterNames);
	m_pUi->comboBox_name_second->clear();
	m_pUi->comboBox_name_second->addItems(m_CurrentFighterNames);
}

void MainWindow::update_fighters(const QString& s)
{
	if (s.isEmpty())
		return;

	QString firstName = s;
	QString lastName;

	int pos = s.indexOf(' ');

	if (pos < s.size())
	{
		firstName = s.left(pos);
		lastName = s.mid(pos + 1);
	}

	const QString weight = m_pUi->comboBox_weight->currentText();
	const QString club; // TODO: later
	const QString category = m_pUi->comboBox_weight_class->currentText();

	Ipponboard::Fighter fNew(firstName, lastName);
	fNew.club = club;
	fNew.weight = weight;
	fNew.category = category;

	m_fighterManager.AddFighter(fNew); // only adds fighter if new
}

void MainWindow::update_statebar()
{
	MainWindowBase::update_statebar();

//    if (Gamepad::eState_ok != m_pGamepad->GetState())
//    {
//        m_pUi->label_controller_state->setText(tr("No controller detected!"));
//    }
//    else
//    {
//        QString controllerName = QString::fromWCharArray(m_pGamepad->GetProductName());
//        m_pUi->label_controller_state->setText(tr("Using controller %1").arg(controllerName));
//    }
	m_pUi->checkBox_use2013rules->setChecked(m_pController->GetOption(eOption_AutoIncrementPoints));
	m_pUi->checkBox_autoIncrement->setChecked(m_pController->GetOption(eOption_Use2013Rules));
}

void MainWindow::on_checkBox_use2013rules_toggled(bool checked)
{
	m_pController->SetOption(eOption_Use2013Rules, checked);
}

void MainWindow::on_checkBox_autoIncrement_toggled(bool checked)
{
	m_pController->SetOption(eOption_AutoIncrementPoints, checked);
}
