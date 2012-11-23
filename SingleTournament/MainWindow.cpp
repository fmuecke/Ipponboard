#include "mainwindow.h"
#include "ui_mainwindow.h" //TODO: may be obsolete

#include "../base/fightcategorymanager.h"
#include "../base/fightcategorymanagerdlg.h"
#include "../base/view.h"
#include "../base/versioninfo.h"
#include "../core/controller.h"
#include "../core/ControllerConfig.h"
#include "../core/fighter.h"
#include "../core/tournamentmodel.h"
#include "../gamepad/gamepad.h"
#include "../util/path_helpers.h"
#include "../widgets/scaledimage.h"
#include "../widgets/scaledtext.h"

#include <QColorDialog>
#include <QComboBox>
#include <QCompleter>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QSettings>
#include <QSplashScreen>
#include <QTimer>
#include <QUrl>
#include <functional>

namespace StrTags
{
static const char* const edition = "Basic Edition";
}

using namespace FMlib;
using namespace Ipponboard;

MainWindow::MainWindow(QWidget* parent)
    : MainWindowBase(parent)
    , m_pCategoryManager()
{
	m_pUi->setupUi(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::Init()
{
    m_pCategoryManager.reset(new FightCategoryMgr());

	MainWindowBase::Init();

    // init tournament classes (if there are none present)
    for (int i(0); i < m_pCategoryManager->CategoryCount(); ++i)
    {
        FightCategory t("");
        m_pCategoryManager->GetCategory(i, t);
        m_pUi->comboBox_weight_class->addItem(t.ToString());
    }

    // trigger tournament class combobox update
    on_comboBox_weight_class_currentIndexChanged(m_pUi->comboBox_weight_class->currentText());
}

void MainWindow::on_actionManage_Classes_triggered()
{
    FightCategoryManagerDlg dlg(m_pCategoryManager, this);

    if (QDialog::Accepted == dlg.exec())
    {
        QString currentClass =
            m_pUi->comboBox_weight_class->currentText();

        m_pUi->comboBox_weight_class->clear();

        for (int i(0); i < m_pCategoryManager->CategoryCount(); ++i)
        {
            FightCategory t("");
            m_pCategoryManager->GetCategory(i, t);
            m_pUi->comboBox_weight_class->addItem(t.ToString());
        }

        int index = m_pUi->comboBox_weight_class->findText(currentClass);

        if (-1 == index)
        {
            index = 0;
            currentClass = m_pUi->comboBox_weight_class->itemText(index);
        }

        m_pUi->comboBox_weight_class->setCurrentIndex(index);
        on_comboBox_weight_class_currentIndexChanged(currentClass);
    }
}

void MainWindow::on_comboBox_weight_currentIndexChanged(const QString& s)
{
    update_fighter_name_completer(s);

    m_pPrimaryView->SetWeight(s);
    m_pSecondaryView->SetWeight(s);
    m_pPrimaryView->UpdateView();
    m_pSecondaryView->UpdateView();
}

void MainWindow::on_comboBox_name_blue_currentIndexChanged(const QString& s)
{
    update_fighters(s);

    m_pController->SetFighterName(eFighter_Blue, s);
}

void MainWindow::on_comboBox_name_white_currentIndexChanged(const QString& s)
{
    update_fighters(s);

    m_pController->SetFighterName(eFighter_White, s);
}

void MainWindow::on_checkBox_golden_score_clicked(bool checked)
{
    const QString name = m_pUi->comboBox_weight_class->currentText();
    FightCategory t(name);
    m_pCategoryManager->GetCategory(name, t);

    m_pController->SetGoldenScore(checked);
    //> Set this before setting the time.
    //> Setting time will then update the views.

    if (checked)
    {
        m_pController->SetRoundTime(
            QTime().addSecs(t.GetGoldenScoreTime()));
    }
    else
    {
        m_pController->SetRoundTime(
            QTime().addSecs(t.GetRoundTime()));
    }
}

void MainWindow::on_comboBox_weight_class_currentIndexChanged(const QString& s)
{
    FightCategory t(s);
    m_pCategoryManager->GetCategory(s, t);

    // add weights
    m_pUi->comboBox_weight->clear();
    m_pUi->comboBox_weight->addItems(t.GetWeightsList());

    // trigger round time update
    on_checkBox_golden_score_clicked(m_pUi->checkBox_golden_score->checkState());

    m_pPrimaryView->SetCategory(s);
    m_pSecondaryView->SetCategory(s);
    m_pPrimaryView->UpdateView();
    m_pSecondaryView->UpdateView();
}

void MainWindow::update_fighter_name_completer(const QString& weight)
{
    // filter fighters for suitable
    m_CurrentFighterNames.clear();

    Q_FOREACH(const Ipponboard::Fighter& f, m_fighters)
    {
        if(f.weight_class == weight || f.weight_class.isEmpty())
        {
            const QString fullName =
                    QString("%1 %2").arg(f.first_name, f.last_name);

            m_CurrentFighterNames.push_back(fullName);
        }
    }

    m_CurrentFighterNames.sort();

    m_pUi->comboBox_name_blue->clear();
    m_pUi->comboBox_name_blue->addItems(m_CurrentFighterNames);
    m_pUi->comboBox_name_white->clear();
    m_pUi->comboBox_name_white->addItems(m_CurrentFighterNames);
}

void MainWindow::on_actionImportList_triggered()
{
    m_fighters.clear();
    MainWindowBase::on_actionImport_Fighters_triggered();

    if (!m_fighters.empty())
        update_fighter_name_completer(m_pUi->comboBox_weight->currentText());
}

void MainWindow::on_actionExportList_triggered()
{
    const QString currentDate = QDate::currentDate().toString("yyyy-MM-dd");

	QString fileName = QString("%1\\IpponboardFighters_%2.csv")
		.arg(QCoreApplication::applicationFilePath(), currentDate);
		
	fileName = QFileDialog::getSaveFileName(this, 
                    tr("Select CSV file to store fighter information"),
                    fileName,
                    tr("CSV files (*.csv);;Text files (*.txt)"));

	if (!fileName.isEmpty())
    {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
        {
            QTextStream out(&file);

            // write header
            out << "@FIRSTNAME;@LASTNAME;@WEIGHT;@CLUB\n";
            Q_FOREACH(const Ipponboard::Fighter& f, m_fighters)
            {
                out << f.first_name << ";"
                    << f.last_name << ";"
                    << f.weight_class << ";"
                    << f.club << "\n";

                out.flush();
            }
            file.close();

            QMessageBox::information(
                        this,
                        QCoreApplication::applicationName(),
                        tr("Successfully exported %1 fighters.").arg(QString::number(m_fighters.size())));
        }
    }
}

void MainWindow::update_fighters(const QString& s)
{
    if (s.isEmpty())
        return;

    QString firstName = s;
    QString lastName;

    int pos = s.indexOf(' ');
    if (pos < s.size())
    {
        firstName = s.left(pos);
        lastName = s.mid(pos+1);
    }
    const QString weight = m_pUi->comboBox_weight->currentText();
    const QString club; // TODO: later

    Ipponboard::Fighter fNew(firstName, lastName, weight, club);

    // Does fighter already exist in list?
    bool found(false);
    Q_FOREACH(const Ipponboard::Fighter& f, m_fighters)
    {
        if (f.first_name == fNew.first_name &&
                f.last_name == fNew.last_name)
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        m_fighters.push_back(fNew);
    }
}



