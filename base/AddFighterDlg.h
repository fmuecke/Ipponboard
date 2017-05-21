// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

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
	explicit AddFighterDlg(QWidget* parent = 0);
	~AddFighterDlg();

	void SetClubs(QStringList clubs);
	Ipponboard::Fighter GetFighter() const;

private:
	Ui::AddFighterDlg* ui;
};

#endif // ADDFIGHTERDLG_H
