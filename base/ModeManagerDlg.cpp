// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ModeManagerDlg.h"

#include "../core/Enums.h"
#include "../core/Rules.h"
#include "../util/theme_colors.h"
#include "ui_ModeManagerDlg.h"

#include <QApplication>
#include <QComboBox>
#include <QMessageBox>
#include <QPalette>
#include <QStringList>
#include <QUuid>

using namespace Ipponboard;

ModeManagerDlg::ModeManagerDlg(CompetitionMode::List const& modes, QStringList const& templates,
                               QString const& currentModeId, QWidget* parent)
    : QDialog(parent),
      fm::DialogResult<CompetitionMode::List>(modes),
      m_pUi(new Ui::ModeManagerDlg),
      m_currentIndex(-1)
{
    std::sort(begin(m_dialogData), end(m_dialogData));

    m_pUi->setupUi(this);
    m_pUi->comboBox_template->addItems(templates);

    m_pUi->comboBox_rules->clear();
    m_pUi->comboBox_rules->addItems(RulesFactory::GetNames());

    m_DefaultMode = CompetitionMode::Default();

    if (!m_dialogData.empty())
    {
        int pos = 0;

        for (size_t i = 0; i < m_dialogData.size(); ++i)
        {
            CompetitionMode const& mode = m_dialogData[i];
            m_pUi->comboBox_mode->addItem(mode.Description(), QVariant(mode.id));

            if (mode.id == currentModeId)
            {
                pos = i;
            }
        }

        m_pUi->comboBox_mode->setCurrentIndex(pos);
    }
}

ModeManagerDlg::~ModeManagerDlg() {}

void ModeManagerDlg::on_comboBox_mode_currentIndexChanged(int i)
{
    m_currentIndex = i; // --> has_Mode() = true

    auto const& mode = GetCurrentMode();
    m_pUi->lineEdit_title->setText(mode.title);
    m_pUi->lineEdit_subtitle->setText(mode.subTitle);
    m_pUi->lineEdit_weights->setText(mode.weights);
    m_pUi->spinBox_rounds->setValue(mode.nRounds);
    m_pUi->spinBox_contestTimeMinutes->setValue(mode.timeInSeconds / 60);
    m_pUi->spinBox_contestTimeSeconds->setValue(mode.timeInSeconds % 60);
    m_pUi->checkBox_doubleWeights->setChecked(mode.weightsAreDoubled);
    update_contests_per_round(mode);

    auto templateIndex = m_pUi->comboBox_template->findText(mode.listTemplate);

    if (templateIndex != -1)
    {
        m_pUi->comboBox_template->setCurrentIndex(templateIndex);
    }

    if (!mode.contestTimeOverrides.empty())
    {
        m_pUi->checkBox_timeOverrides->setChecked(true);
        m_pUi->lineEdit_timeOverrides->setText(mode.GetTimeOverridesString());
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

    m_pUi->checkBox_allSubscoresCount->setChecked(
        mode.IsOptionSet(CompetitionMode::str_Option_AllSubscoresCount));
}

void ModeManagerDlg::on_comboBox_template_currentTextChanged(const QString& s)
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
    update_contests_per_round(mode);
}

void ModeManagerDlg::on_checkBox_allSubscoresCount_toggled(bool checked)
{
    if (!has_Mode())
    {
        return;
    }

    auto& mode = GetCurrentMode();
    mode.SetOption(CompetitionMode::str_Option_AllSubscoresCount, checked);
}

void ModeManagerDlg::on_toolButton_add_clicked()
{
    auto mode = CompetitionMode::Default();
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
    QMessageBox msgBox(QMessageBox::Question,
                       tr("Remove item"),
                       tr("Really remove \"%1\"?").arg(m_pUi->comboBox_mode->currentText()),
                       QMessageBox::Yes | QMessageBox::No,
                       this);
    msgBox.setDefaultButton(QMessageBox::Yes);

    if (auto* yesButton = msgBox.button(QMessageBox::Yes))
    {
        yesButton->setText(tr("Remove"));
    }

    if (auto* noButton = msgBox.button(QMessageBox::No))
    {
        noButton->setText(tr("Keep"));
    }

    const auto answer = msgBox.exec();

    if (answer == QMessageBox::Yes)
    {
        auto id = m_pUi->comboBox_mode->itemData(m_currentIndex).toString();
        auto pos = std::find_if(begin(m_dialogData),
                                end(m_dialogData),
                                [&](CompetitionMode const& mode) { return mode.id == id; });

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
    update_contests_per_round(mode);
}

void ModeManagerDlg::on_spinBox_contestTimeMinutes_valueChanged(int i)
{
    if (!has_Mode())
    {
        return;
    }

    auto& mode = GetCurrentMode();
    mode.timeInSeconds = i * 60 + m_pUi->spinBox_contestTimeSeconds->value();
}

void ModeManagerDlg::on_spinBox_contestTimeSeconds_valueChanged(int i)
{
    if (!has_Mode())
    {
        return;
    }

    auto& mode = GetCurrentMode();
    mode.timeInSeconds = m_pUi->spinBox_contestTimeMinutes->value() * 60 + i;
}

void ModeManagerDlg::on_lineEdit_weights_textChanged(const QString& s)
{
    if (!has_Mode())
    {
        return;
    }

    auto& mode = GetCurrentMode();
    mode.weights = s;
    update_contests_per_round(mode);
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
    QPalette palette(m_pUi->lineEdit_timeOverrides->palette());

    if (s.isEmpty() || CompetitionMode::ExtractTimeOverrides(s, mode.contestTimeOverrides))
    {
        palette.setColor(
            QPalette::Text,
            QApplication::palette(m_pUi->lineEdit_timeOverrides).color(QPalette::Text));
    }
    else
    {
        palette.setColor(QPalette::Text,
                         fm::GetValidationErrorColor(m_pUi->lineEdit_timeOverrides));
    }

    m_pUi->lineEdit_timeOverrides->setPalette(palette);
}

void ModeManagerDlg::update_contests_per_round(const CompetitionMode& mode)
{
    auto text = mode.nRounds > 1 ? tr("%1 contests total, %2 per round")
                                       .arg(mode.ContestsPerRound() * mode.nRounds)
                                       .arg(mode.ContestsPerRound())
                                 : tr("%1 contests total").arg(mode.ContestsPerRound());

    m_pUi->label_contestsPerRound->setText(text);
}

Ipponboard::CompetitionMode& ModeManagerDlg::GetMode(int i)
{
    QString id = m_pUi->comboBox_mode->itemData(i).toString();

    for (auto& mode : m_dialogData)
    {
        if (mode.id == id)
        {
            return mode;
        }
    }

    return m_DefaultMode;
}
