///////////////////////////////////////////////////////////////////////////////
// $Id$
///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2010-2014 Florian Muecke - (ipponboardinfo at googlemail dot com)
// All rights reserved.  Web: http://www.ipponboard.info
//
// This file is part of the Ipponboard project.
//
// It may not be distributed to or shared with the public in any form!
///////////////////////////////////////////////////////////////////////////////


#include "ModeManagerDlg.h"
#include "ui_ModeManagerDlg.h"
#include "..\core\Enums.h"
#include "..\core\EnumStrings.h"

#include <QComboBox>
#include <QStringList>
#include <QMessageBox>
#include <QUuid>
#include <QRegExpValidator>

using namespace Ipponboard;

ModeManagerDlg::ModeManagerDlg(
        TournamentMode::List const& modes,
        QStringList const& templates,
        QWidget *parent)
    : QDialog(parent)
    , FMU::DialogResult<TournamentMode::List>(modes)
    , m_pUi(new Ui::ModeManagerDlg)
{
    std::sort(begin(DialogData()), end(DialogData()));

    m_pUi->setupUi(this);

    if (!DialogData().empty())
    {
        m_pUi->comboBox_template->addItems(templates);

        for (auto const& mode : DialogData())
        {
            m_pUi->comboBox_mode->addItem(mode.FullTitle());
        }

        m_pUi->comboBox_mode->setCurrentIndex(0);
    }
}

ModeManagerDlg::~ModeManagerDlg()
{
}

void ModeManagerDlg::on_comboBox_mode_currentIndexChanged(const QString &s)
{
    auto const& mode = GetMode(s);
    m_pUi->lineEdit_title->setText(mode.title);
    m_pUi->lineEdit_subtitle->setText(mode.subTitle);
    m_pUi->lineEdit_weights->setText(mode.weights);
    m_pUi->spinBox_rounds->setValue(mode.nRounds);
    m_pUi->spinBox_fightTime->setValue(mode.fightTimeInSeconds);
    m_pUi->spinBox_fightsPerRound->setValue(mode.FightsPerRound());
    m_pUi->checkBox_doubleWeights->setChecked(mode.weightsAreDoubled);

    auto index = m_pUi->comboBox_template->findText(mode.listTemplate);
    if (index != -1)
    {
        m_pUi->comboBox_template->setCurrentIndex(index);
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

    m_pUi->checkBox_2013Rules->setChecked(mode.IsOptionSet(eOption_Use2013Rules));
    m_pUi->checkBox_autoIncrement->setChecked(mode.IsOptionSet(eOption_AutoIncrementPoints));
    m_pUi->checkBox_allSubscoresCount->setChecked(mode.IsOptionSet(eOption_AllSubscoresCount));
}

void ModeManagerDlg::on_comboBox_template_currentIndexChanged(const QString &s)
{
    auto& mode = GetMode(m_pUi->comboBox_mode->currentText());
    mode.listTemplate = s;
}

void ModeManagerDlg::on_checkBox_timeOverrides_toggled(bool checked)
{
    m_pUi->lineEdit_timeOverrides->setEnabled(checked);
}

void ModeManagerDlg::on_checkBox_doubleWeights_toggled(bool checked)
{
    auto& mode = GetMode(m_pUi->comboBox_mode->currentText());
    mode.weightsAreDoubled = checked;
    m_pUi->spinBox_fightsPerRound->setValue(mode.FightsPerRound());
}

void ModeManagerDlg::on_checkBox_2013Rules_toggled(bool checked)
{
    auto& mode = GetMode(m_pUi->comboBox_mode->currentText());
    mode.SetOption(eOption_Use2013Rules, checked);
}

void ModeManagerDlg::on_checkBox_autoIncrement_toggled(bool checked)
{
    auto& mode = GetMode(m_pUi->comboBox_mode->currentText());
    mode.SetOption(eOption_AutoIncrementPoints, checked);
}

void ModeManagerDlg::on_checkBox_allSubscoresCount_toggled(bool checked)
{
    auto& mode = GetMode(m_pUi->comboBox_mode->currentText());
    mode.SetOption(eOption_AllSubscoresCount, checked);
}

void ModeManagerDlg::on_toolButton_add_clicked()
{
    TournamentMode mode;
    mode.name = QUuid::createUuid().toString();
    mode.name = mode.name.mid(1, mode.name.length()-2);  // remove "{}"
    mode.title = tr("*new*");
    mode.weights = "-66;-73;-81;-90;+90";
    mode.fightTimeInSeconds = 300;
    mode.nRounds = 2;
    mode.weightsAreDoubled = true;
    mode.SetOption(eOption_Use2013Rules, true);

    while (m_pUi->comboBox_mode->findText(mode.FullTitle()) != -1)
    {
        mode.subTitle.append("*");
    }

    DialogData().push_back(mode);

    m_pUi->comboBox_mode->addItem(mode.FullTitle());
    auto pos = m_pUi->comboBox_mode->findText(mode.FullTitle());
    m_pUi->comboBox_mode->setCurrentIndex(pos);
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
        auto pos = std::find_if(begin(DialogData()), end(DialogData()),
                             [&](TournamentMode const& mode)
        {
            return mode.title == m_pUi->comboBox_mode->currentText();
        });

        if (pos != end(DialogData()))
        {
            DialogData().erase(pos);
        }

        m_pUi->comboBox_mode->removeItem(m_pUi->comboBox_mode->currentIndex());
    }
}

void ModeManagerDlg::on_spinBox_rounds_valueChanged(int i)
{
    auto& mode = GetMode(m_pUi->comboBox_mode->currentText());
    mode.nRounds = i;
}

void ModeManagerDlg::on_spinBox_fightTime_valueChanged(int i)
{
    auto& mode = GetMode(m_pUi->comboBox_mode->currentText());
    mode.fightTimeInSeconds = i;
}

void ModeManagerDlg::on_lineEdit_weights_textChanged(const QString &s)
{
    auto& mode = GetMode(m_pUi->comboBox_mode->currentText());
    mode.weights = s;
    m_pUi->spinBox_fightsPerRound->setValue(mode.FightsPerRound());
}

void ModeManagerDlg::on_lineEdit_title_textChanged(const QString &s)
{
     auto& mode = GetMode(m_pUi->comboBox_mode->currentText());
     mode.title = s;
     m_pUi->comboBox_mode->setItemText(m_pUi->comboBox_mode->currentIndex(), mode.FullTitle());
}

void ModeManagerDlg::on_lineEdit_subtitle_textChanged(const QString &s)
{
    auto& mode = GetMode(m_pUi->comboBox_mode->currentText());
    mode.subTitle = s;
    m_pUi->comboBox_mode->setItemText(m_pUi->comboBox_mode->currentIndex(), mode.FullTitle());
}

void ModeManagerDlg::on_lineEdit_timeOverrides_textChanged(const QString &s)
{
    auto& mode = GetMode(m_pUi->comboBox_mode->currentText());
    if (s.isEmpty() || TournamentMode::ExtractFightTimeOverrides(s, mode.fightTimeOverrides))
    {
        m_pUi->lineEdit_timeOverrides->setStyleSheet("color : black;");
    }
    else
    {
        m_pUi->lineEdit_timeOverrides->setStyleSheet("color : red;");
    }
}

Ipponboard::TournamentMode& ModeManagerDlg::GetMode(const QString &s)
{
    for (auto & mode : DialogData())
    {
        if (mode.FullTitle() == s)
        {
            return mode;
        }
    }

    return std::move(TournamentMode());
}
