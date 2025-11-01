// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ModeManagerDlg.h"
#include "ui_ModeManagerDlg.h"
#include "../core/Enums.h"
#include "../core/Rules.h"

#include <QComboBox>
#include <QStringList>
#include <QMessageBox>
#include <QUuid>

using namespace Ipponboard;

ModeManagerDlg::ModeManagerDlg(TournamentMode::List const& modes,
							   QStringList const& templates,
							   QString const& currentModeId,
							   QWidget* parent)
	: QDialog(parent)
	, fm::DialogResult<TournamentMode::List>(modes)
	, m_pUi(new Ui::ModeManagerDlg)
	, m_currentIndex(-1)
{
	std::sort(begin(m_dialogData), end(m_dialogData));

	m_pUi->setupUi(this);
	m_pUi->comboBox_template->addItems(templates);

	m_pUi->comboBox_rules->clear();
	m_pUi->comboBox_rules->addItems(RulesFactory::GetNames());

    m_DefaultMode = TournamentMode::Default();

	if (!m_dialogData.empty())
	{
		int pos = 0;

		for (size_t i = 0; i < m_dialogData.size(); ++i)
		{
			TournamentMode const& mode = m_dialogData[i];
			m_pUi->comboBox_mode->addItem(mode.Description(), QVariant(mode.id));

			if (mode.id == currentModeId)
			{
				pos = i;
			}
		}

		m_pUi->comboBox_mode->setCurrentIndex(pos);
	}
}

ModeManagerDlg::~ModeManagerDlg()
{
}

void ModeManagerDlg::on_comboBox_mode_currentIndexChanged(int i)
{
	m_currentIndex = i; // --> has_Mode() = true

	auto const& mode = GetCurrentMode();
	m_pUi->lineEdit_title->setText(mode.title);
	m_pUi->lineEdit_subtitle->setText(mode.subTitle);
	m_pUi->lineEdit_weights->setText(mode.weights);
	m_pUi->spinBox_rounds->setValue(mode.nRounds);
	m_pUi->spinBox_fightTimeMinutes->setValue(mode.fightTimeInSeconds / 60);
	m_pUi->spinBox_fightTimeSeconds->setValue(mode.fightTimeInSeconds % 60);
	m_pUi->checkBox_doubleWeights->setChecked(mode.weightsAreDoubled);
	update_fights_per_round(mode);

	auto templateIndex = m_pUi->comboBox_template->findText(mode.listTemplate);

	if (templateIndex != -1)
	{
		m_pUi->comboBox_template->setCurrentIndex(templateIndex);
	}

	if (!mode.fightTimeOverrides.empty())
	{
		m_pUi->checkBox_timeOverrides->setChecked(true);
		m_pUi->lineEdit_timeOverrides->setText(mode.GetFightTimeOverridesString());
	}
	else
	{
		m_pUi->checkBox_timeOverrides->setChecked(false);
		m_pUi->lineEdit_timeOverrides->setText(QString());
	}

	auto rulesIndex = m_pUi->comboBox_rules->findText(mode.rules);

	if (rulesIndex != -1)
	{
		m_pUi->comboBox_rules->setCurrentIndex(rulesIndex);
	}

	m_pUi->checkBox_allSubscoresCount->setChecked(mode.IsOptionSet(TournamentMode::str_Option_AllSubscoresCount));
}

void ModeManagerDlg::on_comboBox_template_currentIndexChanged(const QString& s)
{
	if (!has_Mode())
	{
		return;
	}

	auto& mode = GetCurrentMode();
	mode.listTemplate = s;
}

void ModeManagerDlg::on_comboBox_rules_currentIndexChanged(int /*i*/)
{
	if (!has_Mode())
	{
		return;
	}

	auto& mode = GetCurrentMode();
	mode.rules = m_pUi->comboBox_rules->currentText();
}

void ModeManagerDlg::on_checkBox_timeOverrides_toggled(bool checked)
{
	if (!has_Mode())
	{
		return;
	}

	m_pUi->lineEdit_timeOverrides->setEnabled(checked);
}

void ModeManagerDlg::on_checkBox_doubleWeights_toggled(bool checked)
{
	if (!has_Mode())
	{
		return;
	}

	auto& mode = GetCurrentMode();
	mode.weightsAreDoubled = checked;
	update_fights_per_round(mode);
}

void ModeManagerDlg::on_checkBox_allSubscoresCount_toggled(bool checked)
{
	if (!has_Mode())
	{
		return;
	}

	auto& mode = GetCurrentMode();
	mode.SetOption(TournamentMode::str_Option_AllSubscoresCount, checked);
}

void ModeManagerDlg::on_toolButton_add_clicked()
{
    auto mode = TournamentMode::Default();
	mode.title = tr("*new*");
	mode.listTemplate = m_pUi->comboBox_template->itemText(0);

	m_dialogData.push_back(mode);

	m_pUi->comboBox_mode->addItem(mode.Description(), QVariant(mode.id));
	auto pos = m_pUi->comboBox_mode->findData(QVariant(mode.id));
	m_pUi->comboBox_mode->setCurrentIndex(pos);

	m_pUi->toolButton_remove->setEnabled(m_pUi->comboBox_mode->count() > 1);
}

void ModeManagerDlg::on_toolButton_remove_clicked()
{
	auto answer = QMessageBox::question(
					  this,
					  tr("Remove item"),
					  tr("Really remove \"%1\"?").arg(m_pUi->comboBox_mode->currentText()),
					  tr("Remove"),
					  tr("Keep"));

	if (answer == 0)
	{
		auto id = m_pUi->comboBox_mode->itemData(m_currentIndex).toString();
		auto pos = std::find_if(begin(m_dialogData), end(m_dialogData), [&](TournamentMode const & mode)
		{ return mode.id == id; });

		if (pos != end(m_dialogData))
		{
			m_dialogData.erase(pos);
			m_pUi->comboBox_mode->removeItem(m_currentIndex);
		}

		m_pUi->toolButton_remove->setEnabled(m_pUi->comboBox_mode->count() > 1);
	}
}

void ModeManagerDlg::on_spinBox_rounds_valueChanged(int i)
{
	if (!has_Mode())
	{
		return;
	}

	auto& mode = GetCurrentMode();
	mode.nRounds = i;
	update_fights_per_round(mode);
}

void ModeManagerDlg::on_spinBox_fightTimeMinutes_valueChanged(int i)
{
	if (!has_Mode())
	{
		return;
	}

	auto& mode = GetCurrentMode();
	mode.fightTimeInSeconds = i * 60 + m_pUi->spinBox_fightTimeSeconds->value();
}

void ModeManagerDlg::on_spinBox_fightTimeSeconds_valueChanged(int i)
{
	if (!has_Mode())
	{
		return;
	}

	auto& mode = GetCurrentMode();
	mode.fightTimeInSeconds = m_pUi->spinBox_fightTimeMinutes->value() * 60 + i;
}

void ModeManagerDlg::on_lineEdit_weights_textChanged(const QString& s)
{
	if (!has_Mode())
	{
		return;
	}

	auto& mode = GetCurrentMode();
	mode.weights = s;
	update_fights_per_round(mode);
}

void ModeManagerDlg::on_lineEdit_title_textChanged(const QString& s)
{
	if (!has_Mode())
	{
		return;
	}

	auto& mode = GetCurrentMode();
	mode.title = s;
	m_pUi->comboBox_mode->setItemText(m_currentIndex, mode.Description());
}

void ModeManagerDlg::on_lineEdit_subtitle_textChanged(const QString& s)
{
	if (!has_Mode())
	{
		return;
	}

	auto& mode = GetCurrentMode();
	mode.subTitle = s;
	m_pUi->comboBox_mode->setItemText(m_currentIndex, mode.Description());
}

void ModeManagerDlg::on_lineEdit_timeOverrides_textChanged(const QString& s)
{
	if (!has_Mode())
	{
		return;
	}

	auto& mode = GetCurrentMode();

	if (s.isEmpty() || TournamentMode::ExtractFightTimeOverrides(s, mode.fightTimeOverrides))
	{
		m_pUi->lineEdit_timeOverrides->setStyleSheet("color : black;");
	}
	else
	{
		m_pUi->lineEdit_timeOverrides->setStyleSheet("color : red;");
	}
}

void ModeManagerDlg::update_fights_per_round(const TournamentMode& mode)
{
    auto text = mode.nRounds > 1 ? QString("%1 fights total, %2 per round")
                                       .arg(mode.FightsPerRound() * mode.nRounds)
                                       .arg(mode.FightsPerRound());
            : QString("%1 fights total").arg(mode.FightsPerRound());

	m_pUi->label_fightsPerRound->setText(text);
}

Ipponboard::TournamentMode& ModeManagerDlg::GetMode(int i)
{
	QString id = m_pUi->comboBox_mode->itemData(i).toString();

	for (auto & mode : m_dialogData)
	{
		if (mode.id == id)
		{
			return mode;
		}
	}

    return m_DefaultMode;

}
