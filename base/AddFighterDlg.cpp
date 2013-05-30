// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#include "AddFighterDlg.h"
#include "ui_AddFighterDlg.h"
#include "../core/Fighter.h"

using namespace Ipponboard;

AddFighterDlg::AddFighterDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddFighterDlg)
{
    ui->setupUi(this);
}

AddFighterDlg::~AddFighterDlg()
{
    delete ui;
}

void AddFighterDlg::SetClubs(QStringList clubs)
{
    ui->comboBox_club->addItems(clubs);
    ui->comboBox_club->setEnabled(clubs.size() > 1);
    ui->comboBox_club->setCurrentIndex(0);
}

Ipponboard::Fighter AddFighterDlg::GetFighter() const
{
    Fighter f(ui->lineEdit_firstname->text(), ui->lineEdit_lastname->text());
    //TODO: f.category = ui->comboBox_category->currentText();
    f.club = ui->comboBox_club->currentText();
    f.weight = ui->lineEdit_weight->text();

    return f;
}
