// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
// $Id$

#ifndef MAINDLG_H
#define MAINDLG_H

#include <QDialog>
#include <QTranslator>

namespace Ui {
class MainDlg;
}

class MainDlg : public QDialog
{
    Q_OBJECT

public:
    explicit MainDlg(QWidget *parent = 0);
    ~MainDlg();

protected:
    void changeEvent(QEvent *e);

private slots:
    void on_commandLinkButton_single_clicked();
    void on_commandLinkButton_team_clicked();

private:
    void StartApp(QString const& fileName);
    void switchTranslator(QTranslator& translator, const QString& filename);

    Ui::MainDlg *ui;
    QTranslator m_translator;   ///< contains the translations for this application
    //QTranslator m_translatorQt; ///< contains the translations for qt
};

#endif // MAINDLG_H
