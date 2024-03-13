// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/debug.h"
#include "AddFighterDlg.h"
#include "ui_AddFighterDlg.h"
#include "../core/Fighter.h"

using namespace Ipponboard;

AddFighterDlg::AddFighterDlg(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::AddFighterDlg)
{
    TRACE(2, "AddFighterDlg::AddFighterDlg(parent=%s)", parent->accessibleName().toUtf8().data());
    ui->setupUi(this);
}

AddFighterDlg::~AddFighterDlg()
{
    TRACE(2, "AddFighterDlg::~AddFighterDlg()");
    delete ui;
}

void AddFighterDlg::SetClubs(QStringList clubs)
{
    TRACE(2, "AddFighterDlg::SetClubs()");
    ui->comboBox_club->addItems(clubs);
	ui->comboBox_club->setEnabled(clubs.size() > 1);
	ui->comboBox_club->setCurrentIndex(0);
}

Ipponboard::Fighter AddFighterDlg::GetFighter() const
{
    TRACE(2, "AddFighterDlg::GetFighter()");
    Fighter f(ui->lineEdit_firstname->text(), ui->lineEdit_lastname->text());
	//TODO: f.category = ui->comboBox_category->currentText();
	f.club = ui->comboBox_club->currentText();
	f.weight = ui->lineEdit_weight->text();

	return f;
}
