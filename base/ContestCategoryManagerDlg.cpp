// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "ContestCategoryManagerDlg.h"

#include "../util/theme_colors.h"
#include "ui_ContestCategoryManagerDlg.h"

#include <QInputDialog>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QRegularExpression>

using namespace Ipponboard;

//---------------------------------------------------------
ContestCategoryManagerDlg::ContestCategoryManagerDlg(Ipponboard::ContestCategoryMgr::Ptr pMgr,
                                                     QWidget* parent)
    : QDialog(parent), ui(new Ui::ContestCategoryManagerDlg), m_pClassMgr(pMgr)
//---------------------------------------------------------
{
    ui->setupUi(this);

    // NOTE: This is nasty workaround for the standard buttons not
    // beeing translated (separate translator would be required)
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));

    Q_ASSERT(m_pClassMgr);

    load_values();
}

//---------------------------------------------------------
ContestCategoryManagerDlg::~ContestCategoryManagerDlg()
//---------------------------------------------------------
{
    delete ui;
}

//---------------------------------------------------------
void ContestCategoryManagerDlg::changeEvent(QEvent* e)
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
void ContestCategoryManagerDlg::on_pushButton_add_pressed()
//---------------------------------------------------------
{
    bool ok(false);
    QString name = QInputDialog::getText(this,
                                         tr("Add new category"),
                                         tr("Enter the name of the new category"),
                                         QLineEdit::Normal,
                                         QString(),
                                         &ok);

    while (ok && m_pClassMgr->HasCategory(name))
    {
        QMessageBox::critical(
            this, "", tr("This category already exists. Please choose an other name."));

        name = QInputDialog::getText(this,
                                     tr("Add new category"),
                                     tr("Enter the name of the new category"),
                                     QLineEdit::Normal,
                                     name,
                                     &ok);
    }

    if (ok)
    {
        m_pClassMgr->AddCategory(name);
        Ipponboard::ContestCategory cat;
        m_pClassMgr->GetCategory(name, cat);
        cat.SetGoldenScoreTime(0);
        cat.SetRoundTime(240);

        // update combobox
        QStringList contents;
        contents.append(name);
        contents.append("");
        contents.append("");
        contents.append("");
        QTreeWidgetItem* pItem = new QTreeWidgetItem(contents, QTreeWidgetItem::UserType);
        pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->treeWidget_classes->addTopLevelItem(pItem);
        pItem->setText(eColumn_Time, cat.GetRoundTimeStr());
        pItem->setText(eColumn_GS, cat.GetGoldenScoreTimeStr());
        pItem->setText(eColumn_Weights, "");
    }
}

//---------------------------------------------------------
void ContestCategoryManagerDlg::load_values()
//---------------------------------------------------------
{
    for (int i(0); i < m_pClassMgr->CategoryCount(); ++i)
    {
        Ipponboard::ContestCategory classItem;
        m_pClassMgr->GetCategory(i, classItem);

        QStringList contents;
        contents.append(classItem.ToString());
        contents.append(classItem.GetRoundTimeStr());
        contents.append(classItem.GetGoldenScoreTimeStr());
        contents.append(classItem.GetWeights());

        QTreeWidgetItem* pItem = new QTreeWidgetItem(contents, QTreeWidgetItem::UserType);
        pItem->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled | Qt::ItemIsEditable);
        ui->treeWidget_classes->addTopLevelItem(pItem);
    }
}

//---------------------------------------------------------
void ContestCategoryManagerDlg::on_pushButton_remove_pressed()
//---------------------------------------------------------
{
    QTreeWidgetItem* pItem = ui->treeWidget_classes->currentItem();

    if (pItem)
    {
        ui->treeWidget_classes->takeTopLevelItem(
            ui->treeWidget_classes->indexOfTopLevelItem(pItem));

        m_pClassMgr->RemoveCategory(pItem->text(eColumn_Name));

        delete pItem;
    }
}

//---------------------------------------------------------
void ContestCategoryManagerDlg::on_treeWidget_classes_itemChanged(QTreeWidgetItem* pItem,
                                                                  int column)
//---------------------------------------------------------
{
    bool matches(false);

    ContestCategory cat(pItem->text(eColumn_Name));
    cat.SetRoundTime(pItem->text(eColumn_Time));
    cat.SetGoldenScoreTime(pItem->text(eColumn_GS));
    cat.SetWeights(pItem->text(eColumn_Weights));

    if (eColumn_Name == column)
    {
        // get original data
        ContestCategory old;

        for (int i(0); i < ui->treeWidget_classes->topLevelItemCount(); ++i)
        {
            const QTreeWidgetItem* pCheckItem = ui->treeWidget_classes->topLevelItem(i);

            if (pCheckItem == pItem)
            {
                m_pClassMgr->GetCategory(i, old);

                // Resetting the name to the old one (below) will trigger
                // itemChanged!
                if (old.ToString() == cat.ToString())
                    return;

                break;
            }
        }

        // check if we do have more than one class with that name
        for (int i(0); i < ui->treeWidget_classes->topLevelItemCount(); ++i)
        {
            const QTreeWidgetItem* pCheckItem = ui->treeWidget_classes->topLevelItem(i);

            if (pCheckItem != pItem && pCheckItem->text(eColumn_Name) == pItem->text(eColumn_Name))
            {
                QMessageBox::critical(
                    this, QCoreApplication::applicationName(), tr("This name is already taken!"));

                // set previous text
                pItem->setText(eColumn_Name, old.ToString());

                return;
            }
        }

        m_pClassMgr->RenameCategory(old.ToString(), cat.ToString());
        return;
    }
    else if (eColumn_Time == column || eColumn_GS == column)
    {
        static const QRegularExpression timePattern(QStringLiteral("^[1-6]{0,1}[0-9]:[0-5][0-9]$"));
        matches = timePattern.match(pItem->text(column)).hasMatch();
    }
    else if (column == eColumn_Weights)
    {
        //no regex check to enable creating custom groups instead of weight classes
        matches = true;
    }

    QBrush brush(pItem->foreground(column));

    if (matches)
    {
        brush.setColor(fm::GetThemeTextColor(ui->treeWidget_classes));
        m_pClassMgr->UpdateCategory(cat);
    }
    else
    {
        brush.setColor(fm::GetValidationErrorColor(ui->treeWidget_classes));
    }

    pItem->setForeground(column, brush);
}
