// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#include <QDialog>

namespace Ui
{
class AddAthleteDlg;
}

namespace Ipponboard
{
class Athlete;
}

class AddAthleteDlg : public QDialog
{
    Q_OBJECT

  public:
    explicit AddAthleteDlg(QWidget* parent = nullptr);
    ~AddAthleteDlg();

    void SetClubs(QStringList clubs);
    Ipponboard::Athlete GetAthlete() const;

  private:
    Ui::AddAthleteDlg* ui;
};
