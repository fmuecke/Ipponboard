#include "mainwindowbase.h"
#include "ui_mainwindow.h"

#include "view.h"
#include "../core/controller.h"
#include "../core/fighter.h"
#include "../core/DataSerializer.h"
#include "../base/versioninfo.h"
#include "../base/settingsdlg.h"
#include "../gamepad/gamepad.h"
#include "../util/path_helpers.h"

using namespace FMlib;
using namespace Ipponboard;

MainWindowBase::MainWindowBase(QWidget* parent)
    : QMainWindow(parent)
    , m_pUi(new Ui::MainWindow)
    , m_pPrimaryView()
    , m_pSecondaryView()
    , m_pController(new Ipponboard::Controller())
    , m_Language("en")
    , m_MatLabel()
    , m_pGamePad(new Gamepad)
    , m_FighterNameFont("Calibri", 12, QFont::Bold, false)
    , m_secondScreenNo(0)
    , m_bAlwaysShow(true)
    , m_bAutoSize(true)
    , m_secondScreenSize()
    , m_weights()
    , m_controllerCfg()
{
}

MainWindowBase::~MainWindowBase()
{
}

void MainWindowBase::Init()
{
	// Setup views
    m_pPrimaryView.reset(
        new Ipponboard::View(m_pController->GetIController(), Ipponboard::View::eTypePrimary));

    QWidget* widget = dynamic_cast<QWidget*>(m_pPrimaryView.get());
    if (widget)
    {
        m_pUi->verticalLayout_3->insertWidget(0, widget, 0);
    }

    m_pSecondaryView.reset(
		new Ipponboard::View(m_pController->GetIController(), Ipponboard::View::eTypeSecondary));

    // clear data
    m_pController->ClearFights();

    // Load settings
    read_settings();

    if (m_bAlwaysShow)
    {
        m_pUi->actionShow_SecondaryView->setChecked(true);
        on_actionShow_SecondaryView_triggered();
    }

    change_lang(true);

    // Init gamepad
    QTimer* m_pTimer = new QTimer;
    connect(m_pTimer, SIGNAL(timeout()), this, SLOT(EvaluateInput()));
    m_pTimer->start(75);
}

QString MainWindowBase::GetConfigFileName() const
{
	return QString("Ipponboard%1.ini").arg(EditionNameShort()); 
}

void MainWindowBase::changeEvent(QEvent* e)
{
    QMainWindow::changeEvent(e);

    switch (e->type())
    {
    case QEvent::LanguageChange:
        m_pUi->retranslateUi(this);
        break;

    default:
        break;
    }
}

void MainWindowBase::closeEvent(QCloseEvent* event)
{
    write_settings();

    if (m_pSecondaryView)
    {
        m_pSecondaryView->close();
    }

    event->accept();
}

void MainWindowBase::keyPressEvent(QKeyEvent* event)
{
    const bool isCtrlPressed = event->modifiers().testFlag(Qt::ControlModifier);

    switch (event->key())
    {
    case Qt::Key_Space:
        m_pController->DoAction(Ipponboard::eAction_Hajime_Mate, Ipponboard::eFighter_Nobody);
        qDebug() << "Action [ Hajime/Mate ] was triggered by keyboard";
        break;

    case Qt::Key_Backspace:
        if (isCtrlPressed)
        {
            m_pController->DoAction(Ipponboard::eAction_ResetAll, Ipponboard::eFighter_Nobody);
            qDebug() << "Action [ Reset ] was triggered by keyboard";
        }

        break;

    case Qt::Key_Left:
        {
            if (eState_Holding == m_pController->GetCurrentState() &&
                    Ipponboard::eFighter_Blue != m_pController->GetLead())
            {
                m_pController->DoAction(Ipponboard::eAction_SetOsaekomi,
                                        Ipponboard::eFighter_Blue);
            }
            else
            {
                m_pController->DoAction(Ipponboard::eAction_OsaeKomi_Toketa,
                                        Ipponboard::eFighter_Blue);
            }

            qDebug() << "Action [ Osaekomi/Toketa for blue ] was triggered by keyboard";
        }

        break;

    case Qt::Key_Right:
        {
            if (eState_Holding == m_pController->GetCurrentState() &&
                    Ipponboard::eFighter_White != m_pController->GetLead())
            {
                m_pController->DoAction(Ipponboard::eAction_SetOsaekomi,
                                        Ipponboard::eFighter_White);
            }
            else
            {
                m_pController->DoAction(Ipponboard::eAction_OsaeKomi_Toketa,
                                        Ipponboard::eFighter_White);
            }

            qDebug() << "Action [ Osaekomi/Toketa for white ] was triggered by keyboard";
        }

        break;

    case Qt::Key_Down:
        //if (isCtrlPressed)
        {
            m_pController->DoAction(Ipponboard::eAction_ResetOsaeKomi,
                                    Ipponboard::eFighter_Nobody,
                                    true);
            qDebug() << "Action [ Reset Osaekomi ] was triggered by keyboard";
        }
        break;

    case Qt::Key_F5:
        m_pController->DoAction(Ipponboard::eAction_Ippon,
                                Ipponboard::eFighter_Blue,
                                isCtrlPressed);
        qDebug() << "Action [ Ippon for blue, revoke="
                 << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F6:
        m_pController->DoAction(Ipponboard::eAction_Wazaari,
                                Ipponboard::eFighter_Blue,
                                isCtrlPressed);
        qDebug() << "Action [ Wazaari for blue, revoke="
                 << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F7:
        m_pController->DoAction(Ipponboard::eAction_Yuko,
                                Ipponboard::eFighter_Blue,
                                isCtrlPressed);
        qDebug() << "Action [ Yuko for blue, revoke="
                 << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F8:
        m_pController->DoAction(Ipponboard::eAction_Shido,
                                Ipponboard::eFighter_Blue,
                                isCtrlPressed);
        qDebug() << "Action [ Shido for blue, revoke="
                 << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F9:
        m_pController->DoAction(Ipponboard::eAction_Ippon,
                                Ipponboard::eFighter_White,
                                isCtrlPressed);
        qDebug() << "Action [ Ippon for white, revoke="
                 << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F10:
        m_pController->DoAction(Ipponboard::eAction_Wazaari,
                                Ipponboard::eFighter_White,
                                isCtrlPressed);
        qDebug() << "Action [ Wazaari for white, revoke="
                 << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F11:
        m_pController->DoAction(Ipponboard::eAction_Yuko,
                                Ipponboard::eFighter_White,
                                isCtrlPressed);
        qDebug() << "Action [ Yuko for white, revoke="
                 << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F12:
        m_pController->DoAction(Ipponboard::eAction_Shido,
                                Ipponboard::eFighter_White,
                                isCtrlPressed);
        qDebug() << "Action [ Shido for white, revoke="
                 << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

void MainWindowBase::on_actionImport_Fighters_triggered()
{
    const QString fileName = QFileDialog::getOpenFileName(this,
                    tr("Select CSV file with fighters"),
                    QCoreApplication::applicationDirPath(),
                    tr("CSV files (*.csv);;Text files (*.txt)"), nullptr, QFileDialog::ReadOnly);

	if (!fileName.isEmpty())
	{
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
                        tr("The file format is invalid!\n\nIt must be: firstname;lastname;weight;club\n\nSpecify one fighter per line. The club may be omitted."));
        }
        else
        {
            QMessageBox::critical(this, QCoreApplication::applicationName(), tr("Unable to read file %1").arg(fileName));
        }
    }
}

void MainWindowBase::on_actionAbout_Ipponboard_triggered()
{
    QMessageBox::about(
        this,
        tr("About %1").arg(QCoreApplication::applicationName()),
        tr("<h3>%1 v%2</h3>"
           "<p>%1 is written in C++ <br/>using <a href=\"http://boost.org\">boost</a> and the Qt toolkit %3.</p>"
           "<p>Build: %4, Revision: %5</p>"
           "<p><a href=\"http://www.ipponboard.info\">www.ipponboard.info</a></p>"
           "<p>&copy; 2010-2012 Florian M&uuml;cke. All rights reserved.</p>"
           "<p>Some icons by <a href=\"http://p.yusukekamiyamane.com/\">Yusuke Kamiyamane</a>. All rights reserved.</p>"
           "<p>This program is provided AS IS with NO WARRANTY OF ANY KIND, "
           "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A "
           "PARTICULAR PURPOSE.</p>"
          ).arg(QCoreApplication::applicationName(),
                QCoreApplication::applicationVersion(),
                QLatin1String(QT_VERSION_STR),
                VersionInfo::Date,
                VersionInfo::Revision));
}

void MainWindowBase::on_actionVisit_Project_Homepage_triggered()
{
    QUrl url("http://www.ipponboard.info");
    QDesktopServices::openUrl(url);
}

void MainWindowBase::on_actionOnline_Feedback_triggered()
{
    QUrl url("http://flo.mueckeimnetz.de/ipponboard/survey_de");
    QDesktopServices::openUrl(url);
}

void MainWindowBase::on_actionContact_Author_triggered()
{
    QUrl url("mailto:ipponboardinfo@googlemail.com?"
             "subject=Ipponboard_v" + QCoreApplication::applicationVersion() +
             "&body=Please tell us what you want to know/suggest...");
    QDesktopServices::openUrl(url);
}

void MainWindowBase::change_lang(bool beQuiet)
{
    // set checks
    m_pUi->actionLang_Deutsch->setChecked("de" == m_Language);
    m_pUi->actionLang_English->setChecked("en" == m_Language);

    if (!beQuiet)
    {
        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 tr("Please restart the application so that the changes can take effect."));
    }
}

void MainWindowBase::on_actionLang_Deutsch_triggered(bool val)
{
    if (val)
    {
        m_Language = "de";
        change_lang();
    }
}

void MainWindowBase::on_actionLang_English_triggered(bool val)
{
    if (val)
    {
        m_Language = "en";
        change_lang();
    }
}


void MainWindowBase::write_settings()
{
    QString iniFile(
        QString::fromStdString(
		fmu::GetSettingsFilePath(GetConfigFileName().toAscii())));

    QSettings settings(iniFile, QSettings::IniFormat, this);

    settings.beginGroup(str_tag_Main);
    settings.setValue(str_tag_Version, VersionInfo::VersionStr);
    settings.setValue(str_tag_Language, m_Language);
    settings.setValue(str_tag_size, size());
    settings.setValue(str_tag_pos, pos());
    settings.setValue(str_tag_SecondScreen, m_secondScreenNo);
    settings.setValue(str_tag_SecondScreenSize, m_secondScreenSize);
    settings.setValue(str_tag_AutoSize, m_bAutoSize);
    settings.setValue(str_tag_AlwaysShow, m_bAlwaysShow);
    settings.setValue(str_tag_MatLabel, m_MatLabel);
    settings.endGroup();

    // write the edition specific settings
    write_specific_settings(settings);

    settings.beginGroup(str_tag_Fonts);
    settings.setValue(str_tag_TextFont1, m_pPrimaryView->GetInfoHeaderFont().toString());
    settings.setValue(str_tag_FighterNameFont, m_FighterNameFont.toString());
    settings.setValue(str_tag_DigitFont, m_pPrimaryView->GetDigitFont().toString());
    settings.endGroup();

    settings.beginGroup(str_tag_Colors);
    settings.setValue(str_tag_InfoTextColor, m_pPrimaryView->GetInfoTextColor());
    settings.setValue(str_tag_InfoTextBgColor, m_pPrimaryView->GetInfoTextBgColor());
    settings.setValue(str_tag_TextColorBlue, m_pPrimaryView->GetTextColorBlue());
    settings.setValue(str_tag_TextBgColorBlue, m_pPrimaryView->GetTextBgColorBlue());
    settings.setValue(str_tag_TextColorWhite, m_pPrimaryView->GetTextColorWhite());
    settings.setValue(str_tag_TextBgColorWhite, m_pPrimaryView->GetTextBgColorWhite());
    //settings.setValue(str_tag_MainClockColorRunning, m_pPrimaryView->GetMainClockColor1());
    //settings.setValue(str_tag_MainClockColorStopped, m_pPrimaryView->GetMainClockColor2());
    settings.endGroup();

    // Styles
    settings.beginGroup(str_tag_Styles);
    settings.setValue(str_tag_BgStyle, m_pUi->frame_primary_view->styleSheet());
    settings.endGroup();

    settings.beginGroup(str_tag_Input);
    settings.setValue(str_tag_buttonHajimeMate, m_controllerCfg.button_hajime_mate);
    settings.setValue(str_tag_buttonNext, m_controllerCfg.button_next);
    settings.setValue(str_tag_buttonPrev, m_controllerCfg.button_prev);
    settings.setValue(str_tag_buttonPause, m_controllerCfg.button_pause);
    settings.setValue(str_tag_buttonReset, m_controllerCfg.button_reset);
    settings.setValue(str_tag_buttonReset2, m_controllerCfg.button_reset_2);
    settings.setValue(str_tag_buttonResetHoldBlue, m_controllerCfg.button_reset_hold_blue);
    settings.setValue(str_tag_buttonResetHoldWhite, m_controllerCfg.button_reset_hold_white);
    settings.setValue(str_tag_buttonBlueHolding, m_controllerCfg.button_osaekomi_toketa_blue);
    settings.setValue(str_tag_buttonWhiteHolding, m_controllerCfg.button_osaekomi_toketa_white);
    settings.setValue(str_tag_buttonHansokumakeBlue, m_controllerCfg.button_hansokumake_blue);
    settings.setValue(str_tag_buttonHansokumakeWhite, m_controllerCfg.button_hansokumake_white);
    settings.setValue(str_tag_invertX, m_controllerCfg.axis_inverted_X);
    settings.setValue(str_tag_invertY, m_controllerCfg.axis_inverted_Y);
    settings.setValue(str_tag_invertR, m_controllerCfg.axis_inverted_R);
    settings.setValue(str_tag_invertZ, m_controllerCfg.axis_inverted_Z);
    settings.endGroup();

    settings.beginGroup(str_tag_Sounds);
    settings.setValue(str_tag_sound_time_ends, m_pController->GetGongFile());
    settings.endGroup();
}

void MainWindowBase::read_settings()
{
    QString iniFile(
        QString::fromStdString(
            fmu::GetSettingsFilePath(GetConfigFileName().toAscii())));

    QSettings settings(iniFile, QSettings::IniFormat, this);

    
    // MainWindow
    
    settings.beginGroup(str_tag_Main);

    QString langStr = QLocale::system().name();
    langStr.truncate(langStr.lastIndexOf('_'));
    m_Language = settings.value(str_tag_Language, langStr).toString();

    //resize(settings.value(str_tag_size, size()).toSize());
    move(settings.value(str_tag_pos, QPoint(200, 200)).toPoint());
    m_secondScreenNo = settings.value(str_tag_SecondScreen, 0).toInt();
    m_secondScreenSize = settings.value(str_tag_SecondScreenSize,
                                        QSize(1024, 768)).toSize();
    m_bAutoSize = settings.value(str_tag_AutoSize, true).toBool();
    m_bAlwaysShow = settings.value(str_tag_AlwaysShow, true).toBool();
    m_MatLabel = settings.value(str_tag_MatLabel, "  Ipponboard  ").toString(); // value is also in settings dialog!
    m_pPrimaryView->SetMat(m_MatLabel);
    m_pSecondaryView->SetMat(m_MatLabel);
    settings.endGroup();

    // read edition specific settings
    read_specific_settings(settings);

    
    // Fonts
    
    settings.beginGroup(str_tag_Fonts);
    QFont font = m_pPrimaryView->GetInfoHeaderFont();
    font.fromString(settings.value(str_tag_TextFont1, font.toString()).toString());
    m_pPrimaryView->SetInfoHeaderFont(m_pPrimaryView->GetInfoHeaderFont());
    m_pSecondaryView->SetInfoHeaderFont(m_pPrimaryView->GetInfoHeaderFont());

    font = m_pPrimaryView->GetFighterNameFont();
    font.fromString(settings.value(str_tag_FighterNameFont, font.toString()).toString());
    update_fighter_name_font(font);

    font = m_pPrimaryView->GetDigitFont();
    font.fromString(settings.value(str_tag_DigitFont, font.toString()).toString());
    m_pPrimaryView->SetDigitFont(font);
    m_pSecondaryView->SetDigitFont(font);
    //m_pScoreScreen->SetDigitFont(font);
    settings.endGroup();

    
    // Colors
    
    settings.beginGroup(str_tag_Colors);
    QColor fgColor = m_pSecondaryView->GetInfoTextColor();
    QColor bgColor = m_pSecondaryView->GetInfoTextBgColor();

    if (settings.contains(str_tag_InfoTextColor))
        fgColor = settings.value(str_tag_InfoTextColor).value<QColor>();

    if (settings.contains(str_tag_InfoTextBgColor))
        bgColor = settings.value(str_tag_InfoTextBgColor).value<QColor>();

    update_info_text_color(fgColor, bgColor);

    fgColor = m_pSecondaryView->GetTextColorBlue();
    bgColor = m_pSecondaryView->GetTextBgColorBlue();

    if (settings.contains(str_tag_TextColorBlue))
        fgColor = settings.value(str_tag_TextColorBlue).value<QColor>();

    if (settings.contains(str_tag_TextBgColorBlue))
        bgColor = settings.value(str_tag_TextBgColorBlue).value<QColor>();

    update_text_color_blue(fgColor, bgColor);

    fgColor = m_pPrimaryView->GetTextColorWhite();
    bgColor = m_pSecondaryView->GetTextBgColorWhite();

    if (settings.contains(str_tag_TextColorWhite))
        fgColor = settings.value(str_tag_TextColorWhite).value<QColor>();

    if (settings.contains(str_tag_TextBgColorWhite))
        bgColor = settings.value(str_tag_TextBgColorWhite).value<QColor>();

    update_text_color_white(fgColor, bgColor);

    //fgColor = m_pPrimaryView->GetMainClockColor1();
    //bgColor = m_pPrimaryView->GetMainClockColor2();
    //if( settings.contains(str_tag_MainClockColorRunning) )
    //	fgColor = settings.value(str_tag_MainClockColorRunning).value<QColor>();
    //if( settings.contains(str_tag_MainClockColorStopped) )
    //	bgColor = settings.value(str_tag_MainClockColorStopped).value<QColor>();
    //m_pPrimaryView->SetMainClockColor(fgColor, bgColor);
    //m_pSecondaryView->SetMainClockColor(fgColor, bgColor);
    settings.endGroup();

    // Styles
    settings.beginGroup(str_tag_Styles);
    if (settings.contains(str_tag_BgStyle))
    {
        const QString styleSheet = settings.value(str_tag_BgStyle).toString();
        //m_pUi->frame_primary_view->setStyleSheet(styleSheet);
        //m_pSecondaryView->setStyleSheet(styleSheet);
    }
    settings.endGroup();

    settings.beginGroup(str_tag_Input);

    m_controllerCfg.button_hajime_mate =
        settings.value(str_tag_buttonHajimeMate, Gamepad::eButton_pov_back).toInt();

    m_controllerCfg.button_next =
        settings.value(str_tag_buttonNext, Gamepad::eButton10).toInt();

    m_controllerCfg.button_prev =
        settings.value(str_tag_buttonPrev, Gamepad::eButton9).toInt();

    m_controllerCfg.button_pause =
        settings.value(str_tag_buttonPause, Gamepad::eButton2).toInt();

    m_controllerCfg.button_reset =
        settings.value(str_tag_buttonReset, Gamepad::eButton1).toInt();

    m_controllerCfg.button_reset_2 =
        settings.value(str_tag_buttonReset2, Gamepad::eButton4).toInt();

    m_controllerCfg.button_reset_hold_blue =
        settings.value(str_tag_buttonResetHoldBlue, Gamepad::eButton6).toInt();

    m_controllerCfg.button_reset_hold_white =
        settings.value(str_tag_buttonResetHoldWhite, Gamepad::eButton8).toInt();

    m_controllerCfg.button_osaekomi_toketa_blue =
        settings.value(str_tag_buttonBlueHolding, Gamepad::eButton5).toInt();

    m_controllerCfg.button_osaekomi_toketa_white =
        settings.value(str_tag_buttonWhiteHolding, Gamepad::eButton7).toInt();

    m_controllerCfg.button_hansokumake_blue =
        settings.value(str_tag_buttonHansokumakeBlue, Gamepad::eButton11).toInt();

    m_controllerCfg.button_hansokumake_white =
        settings.value(str_tag_buttonHansokumakeWhite, Gamepad::eButton12).toInt();

    m_controllerCfg.axis_inverted_X = settings.value(str_tag_invertX, false).toBool();
    m_controllerCfg.axis_inverted_Y = settings.value(str_tag_invertY, true).toBool();
    m_controllerCfg.axis_inverted_R = settings.value(str_tag_invertR, true).toBool();
    m_controllerCfg.axis_inverted_Z = settings.value(str_tag_invertZ, true).toBool();
    // apply settings to gamepad controller
    m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_X, m_controllerCfg.axis_inverted_X);
    m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_Y, m_controllerCfg.axis_inverted_Y);
    m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_R, m_controllerCfg.axis_inverted_R);
    m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_Z, m_controllerCfg.axis_inverted_Z);
    settings.endGroup();

    settings.beginGroup(str_tag_Sounds);
    m_pController->SetGongFile(settings.value(str_tag_sound_time_ends,
                               "sounds/buzzer1.wav").toString());
    settings.endGroup();

    // update views
    update_views();
}

void MainWindowBase::update_views()
{
    m_pPrimaryView->UpdateView();
    m_pSecondaryView->UpdateView();
}

void MainWindowBase::on_actionTest_Gong_triggered()
{
    m_pController->Gong();
}

void MainWindowBase::on_actionShow_SecondaryView_triggered()
{
    show_hide_view();
}

void MainWindowBase::on_actionPreferences_triggered()
{
    SettingsDlg dlg(this);
    dlg.SetInfoHeaderSettings(m_pPrimaryView->GetInfoHeaderFont(),
                              m_pPrimaryView->GetInfoTextColor(),
                              m_pPrimaryView->GetInfoTextBgColor());

    dlg.SetFighterNameFont(m_FighterNameFont);

    dlg.SetTextColorsBlue(m_pPrimaryView->GetTextColorBlue(),
                          m_pPrimaryView->GetTextBgColorBlue());

    dlg.SetTextColorsWhite(m_pPrimaryView->GetTextColorWhite(),
                           m_pPrimaryView->GetTextBgColorWhite());

    dlg.SetScreensSettings(m_bAlwaysShow, m_secondScreenNo, m_bAutoSize,
                           m_secondScreenSize);

    dlg.SetControllerConfig(&m_controllerCfg);
    dlg.SetMatLabel(m_MatLabel);
    dlg.SetGongFile(m_pController->GetGongFile());

    if (QDialog::Accepted == dlg.exec())
    {
        m_pPrimaryView->SetInfoHeaderFont(dlg.GetInfoHeaderFont());
        m_pSecondaryView->SetInfoHeaderFont(dlg.GetInfoHeaderFont());
        update_fighter_name_font(dlg.GetFighterNameFont());
        update_info_text_color(dlg.GetInfoTextColor(), dlg.GetInfoTextBgColor());
        update_text_color_blue(dlg.GetTextColorBlue(), dlg.GetTextBgColorBlue());
        update_text_color_white(dlg.GetTextColorWhite(), dlg.GetTextBgColorWhite());

        m_bAlwaysShow = dlg.IsShowAlways();
        m_secondScreenNo = dlg.GetSelectedScreen();
        m_bAutoSize = dlg.IsAutoSize();
        m_secondScreenSize = dlg.GetSize();

        dlg.GetControllerConfig(&m_controllerCfg);
        // apply settings to gamepad
        m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_X, m_controllerCfg.axis_inverted_X);
        m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_Y, m_controllerCfg.axis_inverted_Y);
        m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_R, m_controllerCfg.axis_inverted_R);
        m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_Z, m_controllerCfg.axis_inverted_Z);


        m_MatLabel = dlg.GetMatLabel();
        m_pPrimaryView->SetMat(m_MatLabel);
        m_pSecondaryView->SetMat(m_MatLabel);
        //m_pPrimaryView->UpdateView();
        //m_pSecondaryView->UpdateView();
        m_pController->SetGongFile(dlg.GetGongFile());

        // save changes to file
        write_settings();

        update_views();
    }
}


void MainWindowBase::show_hide_view() const
{
    if (m_pSecondaryView->isHidden())
    {
        const int nScreens(QApplication::desktop()->numScreens());

        if (nScreens > 0 && nScreens > m_secondScreenNo)
        {
            QRect screenres =
                    QApplication::desktop()->screenGeometry(m_secondScreenNo);
            m_pSecondaryView->move(QPoint(screenres.x(), screenres.y()));
        }

        if (m_bAutoSize)
        {
            m_pSecondaryView->showFullScreen();
        }
        else
        {
            m_pSecondaryView->resize(m_secondScreenSize);
            m_pSecondaryView->show();
        }
    }
    else
    {
        m_pSecondaryView->hide();
    }
}

void MainWindowBase::EvaluateInput()
{
    if (Gamepad::eState_ok != m_pGamePad->GetState())
        return;

    m_pGamePad->ReadData();

    if (EvaluateSpecificInput(m_pGamePad.get()))
        return;

    if (m_pGamePad->WasPressed(Gamepad::EButton(m_controllerCfg.button_hajime_mate)))
    {
        m_pController->DoAction(eAction_Hajime_Mate, eFighter_Nobody);
    }
    else if (m_pGamePad->WasPressed(Gamepad::EButton(m_controllerCfg.button_reset_hold_blue)))
    {
        m_pController->DoAction(eAction_ResetOsaeKomi, eFighter_Blue, true);
    }
    else if (m_pGamePad->WasPressed(Gamepad::EButton(m_controllerCfg.button_reset_hold_white)))
    {
        m_pController->DoAction(eAction_ResetOsaeKomi, eFighter_White, true);
    }
    else if (m_pGamePad->WasPressed(Gamepad::EButton(m_controllerCfg.button_osaekomi_toketa_blue)))
    {
        if (eState_Holding == m_pController->GetCurrentState() &&
                eFighter_Blue != m_pController->GetLead())
        {
            m_pController->DoAction(eAction_SetOsaekomi, eFighter_Blue);
        }
        else
        {
            m_pController->DoAction(eAction_OsaeKomi_Toketa, eFighter_Blue);
        }
    }
    else if (m_pGamePad->WasPressed(Gamepad::EButton(m_controllerCfg.button_osaekomi_toketa_white)))
    {
        if (eState_Holding == m_pController->GetCurrentState() &&
                eFighter_White != m_pController->GetLead())
        {
            m_pController->DoAction(eAction_SetOsaekomi, eFighter_White);
        }
        else
        {
            m_pController->DoAction(eAction_OsaeKomi_Toketa, eFighter_White);
        }
    }
    // reset
    else if (
        m_pGamePad->IsPressed(Gamepad::EButton(m_controllerCfg.button_reset)) &&
        m_pGamePad->IsPressed(Gamepad::EButton(m_controllerCfg.button_reset_2)))
    {
        m_pController->DoAction(eAction_ResetAll, eFighter_Nobody);
    }

    // hansokumake blue
    else if (m_pGamePad->WasPressed(Gamepad::EButton(m_controllerCfg.button_hansokumake_blue)))
    {
        const bool revoke(m_pController->GetScore(
                              eFighter_Blue, ePoint_Hansokumake) != 0);
        m_pController->DoAction(eAction_Hansokumake, eFighter_Blue, revoke);
    }
    // hansokumake white
    else if (m_pGamePad->WasPressed(Gamepad::EButton(m_controllerCfg.button_hansokumake_white)))
    {
        const bool revoke(m_pController->GetScore(
                              eFighter_White, ePoint_Hansokumake) != 0);
        m_pController->DoAction(eAction_Hansokumake, eFighter_White, revoke);

    }
    else
    {
        // TODO: don't calc this every time...
        float sections[8][2] = {{0}};
        float angle = 360.0f;
        float deadSpace = 2.0f;
        float angle_adjustment = 5.0f;
        sections[0][0] = 360 - 45 / 2 + 1 + deadSpace + angle_adjustment;
        sections[0][1] = 45 / 2 + 1 - deadSpace + angle_adjustment;
        angle = sections[0][1];

        for (int i = 1; i < 8; ++i)
        {
            sections[i][0] = angle + 2 * deadSpace;
            sections[i][1] = angle + 45;
            angle = sections[i][1];
        }

        if (m_pGamePad->WasSectionEnteredXY(sections[0][0], sections[0][1]))
        {
            m_pController->DoAction(eAction_Ippon, eFighter_Blue);
        }
        else if (m_pGamePad->WasSectionEnteredXY(sections[1][0], sections[1][1]))
        {
            m_pController->DoAction(eAction_Wazaari, eFighter_Blue);
        }
        else if (m_pGamePad->WasSectionEnteredXY(sections[2][0], sections[2][1]))
        {
            m_pController->DoAction(eAction_Yuko, eFighter_Blue);
        }
        else if (m_pGamePad->WasSectionEnteredXY(sections[3][0], sections[3][1]))
        {
            m_pController->DoAction(eAction_Shido, eFighter_Blue, true);
        }
        else if (m_pGamePad->WasSectionEnteredXY(sections[4][0], sections[4][1]))
        {
            m_pController->DoAction(eAction_Ippon, eFighter_Blue, true);
        }
        else if (m_pGamePad->WasSectionEnteredXY(sections[5][0], sections[5][1]))
        {
            m_pController->DoAction(eAction_Wazaari, eFighter_Blue, true);
        }
        else if (m_pGamePad->WasSectionEnteredXY(sections[6][0], sections[6][1]))
        {
            m_pController->DoAction(eAction_Yuko, eFighter_Blue, true);
        }
        else if (m_pGamePad->WasSectionEnteredXY(sections[7][0], sections[7][1]))
        {
            m_pController->DoAction(eAction_Shido, eFighter_Blue);
        }

        // evaluate white actions
        else if (m_pGamePad->WasSectionEnteredRZ(sections[0][0], sections[0][1]))
        {
            m_pController->DoAction(eAction_Ippon, eFighter_White);
        }
        else if (m_pGamePad->WasSectionEnteredRZ(sections[1][0], sections[1][1]))
        {
            m_pController->DoAction(eAction_Wazaari, eFighter_White);
        }
        else if (m_pGamePad->WasSectionEnteredRZ(sections[2][0], sections[2][1]))
        {
            m_pController->DoAction(eAction_Yuko, eFighter_White);
        }
        else if (m_pGamePad->WasSectionEnteredRZ(sections[3][0], sections[3][1]))
        {
            m_pController->DoAction(eAction_Shido, eFighter_White, true);
        }
        else if (m_pGamePad->WasSectionEnteredRZ(sections[4][0], sections[4][1]))
        {
            m_pController->DoAction(eAction_Ippon, eFighter_White, true);
        }
        else if (m_pGamePad->WasSectionEnteredRZ(sections[5][0], sections[5][1]))
        {
            m_pController->DoAction(eAction_Wazaari, eFighter_White, true);
        }
        else if (m_pGamePad->WasSectionEnteredRZ(sections[6][0], sections[6][1]))
        {
            m_pController->DoAction(eAction_Yuko, eFighter_White, true);
        }
        else if (m_pGamePad->WasSectionEnteredRZ(sections[7][0], sections[7][1]))
        {
            m_pController->DoAction(eAction_Shido, eFighter_White);
        }
    }
}

void MainWindowBase::update_info_text_color(const QColor& color, const QColor& bgColor)
{
    m_pPrimaryView->SetInfoTextColor(color, bgColor);
    m_pSecondaryView->SetInfoTextColor(color, bgColor);
}

void MainWindowBase::update_text_color_blue(const QColor& color, const QColor& bgColor)
{
    m_pPrimaryView->SetTextColorBlue(color, bgColor);
    m_pSecondaryView->SetTextColorBlue(color, bgColor);
}

void MainWindowBase::update_text_color_white(const QColor& color, const QColor& bgColor)
{
    m_pPrimaryView->SetTextColorWhite(color, bgColor);
    m_pSecondaryView->SetTextColorWhite(color, bgColor);
}

void MainWindowBase::update_fighter_name_font(const QFont& font)
{
    m_FighterNameFont = font;
    m_pPrimaryView->SetFighterNameFont(font);
    m_pSecondaryView->SetFighterNameFont(font);
}


void MainWindowBase::on_button_reset_clicked()
{
//	QMessageBox::StandardButton answer =
//		QMessageBox::question( this,
//							   tr("Reset"),
//							   tr("Really reset current fight?"),
//							   QMessageBox::No | QMessageBox::Yes );
//	if( QMessageBox::Yes == answer )
    m_pController->DoAction(Ipponboard::eAction_ResetAll,
                            Ipponboard::eFighter_Nobody,
                            false);
}

void MainWindowBase::on_action_Info_Header_triggered(bool val)
{
    m_pPrimaryView->SetShowInfoHeader(val);
    m_pSecondaryView->SetShowInfoHeader(val);
}

void MainWindowBase::on_actionSet_Hold_Timer_triggered()
{
    bool ok(false);
    const int seconds = QInputDialog::getInt(
                            this,
                            tr("Set Value"),
                            tr("Set value to (ss):"),
                            0,	// value
                            0,	// min
                            59,	// max
                            1,	// step
                            &ok);

    if (ok)
	{
        m_pController->SetTimerValue(eTimer_Hold, QString::number(seconds));
	}
}

void MainWindowBase::on_actionSet_Main_Timer_triggered()
{
    // Note: this is implemented in the view as well!

//	if( m_pController->GetCurrentState() == eState_SonoMama ||
//		m_pController->GetCurrentState() == eState_TimerStopped )
    {
        bool ok(false);
        const QString time = QInputDialog::getText(
                                 this,
                                 tr("Set Value"),
                                 tr("Set value to (m:ss):"),
                                 QLineEdit::Normal,
                                 m_pController->GetTimeText(eTimer_Main),
                                 &ok);

        if (ok)
        {
            m_pController->SetTimerValue(eTimer_Main, time);
        }
    }
}


void MainWindowBase::write_specific_settings(QSettings&)
{
    qDebug() << "virtual function not implemented: " << __FUNCTION__;
}

void MainWindowBase::read_specific_settings(QSettings&)
{
    qDebug() << "virtual function not implemented: " << __FUNCTION__;
}

