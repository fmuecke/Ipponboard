// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "AddAthleteDlg.h"

#include "../core/Athlete.h"
#include "ui_AddAthleteDlg.h"

using namespace Ipponboard;

AddAthleteDlg::AddAthleteDlg(QWidget* parent) : QDialog(parent), ui(new Ui::AddAthleteDlg)
{
    ui->setupUi(this);
}

AddAthleteDlg::~AddAthleteDlg() { delete ui; }

void AddAthleteDlg::SetClubs(QStringList clubs)
{
    ui->comboBox_club->addItems(clubs);
    ui->comboBox_club->setEnabled(clubs.size() > 1);
    ui->comboBox_club->setCurrentIndex(0);
}

Ipponboard::Athlete AddAthleteDlg::GetAthlete() const
{
    Athlete f(ui->lineEdit_firstname->text(), ui->lineEdit_lastname->text());
    //TODO: f.category = ui->comboBox_category->currentText();
    f.club = ui->comboBox_club->currentText();
    f.weight = ui->lineEdit_weight->text();

    return f;
}
