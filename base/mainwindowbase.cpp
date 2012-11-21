#include "mainwindow.h"

using namespace FMlib;
using namespace Ipponboard;

#include "../core/fighter.h"
#include "../core/DataSerializer.h"

//=========================================================
MainWindowBase::MainWindowBase(QWidget* parent)
    : QMainWindow(parent)
//=========================================================
{
}

//=========================================================
MainWindowBase::~MainWindowBase()
//=========================================================
{
}

//=========================================================
void MainWindowBase::changeEvent(QEvent* e)
//=========================================================
{
    QMainWindow::changeEvent(e);
}

//=========================================================
void MainWindowBase::closeEvent(QCloseEvent* event)
//=========================================================
{
    event->accept();
}

//=========================================================
void MainWindowBase::keyPressEvent(QKeyEvent* event)
//=========================================================
{
    QMainWindow::keyPressEvent(event);
}

//=========================================================
void MainWindowBase::on_actionImport_Fighters_triggered()
//=========================================================
{
    QFileDialog fileDlg(
                    this,
                    tr("Select CSV file with fighters"),
                    QCoreApplication::applicationFilePath(),
                    tr("CSV files (*.csv);;Text files (*.txt)"));

    if (QDialog::Accepted == fileDlg.exec()
                && !fileDlg.selectedFiles().empty())
    {
        const QString fileName = fileDlg.selectedFiles()[0];
        std::vector<Ipponboard::Fighter> fighters;
        auto retVal = DataSerializer::ReadFighters(fileName, fighters);

        if (DataSerializer::eOk == retVal)
        {
            QMessageBox::information(this,
                        QCoreApplication::applicationName(),
                        tr("Successfully imported %1 fighters.").arg(QString::number(fighters.size())));

            m_fighters.insert(end(m_fighters), begin(fighters), end(fighters));

            //TODO?: update_fighter_name_completer(m_pUi->comboBox_weight->currentText());
        }
        else if (DataSerializer::eInvalid_file_format == retVal)
        {
            QMessageBox::critical(this,
                         QCoreApplication::applicationName(),
                        tr("Invalid file format, must be: firstname;lastname;weight;club (club may be omitted)."));
        }
        else
        {
            QMessageBox::critical(this, QCoreApplication::applicationName(), tr("Unable to read file %1").arg(fileName));
        }
    }
}

