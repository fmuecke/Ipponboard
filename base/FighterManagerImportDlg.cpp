// Copyright 2010-2014 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#include "FighterManagerImportDlg.h"
#include "ui_FighterManagerImportDlg.h"
#include "FighterManager.h"
#include <QTextStream>

const char* str_Tab = "Tab";

using namespace Ipponboard;

FighterManagerImportDlg::FighterManagerImportDlg(QString const& filePath, QStringList const& clubs, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::FighterManagerImportDlg)
{
    ui->setupUi(this);

    ui->comboBox_clubs->addItems(clubs);
    if (clubs.size() == 1)
    {
        ui->checkBox_importToClub->setChecked(true);
    }

    m_separators << ";" << "," << ":" << "|" << str_Tab;
	m_currentSeparator = ";";
    ui->comboBox_separator->addItems(m_separators);
    ui->comboBox_separator->setCurrentIndex(0);

    ui->label_path->setText(filePath);

    QFile file(filePath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);

        QString fileContent;
        while (!in.atEnd())
        {
            QString line = in.readLine();
            fileContent.append(line + "\n");
        }

        file.close();
        ui->plainTextEdit_content->setPlainText(fileContent);
    }
    else
    {
        ui->plainTextEdit_content->setPlainText("Unable to read file!");
    }

    auto headerItem = ui->treeWidget_preview->headerItem();
    headerItem->setText(eColumn_Lastname, FighterManager::str_lastname);
    headerItem->setText(eColumn_Firstname, FighterManager::str_firstname);
    headerItem->setText(eColumn_Club, FighterManager::str_club);
    //headerItem->setText(eColumn_Team, FighterManager::str_team);
    headerItem->setText(eColumn_Year, FighterManager::str_year);
    headerItem->setText(eColumn_Nation, FighterManager::str_nation);
    headerItem->setText(eColumn_Weight, FighterManager::str_weight);
    headerItem->setText(eColumn_Category, FighterManager::str_category);
	
	ui->label_parse_error->hide();
	ui->toolButton_lastname->toggle();
	ui->toolButton_firstname->toggle();
}

FighterManagerImportDlg::~FighterManagerImportDlg()
{
    delete ui;
}

QString FighterManagerImportDlg::GetFormatStr() const
{
    return ui->lineEdit_specifiers->text();
}

QString FighterManagerImportDlg::GetSelectedClub() const
{
    if (ui->checkBox_importToClub->isChecked())
    {
        return ui->comboBox_clubs->currentText();
    }

    return QString();
}

void FighterManagerImportDlg::ToggleSpecifier(const QString& specifier, bool checked)
{
    auto text = ui->lineEdit_specifiers->text();
    if (checked)
    {
        if (!text.isEmpty())
        {
            text.append(m_currentSeparator);
        }
        text.append(specifier);
    }
    else
    {
        while(text.contains(specifier))
        {
            text.remove(m_currentSeparator + specifier);
            text.remove(specifier + m_currentSeparator);
            text.remove(specifier);
        }
    }
    ui->lineEdit_specifiers->setText(text);
}

void FighterManagerImportDlg::on_toolButton_firstname_toggled(bool checked)
{
    ToggleSpecifier(Ipponboard::FighterManager::str_firstname, checked);
}

void FighterManagerImportDlg::on_toolButton_lastname_toggled(bool checked)
{
    ToggleSpecifier(Ipponboard::FighterManager::str_lastname, checked);
}

void FighterManagerImportDlg::on_toolButton_nation_toggled(bool checked)
{
    ToggleSpecifier(Ipponboard::FighterManager::str_nation, checked);
}

void FighterManagerImportDlg::on_toolButton_year_toggled(bool checked)
{
    ToggleSpecifier(Ipponboard::FighterManager::str_year, checked);
}

void FighterManagerImportDlg::on_toolButton_club_toggled(bool checked)
{
    ToggleSpecifier(Ipponboard::FighterManager::str_club, checked);
}

//void FighterManagerImportDlg::on_toolButton_team_toggled(bool checked)
//{
//    ToggleSpecifier(Ipponboard::FighterManager::str_team, checked);
//}

void FighterManagerImportDlg::on_toolButton_category_toggled(bool checked)
{
    ToggleSpecifier(Ipponboard::FighterManager::str_category, checked);
}

void FighterManagerImportDlg::on_toolButton_weight_toggled(bool checked)
{
    ToggleSpecifier(Ipponboard::FighterManager::str_weight, checked);
}

void FighterManagerImportDlg::on_comboBox_separator_currentIndexChanged(const QString &s)
{
    QString text = ui->lineEdit_specifiers->text();
    if (s == str_Tab)
    {
        text.replace(m_currentSeparator, "\t");
        m_currentSeparator = "\t";
    }
    else
    {
        text.replace(m_currentSeparator, s);
        m_currentSeparator = s;
    }

    ui->lineEdit_specifiers->setText(text);
}

void FighterManagerImportDlg::on_lineEdit_specifiers_textChanged(const QString& s)
{
    ui->treeWidget_preview->clear();
    QString errorMsg;
    Ipponboard::FighterManager fighterManager;

    if (fighterManager.ImportFighters(ui->label_path->text(), s, m_currentSeparator, errorMsg))
    {
        for (Ipponboard::Fighter const& fighter : fighterManager.m_fighters)
        {
            QTreeWidgetItem* pItem = new QTreeWidgetItem(QTreeWidgetItem::UserType);
            pItem->setText(eColumn_Lastname, ui->toolButton_lastname->isChecked()? fighter.last_name : "");
            pItem->setText(eColumn_Firstname, ui->toolButton_firstname->isChecked()? fighter.first_name : "");
            if (ui->checkBox_importToClub->isChecked())
            {
                pItem->setText(eColumn_Club, ui->comboBox_clubs->currentText());
            }
            else
            {
                pItem->setText(eColumn_Club, ui->toolButton_club->isChecked()? fighter.club : "");
            }
            //pItem->setText(eColumn_Team, ui->toolButton_team->isChecked()? fighter.team : "");
            pItem->setText(eColumn_Nation, ui->toolButton_nation->isChecked()? fighter.nation : "");
            pItem->setText(eColumn_Year, ui->toolButton_year->isChecked()? fighter.year : "");
            pItem->setText(eColumn_Category, ui->toolButton_category->isChecked()? fighter.category : "");
            pItem->setText(eColumn_Weight, ui->toolButton_weight->isChecked()? fighter.weight : "");
            ui->treeWidget_preview->addTopLevelItem(pItem);
        }

        ui->label_parse_error->hide();
    }
    else
    {
        ui->label_parse_error->setText(errorMsg);
        ui->label_parse_error->show();
    }
}

void FighterManagerImportDlg::on_checkBox_importToClub_toggled(bool checked)
{
    if (checked && ui->toolButton_club->isChecked())
    {
        ui->toolButton_club->toggle();
    }

    ui->toolButton_club->setEnabled(!checked);
}
