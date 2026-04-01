// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ClubManagerDlg.h"

#include "../util/path_helpers.h"
#include "../util/theme_colors.h"
#include "ClubManager.h"
#include "ui_ClubManagerDlg.h"

#include <QFileDialog>
#include <QMessageBox>
#include <memory>

using namespace Ipponboard;

namespace
{
QString clubsConfigFilePath() { return fm::GetConfigFilePath(ClubManager::str_clubs_settingsFile); }

QString resolveClubLogoPath(const QString& storedPath)
{
    return fm::ResolveConfigOwnedAsset(clubsConfigFilePath(), storedPath);
}

QString defaultClubLogoPath() { return resolveClubLogoPath(QStringLiteral("clubs/default.png")); }
} // namespace

//---------------------------------------------------------
ClubManagerDlg::ClubManagerDlg(std::shared_ptr<ClubManager> pMgr, QWidget* parent)
    : QDialog(parent), ui(new Ui::ClubManagerDlg), m_pClubMgr(pMgr), m_SelectedClub(-1)
//---------------------------------------------------------
{
    ui->setupUi(this);

    Q_ASSERT(m_pClubMgr);

    // load club data
    if (m_pClubMgr->ClubCount() > 0)
    {
        // add clubs
        for (int i = 0; i != m_pClubMgr->ClubCount(); ++i)
        {
            Ipponboard::Club club;
            m_pClubMgr->GetClub(i, club);
            ui->comboBox_club->addItem(club.name);
        }

        ui->comboBox_club->setCurrentIndex(0); //SelectClub_(0);
    }

    update_ui();
}

//---------------------------------------------------------
ClubManagerDlg::~ClubManagerDlg()
//---------------------------------------------------------
{
    // we need to resort the clubs because there might be
    // new ones
    m_pClubMgr->SortClubs();

    delete ui;
}

//---------------------------------------------------------
void ClubManagerDlg::changeEvent(QEvent* e)
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
void ClubManagerDlg::select_club(int index)
//---------------------------------------------------------
{
    m_SelectedClub = index;

    if (m_SelectedClub != -1)
    {
        Ipponboard::Club club;

        if (m_pClubMgr->GetClub(index, club))
        {
            ui->lineEdit_name->setText(club.name);
            ui->lineEdit_logoFile->setText(club.logoFile);
            ui->lineEdit_address->setText(club.address);
            //TODO: why was this line removed?
            //ui->scaledImage_logo->UpdateImage(club.logoFile);
        }
    }

    update_ui();
}

//---------------------------------------------------------
void ClubManagerDlg::update_ui()
//---------------------------------------------------------
{
    const QString fileName = ui->lineEdit_logoFile->text();
    const QString resolvedFileName = resolveClubLogoPath(fileName);
    QPalette palette(ui->lineEdit_logoFile->palette());

    if (QFile::exists(resolvedFileName))
    {
        palette.setColor(QPalette::Text, fm::GetThemeTextColor(ui->lineEdit_logoFile));
        ui->scaledImage_logo->UpdateImage(resolvedFileName);
    }
    else
    {
        palette.setColor(QPalette::Text, fm::GetValidationErrorColor(ui->lineEdit_logoFile));
        ui->scaledImage_logo->UpdateImage(defaultClubLogoPath());
    }

    ui->lineEdit_logoFile->setPalette(palette);
    //	if( 0 == m_pClubMgr->ClubCount() )
    //	{
    //		ui->pushButton_remove->setEnabled(false);
    //		ui->pushButton_update->setEnabled(false);
    //		ui->lineEdit_name->clear();
    //		ui->lineEdit_name->setEnabled(false);
    //	}
    //	else
    //	{
    //		ui->lineEdit_name->setEnabled(true);
    //		ui->pushButton_remove->setEnabled(true);
    //		ui->pushButton_update->setEnabled(true);
    //	}
}

//---------------------------------------------------------
void ClubManagerDlg::on_pushButton_add_pressed()
//---------------------------------------------------------
{
    // add empty club
    Ipponboard::Club club("--> new <--", "clubs/default.png");
    m_pClubMgr->AddClub(club);
    ui->comboBox_club->addItem(club.name);
    int index = ui->comboBox_club->findText(club.name);
    ui->comboBox_club->setCurrentIndex(index);

    m_SelectedClub = index;
    //UpdateButtonsAndText_();
}

//---------------------------------------------------------
void ClubManagerDlg::on_pushButton_save_pressed()
//---------------------------------------------------------
{
    if (m_SelectedClub < 0)
        return;

    Ipponboard::Club club;
    club.name = ui->lineEdit_name->text();
    club.logoFile = ui->lineEdit_logoFile->text();
    club.address = ui->lineEdit_address->text();

    // save club
    m_pClubMgr->UpdateClub(m_SelectedClub, club);

    // update club view in list
    ui->comboBox_club->setItemText(m_SelectedClub, club.name);
}

//---------------------------------------------------------
void ClubManagerDlg::on_pushButton_remove_pressed()
//---------------------------------------------------------
{
    if (m_SelectedClub < 0)
        return;

    m_pClubMgr->RemoveClub(m_SelectedClub);
    ui->comboBox_club->removeItem(m_SelectedClub);
}

//---------------------------------------------------------
void ClubManagerDlg::on_comboBox_club_currentIndexChanged(int index)
//---------------------------------------------------------
{
    select_club(index);
    update_ui();
}

//---------------------------------------------------------
void ClubManagerDlg::on_lineEdit_logoFile_textEdited(QString const& /*fileName*/)
//---------------------------------------------------------
{
    update_ui();
}

//---------------------------------------------------------
void ClubManagerDlg::on_pushButton_browseLogo_pressed()
//---------------------------------------------------------
{
    if (m_SelectedClub < 0)
        return;

    QString fileName =
        QFileDialog::getOpenFileName(this,
                                     tr("Select Club Emblem"),
                                     resolveClubLogoPath(ui->lineEdit_logoFile->text()),
                                     tr("PNG files (*.png);;Image files (*.png *.xpm *.jpg)"),
                                     nullptr,
                                     QFileDialog::ReadOnly);

    if (!fileName.isEmpty())
    {
        QIcon icon(fileName);

        if (icon.availableSizes().empty())
        {
            QMessageBox::critical(
                this, tr("Unsupported Image Type"), tr("The specified image could not be loaded!"));
        }
        else
        {
            const QDir configDir(fm::GetConfigDir());
            const auto absoluteFilePath = QFileInfo(fileName).absoluteFilePath();

            if (absoluteFilePath.startsWith(configDir.absolutePath() + QDir::separator()) ||
                absoluteFilePath == configDir.absolutePath())
            {
                fileName = configDir.relativeFilePath(absoluteFilePath);
            }
            else
            {
                fileName = absoluteFilePath;
            }

            ui->lineEdit_logoFile->setText(fileName);
            ui->scaledImage_logo->UpdateImage(resolveClubLogoPath(fileName));
        }
    }
}
