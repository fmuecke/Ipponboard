// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "../base/FightCategoryManager.h"
#include "../base/FightCategoryManagerDlg.h"
#include "../base/FighterManagerDlg.h"
#include "../base/View.h"
#include "../core/Controller.h"

#include "../core/Fighter.h"

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
#include <QTimer>
#include <QUrl>

namespace StrTags
{
static const char* const edition = "Basic Edition";
}

using namespace FMlib;
using namespace Ipponboard;

MainWindow::MainWindow(QWidget* parent)
	: MainWindowBase(parent)
	, m_pUi(new Ui::MainWindow)
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

	m_pUi->actionAutoAdjustPoints->setChecked(m_pController->IsAutoAdjustPoints());
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

	m_pController->SetFighterName(FighterEnum::First, s);
}

void MainWindow::on_comboBox_name_second_currentIndexChanged(const QString& s)
{
	update_fighters(s);

	m_pController->SetFighterName(FighterEnum::Second, s);
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
		if (m_pController->GetRules()->IsOption_OpenEndGoldenScore())
		{
			m_pController->SetRoundTime(QTime(0,0,0,0));
		}
		else
		{
			m_pController->SetRoundTime(QTime(0,0,0,0).addSecs(t.GetGoldenScoreTime()));
		}
	}
	else
	{
		m_pController->SetRoundTime(QTime(0,0,0,0).addSecs(t.GetRoundTime()));
	}
}

void MainWindow::on_comboBox_weight_class_currentIndexChanged(const QString& s)
{
	FightCategory category(s);
	m_pCategoryManager->GetCategory(s, category);

	// add weights
	m_pUi->comboBox_weight->clear();
	m_pUi->comboBox_weight->addItems(category.GetWeightsList());

	// trigger round time update
	on_checkBox_golden_score_clicked(m_pUi->checkBox_golden_score->checkState());

	m_pController->OverrideRoundTimeOfFightMode(category.GetRoundTime());
	m_pController->DoAction(Ipponboard::eAction_ResetAll);

	m_pPrimaryView->SetCategory(s);
	m_pSecondaryView->SetCategory(s);
	m_pPrimaryView->UpdateView();
	m_pSecondaryView->UpdateView();
}

void MainWindow::update_fighter_name_completer(const QString& weight)
{
	// filter fighters for suitable
	m_CurrentFighterNames.clear();

	for (const Ipponboard::Fighter & f : m_fighterManager.m_fighters)
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
	ui_check_rules_items();

//    if (m_pController->GetOption(eOption_Use2013Rules))
//    {
//        m_pUi->actionRulesClassic->setChecked(false);
//        m_pUi->actionRules2013->setChecked(true);
//        m_pUi->actionRules2017->setChecked(false);
//    }
//    else
//    {
//        m_pUi->actionRulesClassic->setChecked(true);
//        m_pUi->actionRules2013->setChecked(false);
//        m_pUi->actionRules2017->setChecked(false);
//    }
}

void MainWindow::attach_primary_view()
{
	QWidget* widget = dynamic_cast<QWidget*>(m_pPrimaryView.get());

	if (widget)
	{
		m_pUi->verticalLayout_3->insertWidget(0, widget, 0);
	}
}

void MainWindow::retranslate_Ui()
{
	m_pUi->retranslateUi(this);
}

void MainWindow::ui_check_language_items()
{
	m_pUi->actionLang_Deutsch->setChecked("de" == m_Language);
	m_pUi->actionLang_English->setChecked("en" == m_Language);
	m_pUi->actionLang_Dutch->setChecked("nl" == m_Language);

	// don't forget second implementation!
}

void MainWindow::ui_check_rules_items()
{
	auto rules = m_pController->GetRules();
	m_pUi->actionRulesClassic->setChecked(rules->IsOfType<ClassicRules>());
	m_pUi->actionRules2013->setChecked(rules->IsOfType<Rules2013>());
	m_pUi->actionRules2017->setChecked(rules->IsOfType<Rules2017>());
	m_pUi->actionRules2017U15->setChecked(rules->IsOfType<Rules2017U15>());
	m_pUi->actionRules2018->setChecked(rules->IsOfType<Rules2018>());

	if (rules->IsOfType<ClassicRules>())
	{
		m_pUi->label_usedRules->setText(m_pUi->actionRulesClassic->text());
	}
	else if (rules->IsOfType<Rules2013>())
	{
		m_pUi->label_usedRules->setText(m_pUi->actionRules2013->text());
	}
	else if (rules->IsOfType<Rules2017>())
	{
		m_pUi->label_usedRules->setText(m_pUi->actionRules2017->text());
	}
	else if (rules->IsOfType<Rules2017U15>())
	{
		m_pUi->label_usedRules->setText(m_pUi->actionRules2017U15->text());
	}
	else if (rules->IsOfType<Rules2018>())
	{
		m_pUi->label_usedRules->setText(m_pUi->actionRules2018->text());
	}

	m_pPrimaryView->UpdateView();
	m_pSecondaryView->UpdateView();
}

void MainWindow::ui_check_show_secondary_view(bool checked) const
{
	m_pUi->actionShow_SecondaryView->setChecked(checked);
	m_pUi->toolButton_viewSecondaryScreen->setChecked(checked);
}

void MainWindow::on_actionAutoAdjustPoints_toggled(bool checked)
{
	MainWindowBase::on_actionAutoAdjustPoints_toggled(checked);

	ui_update_used_options();
}

void MainWindow::ui_update_used_options()
{
	QString text;

	if (m_pController->IsAutoAdjustPoints())
	{
		text += tr("Auto adjust points");
	}
	else
	{
		text = "-";
	}

	m_pUi->label_usedOptions->setText(text);

}

void MainWindow::on_actionViewInfoBar_toggled(bool checked)
{
	for (int i = 0; i < m_pUi->horizontalLayout_infoBar->count(); ++i)
	{
		auto item = m_pUi->horizontalLayout_infoBar->itemAt(i)->widget();

		if (item)
		{
			if (checked)
			{
				item->show();
			}
			else
			{
				item->hide();
			}
		}
	}

	if (checked)
	{
		m_pUi->line_infoBar->show();
	}
	else
	{
		m_pUi->line_infoBar->hide();
	}
}

void MainWindow::on_toolButton_viewSecondaryScreen_toggled()
{
	on_actionShow_SecondaryView_triggered();
}

void MainWindow::write_specific_settings(QSettings& settings)
{
	settings.beginGroup(EditionNameShort());
	{
		settings.remove("");
		settings.setValue(str_tag_MatLabel, m_MatLabel);
		settings.setValue(str_tag_rules, m_pController->GetRules()->Name());
	}
	settings.endGroup();
}

void MainWindow::read_specific_settings(QSettings& settings)
{
	settings.beginGroup(EditionNameShort());
	{
        m_MatLabel = settings.value(str_tag_MatLabel, "Ipponboard").toString(); // value is also in settings dialog!
		m_pPrimaryView->SetMat(m_MatLabel);
		m_pSecondaryView->SetMat(m_MatLabel);

		// rules
		auto rules = RulesFactory::Create(settings.value(str_tag_rules).toString());
		m_pController->SetRules(rules);
	}
	settings.endGroup();
}

void MainWindow::UpdateGoldenScoreView()
{
	m_pUi->checkBox_golden_score->setEnabled(m_pController->GetRules()->IsOption_OpenEndGoldenScore());
	m_pUi->checkBox_golden_score->setChecked(m_pController->IsGoldenScore());
}
