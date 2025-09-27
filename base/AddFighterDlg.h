// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef ADDFIGHTERDLG_H
#define ADDFIGHTERDLG_H

#include <QDialog>

namespace Ui
{
class AddFighterDlg;
}

namespace Ipponboard
{
class Fighter;
}

class AddFighterDlg : public QDialog
{
	Q_OBJECT

  public:
	explicit AddFighterDlg(QWidget* parent = nullptr);
	~AddFighterDlg();

	void SetClubs(QStringList clubs);
	Ipponboard::Fighter GetFighter() const;

  private:
	Ui::AddFighterDlg* ui;
};

#endif // ADDFIGHTERDLG_H
