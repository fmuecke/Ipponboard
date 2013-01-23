// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
// $Id$

#include "maindlg.h"
#include "ui_maindlg.h"
#include <QProcess>
#include <QFile>
#include <QMessageBox>

MainDlg::MainDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDlg)
{
#ifndef _DEBUG
    QString locale = QLocale::system().name();
    locale.truncate(locale.lastIndexOf('_'));
    if (locale == "de")
    {
        // remove the old translator
        qApp->removeTranslator(&m_translator);

        // load the new translator
        if(m_translator.load("lang/VersionSelector_de.qm"))
            qApp->installTranslator(&m_translator);
    }
#endif

    ui->setupUi(this);
}

MainDlg::~MainDlg()
{
    delete ui;
}

void MainDlg::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void MainDlg::StartApp(QString const& fileName)
{
    if (QFile::exists(fileName))
    {
        if (QProcess::startDetached(fileName))
        {
            accept();
        }
        else
        {
            QMessageBox::critical(this,
                                  tr("Error"),
                                  QString(tr("%1 could not be started!")).arg(fileName));
        }
    }
    else
    {
        QMessageBox::critical(this,
                              tr("Error"),
                              QString(tr("%1 could not be found!")).arg(fileName));
    }
}


void MainDlg::on_commandLinkButton_single_clicked()
{
    StartApp("Ipponboard.exe");
}

void MainDlg::on_commandLinkButton_team_clicked()
{
    StartApp("Ipponboard_team.exe");
}
