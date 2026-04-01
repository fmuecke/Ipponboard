// Copyright 2018-2025 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "MainWindowBase.h"

#include "../base/InputBindingResolver.h"
#include "../base/OnlineVersionChecker.h"
#include "../base/SettingsDlg.h"
#include "../base/versioninfo.h"
#include "../core/Controller.h"
#include "../core/Enums.h"
#include "../core/Rules.h"
#include "../gamepad/Gamepad.h"
#include "../util/path_helpers.h"
#include "DonationManager.h"
#include "GamepadSectionMapper.h"
#include "View.h"

#include <QApplication>
#include <QDebug>
#include <QDesktopServices>
#include <QGuiApplication>
#include <QInputDialog>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QPushButton>
#include <QScreen>
#include <QSettings>
#include <QString>
#include <QStyle>
#include <QStyleHints>
#include <QTimer>
#include <QUrl>

using namespace GamepadLib;
using namespace Ipponboard;
using Point = Score::Point;

MainWindowBase::MainWindowBase(QWidget* parent)
    : QMainWindow(parent),
      m_pPrimaryView(),
      m_pSecondaryView(),
      m_pController(new Ipponboard::Controller()),
      m_fighterManager(),
      m_Language("en"),
      m_Theme(Qt::ColorScheme::Unknown),
      m_MatLabel("  Ipponboard   "),
      m_weights(),
      m_FighterNameFont("Calibri", 12, QFont::Bold, false),
      m_secondScreenNo(0),
      m_secondScreenSize(0, 0),
      m_secondScreenOffset(0, 0),
      m_controllerCfg(),
      m_pGamepad(new Gamepad())
{
    if (m_pGamepad)
    {
        const auto state = m_pGamepad->GetState();
        if (state == GamepadLib::EGamepadState::ok)
        {
            const auto* namePtr = m_pGamepad->GetProductName();
            QString name =
                (namePtr && *namePtr != L'\0') ? QString::fromWCharArray(namePtr) : QString();
            if (name.trimmed().isEmpty())
            {
                name = tr("Unnamed controller");
            }
            qInfo() << "Gamepad detected:" << name;
        }
        else
        {
            qInfo() << "No gamepad detected (state" << static_cast<int>(state) << ')';
        }
    }
}

MainWindowBase::~MainWindowBase() {}

void MainWindowBase::Init()
{
    setWindowTitle(QCoreApplication::applicationName());

    setWindowFlags(Qt::Window);
    //setWindowState(Qt::WindowMaximized);
    // instead, center window

    auto rect = QGuiApplication::screens().first()->availableGeometry();
    this->setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, this->size(), rect));

    load_fighters();

    // Setup views
    m_pPrimaryView.reset(new View(m_pController->GetIController(), Edition(), View::eTypePrimary));

    attach_primary_view();

    m_pSecondaryView.reset(
        new View(m_pController->GetIController(), Edition(), View::eTypeSecondary));
    m_pSecondaryView->setWindowFlag(Qt::FramelessWindowHint, true);

    // clear data
    m_pController->ClearFightsAndResetTimers();

    // Load settings
    read_settings();

    change_theme();
    change_lang(true);

    // Init gamepad
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindowBase::EvaluateInput);
    timer->start(75);
    update_statebar();
    ensureVersionStatusLabel();

    m_pController->RegisterView(m_pPrimaryView.get());
    m_pController->RegisterView(m_pSecondaryView.get());
    m_pController->RegisterView(static_cast<IView*>(this));
    m_pController->RegisterView(static_cast<IGoldenScoreView*>(this));

    if (m_checkVersionOnStartup)
    {
        startVersionCheck();
    }
}

void MainWindowBase::ensureVersionStatusLabel()
{
    if (m_pVersionStatusLabel != nullptr)
    {
        return;
    }

    auto* label = new QLabel(menuBar());
    label->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    label->setMargin(4);
    label->setMinimumWidth(220);
    label->setTextFormat(Qt::RichText);
    label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    label->setOpenExternalLinks(false);
    label->setVisible(false);
    connect(label,
            &QLabel::linkActivated,
            this,
            [this](const QString&)
            {
                if (m_versionStatusState == OnlineVersionChecker::State::NewerAvailable)
                {
                    showVersionDialog();
                }
            });
    menuBar()->setCornerWidget(label, Qt::TopRightCorner);
    m_pVersionStatusLabel = label;

    updateVersionStatusText();
}

void MainWindowBase::startVersionCheck()
{
    ensureVersionStatusLabel();
    if (m_isVersionCheckInProgress)
    {
        updateVersionStatusText();
        return;
    }

    m_isVersionCheckInProgress = true;
    updateVersionStatusText();

    OnlineVersionChecker::CheckOnlineVersionAsync(
        this,
        [this](OnlineVersionChecker::OnlineVersion onlineVersion)
        { applyVersionStatus(onlineVersion); });
}

void MainWindowBase::applyVersionStatus(const OnlineVersionChecker::OnlineVersion& onlineVersion)
{
    m_latestOnlineVersion = onlineVersion;
    m_isVersionCheckInProgress = false;
    m_versionStatusState = onlineVersion.state;
    updateVersionStatusText();
}

void MainWindowBase::showVersionDialog()
{
    if (m_versionStatusState != OnlineVersionChecker::State::NewerAvailable)
    {
        return;
    }

    const QString changes = (QCoreApplication::tr("en") == "de" ? m_latestOnlineVersion.changes_de
                                                                : m_latestOnlineVersion.changes_en)
                                .trimmed();

    const QString msg =
        QString("%1\n\n%2\n\n*%3*")
            .arg(QCoreApplication::tr("Version %1 available (currently using: %2)")
                     .arg(QString("**%1**").arg(m_latestOnlineVersion.version))
                     .arg(QString("`%1`").arg(QCoreApplication::applicationVersion())))
            .arg(changes)
            .arg(QCoreApplication::tr("Do you want to download it or visit the project homepage?"));

    QMessageBox versionBox(QMessageBox::Information,
                           QCoreApplication::tr("Ipponboard - New Version Available"),
                           msg,
                           QMessageBox::NoButton,
                           this);
    versionBox.setTextFormat(Qt::MarkdownText);
    versionBox.setTextInteractionFlags(Qt::TextBrowserInteraction);
    QAbstractButton* downloadButton =
        versionBox.addButton(QCoreApplication::tr("Download"), QMessageBox::ActionRole);
    QAbstractButton* homepageButton =
        versionBox.addButton(QCoreApplication::tr("Visit Homepage"), QMessageBox::ActionRole);
    versionBox.addButton(QCoreApplication::tr("Ignore"), QMessageBox::RejectRole);
    if (auto* pushButton = qobject_cast<QPushButton*>(homepageButton))
    {
        versionBox.setDefaultButton(pushButton);
    }

    versionBox.exec();
    QAbstractButton* clickedButton = versionBox.clickedButton();

    if (clickedButton == downloadButton)
    {
        qDebug() << "Opening download URL:" << m_latestOnlineVersion.downloadUrl;
        QDesktopServices::openUrl(QUrl(m_latestOnlineVersion.downloadUrl));
    }
    else if (clickedButton == homepageButton)
    {
        qDebug() << "Opening homepage URL:" << m_latestOnlineVersion.infoUrl;
        QDesktopServices::openUrl(QUrl(m_latestOnlineVersion.infoUrl));
    }
}

void MainWindowBase::updateVersionStatusText()
{
    if (m_pVersionStatusLabel == nullptr)
        return;

    QString text = "v" + QCoreApplication::applicationVersion();
    QString toolTip;

    if (!m_checkVersionOnStartup)
    {
        toolTip = tr("Online version check is disabled in the settings.");
    }
    else
    {
        if (m_isVersionCheckInProgress)
        {
            text = tr("Checking for updated version...");
        }
        else
        {
            switch (m_versionStatusState)
            {
            case OnlineVersionChecker::State::NewerAvailable:
                text = QStringLiteral("<a href=\"show-version-dialog\">%1</a>")
                           .arg(tr("Version %1 available!").arg(m_latestOnlineVersion.version));
                toolTip = tr("Click the link for details");
                break;

            case OnlineVersionChecker::State::UpToDate:
                text += QString(" (%1)").arg(tr("latest"));
                toolTip = tr("Version is up to date.");
                break;

            case OnlineVersionChecker::State::NewerThanOnlineAvailable:
                text += QString(" (%1)").arg(tr("newer"));
                toolTip = tr("Version is newer than online.");
                break;

            case OnlineVersionChecker::State::Empty:
            default:
                toolTip = tr("Online version check failed. See log for details.");
                break;
            }
        }
    }
    const QPalette pal = m_pVersionStatusLabel->style()->standardPalette();
    const QColor textColor = pal.color(QPalette::Active, QPalette::WindowText);
    const QColor dimmedColor = pal.color(QPalette::Disabled, QPalette::WindowText);
    const QColor linkColor = pal.color(QPalette::Active, QPalette::Link);

    m_pVersionStatusLabel->setText(text);
    m_pVersionStatusLabel->setToolTip(toolTip);
    m_pVersionStatusLabel->setTextInteractionFlags(
        m_versionStatusState == OnlineVersionChecker::State::NewerAvailable
            ? Qt::TextBrowserInteraction
            : Qt::NoTextInteraction);
    m_pVersionStatusLabel->setStyleSheet(
        m_versionStatusState == OnlineVersionChecker::State::NewerAvailable
            ? QStringLiteral("QLabel { padding: 4px; color: %1; } "
                             "QLabel a { color: %2; text-decoration: underline; }")
                  .arg(textColor.name(), linkColor.name())
            : QStringLiteral("QLabel { padding: 4px; color: %1; }").arg(dimmedColor.name()));
    m_pVersionStatusLabel->setVisible(!text.isEmpty());
}

QString MainWindowBase::GetFighterFileName() const
{
    return QString("Fighters%1.csv").arg(EditionNameShort());
}

void MainWindowBase::UpdateView() { update_views(); }

void MainWindowBase::changeEvent(QEvent* e)
{
    QMainWindow::changeEvent(e);

    switch (e->type())
    {
    case QEvent::LanguageChange:
        retranslate_Ui();
        updateVersionStatusText();
        break;

    default:
        break;
    }
}

void MainWindowBase::closeEvent(QCloseEvent* event)
{
    write_settings();
    save_fighters();

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
        m_pController->DoAction(eAction_Hajime_Mate, FighterEnum::Nobody);
        qDebug() << "Action [ Hajime/Mate ] was triggered by keyboard";
        break;

    case Qt::Key_Backspace:
        if (isCtrlPressed)
        {
            m_pController->DoAction(eAction_ResetAll, FighterEnum::Nobody);
            qDebug() << "Action [ Reset ] was triggered by keyboard";
        }

        break;

    case Qt::Key_Left:
    {
        if (eState_Holding == m_pController->GetCurrentState() &&
            FighterEnum::First != m_pController->GetLead())
        {
            m_pController->DoAction(eAction_SetOsaekomi, FighterEnum::First);
        }
        else
        {
            m_pController->DoAction(eAction_OsaeKomi_Toketa, FighterEnum::First);
        }

        qDebug() << "Action [ Osaekomi/Toketa for fighter1 ] was triggered by keyboard";
    }

    break;

    case Qt::Key_Right:
    {
        if (eState_Holding == m_pController->GetCurrentState() &&
            FighterEnum::Second != m_pController->GetLead())
        {
            m_pController->DoAction(eAction_SetOsaekomi, FighterEnum::Second);
        }
        else
        {
            m_pController->DoAction(eAction_OsaeKomi_Toketa, FighterEnum::Second);
        }

        qDebug() << "Action [ Osaekomi/Toketa for fighter2 ] was triggered by keyboard";
    }

    break;

    case Qt::Key_Down:
        //if (isCtrlPressed)
        {
            m_pController->DoAction(eAction_ResetOsaeKomi, FighterEnum::Nobody, true);
            qDebug() << "Action [ Reset Osaekomi ] was triggered by keyboard";
        }
        break;

    case Qt::Key_F5:
        m_pController->DoAction(eAction_Ippon, FighterEnum::First, isCtrlPressed);
        qDebug() << "Action [ Ippon for fighter1, revoke=" << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F6:
        m_pController->DoAction(eAction_Wazaari, FighterEnum::First, isCtrlPressed);
        qDebug() << "Action [ Wazaari for fighter1, revoke=" << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F7:
        m_pController->DoAction(eAction_Yuko, FighterEnum::First, isCtrlPressed);
        qDebug() << "Action [ Yuko for fighter1, revoke=" << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F8:
        m_pController->DoAction(eAction_Shido, FighterEnum::First, isCtrlPressed);
        qDebug() << "Action [ Shido for fighter1, revoke=" << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F9:
        m_pController->DoAction(eAction_Ippon, FighterEnum::Second, isCtrlPressed);
        qDebug() << "Action [ Ippon for fighter2, revoke=" << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F10:
        m_pController->DoAction(eAction_Wazaari, FighterEnum::Second, isCtrlPressed);
        qDebug() << "Action [ Wazaari for fighter2, revoke=" << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F11:
        m_pController->DoAction(eAction_Yuko, FighterEnum::Second, isCtrlPressed);
        qDebug() << "Action [ Yuko for fighter2, revoke=" << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    case Qt::Key_F12:
        m_pController->DoAction(eAction_Shido, FighterEnum::Second, isCtrlPressed);
        qDebug() << "Action [ Shido for fighter2, revoke=" << isCtrlPressed
                 << "] was triggered by keyboard";
        break;

    default:
        QMainWindow::keyPressEvent(event);
        break;
    }
}

void MainWindowBase::on_actionAbout_Ipponboard_triggered()
{
    QMessageBox::about(
        this,
        tr("About %1").arg(QCoreApplication::applicationName()),
        QString(
            "<h3>%1 v%2</h3>"
            "<p>Build: %3, Revision: %4</p>"
            "<p>&copy; 2010-%5 Florian M&uuml;cke &amp; contributors. All rights reserved.<br>For "
            "third party licenses see the User Manual.</p>"
            "<p><a "
            "href=\"https://github.com/fmuecke/Ipponboard\">github.com/fmuecke/Ipponboard</a></p>"
            "<p>Read how <a "
            "href=\"https://github.com/fmuecke/Ipponboard/blob/main/CONTRIBUTING.md\">you can "
            "contribute</a> and help Ipponboard improve. "
            "Please keep Ipponboard alive with <a href=\"%6\">a little donation.</a></p>"
            "<p>This program is provided AS IS with NO WARRANTY OF ANY KIND, "
            "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A "
            "PARTICULAR PURPOSE.</p>")
            .arg(QCoreApplication::applicationName(),
                 QCoreApplication::applicationVersion(),
                 VersionInfo::Date,
                 VersionInfo::Revision,
                 VersionInfo::CopyrightYear,
                 DonationManager::DonationUrl));
}

void MainWindowBase::on_actionUser_Manual_triggered()
{
    const auto manualPath = fm::GetProgramDataFilePath(tr("User-Manual.html"));
    QDesktopServices::openUrl(QUrl::fromLocalFile(manualPath));
}

void MainWindowBase::on_actionView_Logfile_triggered()
{
    const auto logPath = fm::GetLocalDataFilePath(QCoreApplication::applicationName() + ".log");
    QDesktopServices::openUrl(QUrl::fromLocalFile(logPath));

    // TODO: open log file in Log Window
}

void MainWindowBase::on_actionAutoAdjustPoints_toggled(bool checked)
{
    m_pController->SetAutoAdjustPoints(checked);
}

void MainWindowBase::on_actionVisit_Project_Homepage_triggered()
{
    QUrl url("https://github.com/fmuecke/Ipponboard");
    QDesktopServices::openUrl(url);
}

void MainWindowBase::on_actionOnline_Feedback_triggered()
{
    QUrl url("https://github.com/fmuecke/Ipponboard/issues");
    QDesktopServices::openUrl(url);
}

void MainWindowBase::change_lang(bool beQuiet)
{
    ui_check_language_items();

    if (!beQuiet)
    {
        QMessageBox::information(
            this,
            QCoreApplication::applicationName(),
            tr("Please restart the application so that the change can take effect."));
    }
}

void MainWindowBase::change_theme()
{
    ui_check_theme_items();

    qDebug() << "setting color theme...";
    qDebug() << "style:" << qApp->style()->objectName();
    qDebug() << "scheme before:" << qApp->styleHints()->colorScheme();

    QGuiApplication::styleHints()->setColorScheme(m_Theme);

    qDebug() << "scheme after:" << qApp->styleHints()->colorScheme();
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

void MainWindowBase::on_actionLang_Dutch_triggered(bool val)
{
    if (val)
    {
        m_Language = "nl";
        change_lang();
    }
}

void MainWindowBase::on_actionThemeAutomatic_triggered(bool val)
{
    if (val)
    {
        m_Theme = Qt::ColorScheme::Unknown;
        change_theme();
    }
    else
    {
        ui_check_theme_items();
    }
}

void MainWindowBase::on_actionThemeDark_triggered(bool val)
{
    if (val)
    {
        m_Theme = Qt::ColorScheme::Dark;
        change_theme();
    }
    else
    {
        ui_check_theme_items();
    }
}

void MainWindowBase::on_actionThemeLight_triggered(bool val)
{
    if (val)
    {
        m_Theme = Qt::ColorScheme::Light;
        change_theme();
    }
    else
    {
        ui_check_theme_items();
    }
}

void MainWindowBase::on_actionRulesClassic_triggered(bool checked)
{
    if (checked)
    {
        m_pController->SetRules(std::make_shared<ClassicRules>());
        ui_check_rules_items();
    }
}

void MainWindowBase::on_actionRules2013_triggered(bool checked)
{
    if (checked)
    {
        m_pController->SetRules(std::make_shared<Rules2013>());
        ui_check_rules_items();
    }
}

void MainWindowBase::on_actionRules2017_triggered(bool checked)
{
    if (checked)
    {
        m_pController->SetRules(std::make_shared<Rules2017>());
        ui_check_rules_items();
    }
}

void MainWindowBase::on_actionRules2017U15_triggered(bool checked)
{
    if (checked)
    {
        m_pController->SetRules(std::make_shared<Rules2017U15>());
        ui_check_rules_items();
    }
}

void MainWindowBase::on_actionRules2018_triggered(bool checked)
{
    if (checked)
    {
        m_pController->SetRules(std::make_shared<Rules2018>());
        ui_check_rules_items();
    }
}

void MainWindowBase::on_actionRules2025_triggered(bool checked)
{
    if (checked)
    {
        m_pController->SetRules(std::make_shared<Rules2025>());
        ui_check_rules_items();
    }
}

void MainWindowBase::write_settings() const
{
    QSettings settings;
    qInfo() << "Writing user settings.";
    settings.setValue("SettingsVersion", 1);

    settings.beginGroup(settings::str_Main);
    {
        settings.remove("");
        settings.setValue(settings::str_Version, VersionInfo::VersionStr);
        settings.setValue(settings::str_Language, m_Language);
        settings.setValue(settings::str_Theme, static_cast<int>(m_Theme));
        settings.setValue(settings::str_SecondScreen, m_secondScreenNo);
        settings.setValue(settings::str_SecondScreenSize, m_secondScreenSize);
        settings.setValue(settings::str_SecondScreenOffset, m_secondScreenOffset);
        settings.setValue(settings::str_checkVersionOnStartup, m_checkVersionOnStartup);
    }
    settings.endGroup();

    settings.beginGroup(settings::str_Fonts);
    {
        settings.remove("");
        settings.setValue(settings::str_TextFont1, m_pPrimaryView->GetInfoHeaderFont().toString());
        settings.setValue(settings::str_FighterNameFont, m_FighterNameFont.toString());
        settings.setValue(settings::str_DigitFont, m_pPrimaryView->GetDigitFont().toString());
    }
    settings.endGroup();

    settings.beginGroup(settings::str_Colors);
    {
        settings.remove("");
        settings.setValue(settings::str_InfoTextColor, m_pPrimaryView->GetInfoTextColor());
        settings.setValue(settings::str_InfoTextBgColor, m_pPrimaryView->GetInfoTextBgColor());
        settings.setValue(settings::str_TextColorFirst, m_pPrimaryView->GetTextColorFirst());
        settings.setValue(settings::str_TextBgColorFirst, m_pPrimaryView->GetTextBgColorFirst());
        settings.setValue(settings::str_TextColorSecond, m_pPrimaryView->GetTextColorSecond());
        settings.setValue(settings::str_TextBgColorSecond, m_pPrimaryView->GetTextBgColorSecond());
        //settings.setValue(settings::str_MainClockColorRunning, m_pPrimaryView->GetMainClockColor1());
        //settings.setValue(settings::str_MainClockColorStopped, m_pPrimaryView->GetMainClockColor2());
    }
    settings.endGroup();

    settings.beginGroup(settings::str_Input);
    {
        settings.remove("");
        settings.setValue(settings::str_buttonHajimeMate, m_controllerCfg.button_hajime_mate);
        settings.setValue(settings::str_buttonNext, m_controllerCfg.button_next);
        settings.setValue(settings::str_buttonPrev, m_controllerCfg.button_prev);
        settings.setValue(settings::str_buttonPause, m_controllerCfg.button_pause);
        settings.setValue(settings::str_buttonReset, m_controllerCfg.button_reset);
        settings.setValue(settings::str_buttonReset2, m_controllerCfg.button_reset_2);
        settings.setValue(settings::str_buttonResetHoldFirst,
                          m_controllerCfg.button_reset_hold_first);
        settings.setValue(settings::str_buttonResetHoldSecond,
                          m_controllerCfg.button_reset_hold_second);
        settings.setValue(settings::str_buttonFirstHolding,
                          m_controllerCfg.button_osaekomi_toketa_first);
        settings.setValue(settings::str_buttonSecondHolding,
                          m_controllerCfg.button_osaekomi_toketa_second);
        settings.setValue(settings::str_buttonHansokumakeFirst,
                          m_controllerCfg.button_hansokumake_first);
        settings.setValue(settings::str_buttonHansokumakeSecond,
                          m_controllerCfg.button_hansokumake_second);
        settings.setValue(settings::str_invertX, m_controllerCfg.axis_inverted_X);
        settings.setValue(settings::str_invertY, m_controllerCfg.axis_inverted_Y);
        settings.setValue(settings::str_invertR, m_controllerCfg.axis_inverted_R);
        settings.setValue(settings::str_invertZ, m_controllerCfg.axis_inverted_Z);
    }
    settings.endGroup();

    settings.beginGroup(settings::str_InputRaw);
    {
        settings.remove("");
        settings.setValue(settings::str_raw_buttonHajimeMate,
                          m_controllerCfg.button_hajime_mate_raw);
        settings.setValue(settings::str_raw_buttonNext, m_controllerCfg.button_next_raw);
        settings.setValue(settings::str_raw_buttonPrev, m_controllerCfg.button_prev_raw);
        settings.setValue(settings::str_raw_buttonPause, m_controllerCfg.button_pause_raw);
        settings.setValue(settings::str_raw_buttonReset, m_controllerCfg.button_reset_raw);
        settings.setValue(settings::str_raw_buttonReset2, m_controllerCfg.button_reset2_raw);
        settings.setValue(settings::str_raw_buttonFirstHolding,
                          m_controllerCfg.button_osaekomi_toketa_first_raw);
        settings.setValue(settings::str_raw_buttonSecondHolding,
                          m_controllerCfg.button_osaekomi_toketa_second_raw);
        settings.setValue(settings::str_raw_buttonResetHoldFirst,
                          m_controllerCfg.button_reset_hold_first_raw);
        settings.setValue(settings::str_raw_buttonResetHoldSecond,
                          m_controllerCfg.button_reset_hold_second_raw);
        settings.setValue(settings::str_raw_buttonHansokumakeFirst,
                          m_controllerCfg.button_hansokumake_first_raw);
        settings.setValue(settings::str_raw_buttonHansokumakeSecond,
                          m_controllerCfg.button_hansokumake_second_raw);

        settings.setValue(settings::str_axisLeftX, m_controllerCfg.axis_left_x);
        settings.setValue(settings::str_axisLeftY, m_controllerCfg.axis_left_y);
        settings.setValue(settings::str_axisRightX, m_controllerCfg.axis_right_x);
        settings.setValue(settings::str_axisRightY, m_controllerCfg.axis_right_y);

        settings.setValue(settings::str_axisLeftInvertX, m_controllerCfg.axis_left_invert_x);
        settings.setValue(settings::str_axisLeftInvertY, m_controllerCfg.axis_left_invert_y);
        settings.setValue(settings::str_axisRightInvertX, m_controllerCfg.axis_right_invert_x);
        settings.setValue(settings::str_axisRightInvertY, m_controllerCfg.axis_right_invert_y);
    }
    settings.endGroup();

    settings.beginGroup(settings::str_Sounds);
    {
        settings.remove("");
        auto soundFile = m_pController->GetMatSignal();
        settings.setValue(settings::str_MatSignal, soundFile);
    }
    settings.endGroup();

    settings.beginGroup(settings::str_Options);
    {
        settings.remove("");
        settings.setValue(settings::str_autoAdjustPoints, m_pController->IsAutoAdjustPoints());
    }
    settings.endGroup();
}

void MainWindowBase::read_settings()
{
    qInfo() << "Reading user settings";
    QSettings settings;
    int version = settings.value("SettingsVersion", 1).toInt();
    qInfo() << "Settings version is" << version;

    settings.beginGroup(settings::str_Main);
    {
        QString langStr = QLocale::system().name();
        langStr.truncate(langStr.lastIndexOf('_'));
        m_Language = settings.value(settings::str_Language, langStr).toString();
        auto themeVal =
            settings.value(settings::str_Theme, static_cast<int>(Qt::ColorScheme::Unknown)).toInt();
        m_Theme = (themeVal == 1 || themeVal == 2) ? static_cast<Qt::ColorScheme>(themeVal)
                                                   : Qt::ColorScheme::Unknown;
        QGuiApplication::styleHints()->setColorScheme(m_Theme);
        m_secondScreenNo = settings.value(settings::str_SecondScreen, 0).toInt();
        m_secondScreenSize = settings.value(settings::str_SecondScreenSize, QSize(0, 0)).toSize();
        m_secondScreenOffset =
            settings.value(settings::str_SecondScreenOffset, QPoint(0, 0)).toPoint();
        if (m_secondScreenNo >= 0 && !m_secondScreenSize.isNull())
        {
            qInfo()
                << "Detected legacy second screen size for fullscreen setup; resetting to auto.";
            m_secondScreenSize = QSize(0, 0);
        }
        m_checkVersionOnStartup =
            settings.value(settings::str_checkVersionOnStartup, true).toBool();
        update_statebar();
    }
    settings.endGroup();

    auto& pV = m_pPrimaryView;
    auto& sV = m_pSecondaryView;
    //
    // Fonts
    //
    settings.beginGroup(settings::str_Fonts);
    {
        QFont font = pV->GetInfoHeaderFont();
        font.fromString(settings.value(settings::str_TextFont1, font.toString()).toString());
        pV->SetInfoHeaderFont(pV->GetInfoHeaderFont());
        sV->SetInfoHeaderFont(sV->GetInfoHeaderFont());

        font = pV->GetFighterNameFont();
        font.fromString(settings.value(settings::str_FighterNameFont, font.toString()).toString());
        update_fighter_name_font(font);

        font = pV->GetDigitFont();
        font.fromString(settings.value(settings::str_DigitFont, font.toString()).toString());
        pV->SetDigitFont(font);
        sV->SetDigitFont(font);
        //m_pScoreScreen->SetDigitFont(font);
    }
    settings.endGroup();

    //
    // Colors
    //
    settings.beginGroup(settings::str_Colors);
    {
        auto fgColor =
            settings.value(settings::str_InfoTextColor, sV->GetInfoTextColor()).value<QColor>();
        auto bgColor =
            settings.value(settings::str_InfoTextBgColor, sV->GetInfoTextBgColor()).value<QColor>();

        update_info_text_color(fgColor, bgColor);

        fgColor =
            settings.value(settings::str_TextColorFirst, sV->GetTextColorFirst()).value<QColor>();
        bgColor = settings.value(settings::str_TextBgColorFirst, sV->GetTextBgColorFirst())
                      .value<QColor>();

        update_text_color_first(fgColor, bgColor);

        fgColor =
            settings.value(settings::str_TextColorSecond, sV->GetTextColorSecond()).value<QColor>();
        bgColor = settings.value(settings::str_TextBgColorSecond, sV->GetTextBgColorSecond())
                      .value<QColor>();

        update_text_color_second(fgColor, bgColor);

        //fgColor = m_pPrimaryView->GetMainClockColor1();
        //bgColor = m_pPrimaryView->GetMainClockColor2();
        //if( settings.contains(settings::str_MainClockColorRunning) )
        //	fgColor = settings.value(settings::str_MainClockColorRunning).value<QColor>();
        //if( settings.contains(settings::str_MainClockColorStopped) )
        //	bgColor = settings.value(settings::str_MainClockColorStopped).value<QColor>();
        //m_pPrimaryView->SetMainClockColor(fgColor, bgColor);
        //m_pSecondaryView->SetMainClockColor(fgColor, bgColor);
    }
    settings.endGroup();

    settings.beginGroup(settings::str_Input);
    {
        using eb = GamepadLib::EButton;

        m_controllerCfg.button_hajime_mate =
            settings.value(settings::str_buttonHajimeMate, eb::button_pov_back).toInt();

        m_controllerCfg.button_next =
            settings.value(settings::str_buttonNext, eb::button10).toInt();

        m_controllerCfg.button_prev = settings.value(settings::str_buttonPrev, eb::button9).toInt();

        m_controllerCfg.button_pause =
            settings.value(settings::str_buttonPause, eb::button2).toInt();

        m_controllerCfg.button_reset =
            settings.value(settings::str_buttonReset, eb::button1).toInt();

        m_controllerCfg.button_reset_2 =
            settings.value(settings::str_buttonReset2, eb::button4).toInt();

        m_controllerCfg.button_reset_hold_first =
            settings.value(settings::str_buttonResetHoldFirst, eb::button6).toInt();

        m_controllerCfg.button_reset_hold_second =
            settings.value(settings::str_buttonResetHoldSecond, eb::button8).toInt();

        m_controllerCfg.button_osaekomi_toketa_first =
            settings.value(settings::str_buttonFirstHolding, eb::button5).toInt();

        m_controllerCfg.button_osaekomi_toketa_second =
            settings.value(settings::str_buttonSecondHolding, eb::button7).toInt();

        m_controllerCfg.button_hansokumake_first =
            settings.value(settings::str_buttonHansokumakeFirst, eb::button11).toInt();

        m_controllerCfg.button_hansokumake_second =
            settings.value(settings::str_buttonHansokumakeSecond, eb::button12).toInt();

        m_controllerCfg.axis_inverted_X = settings.value(settings::str_invertX, false).toBool();
        m_controllerCfg.axis_inverted_Y = settings.value(settings::str_invertY, true).toBool();
        m_controllerCfg.axis_inverted_R = settings.value(settings::str_invertR, true).toBool();
        m_controllerCfg.axis_inverted_Z = settings.value(settings::str_invertZ, true).toBool();
        // apply settings to gamepad controller
        m_pGamepad->SetInverted(GamepadLib::EAxis::X, m_controllerCfg.axis_inverted_X);
        m_pGamepad->SetInverted(GamepadLib::EAxis::Y, m_controllerCfg.axis_inverted_Y);
        m_pGamepad->SetInverted(GamepadLib::EAxis::R, m_controllerCfg.axis_inverted_R);
        m_pGamepad->SetInverted(GamepadLib::EAxis::Z, m_controllerCfg.axis_inverted_Z);
    }
    settings.endGroup();

    settings.beginGroup(settings::str_InputRaw);
    {
        m_controllerCfg.button_hajime_mate_raw =
            settings.value(settings::str_raw_buttonHajimeMate, -1).toInt();
        m_controllerCfg.button_next_raw = settings.value(settings::str_raw_buttonNext, -1).toInt();
        m_controllerCfg.button_prev_raw = settings.value(settings::str_raw_buttonPrev, -1).toInt();
        m_controllerCfg.button_pause_raw =
            settings.value(settings::str_raw_buttonPause, -1).toInt();
        m_controllerCfg.button_reset_raw =
            settings.value(settings::str_raw_buttonReset, -1).toInt();
        m_controllerCfg.button_reset2_raw =
            settings.value(settings::str_raw_buttonReset2, -1).toInt();
        m_controllerCfg.button_osaekomi_toketa_first_raw =
            settings.value(settings::str_raw_buttonFirstHolding, -1).toInt();
        m_controllerCfg.button_reset_hold_first_raw =
            settings.value(settings::str_raw_buttonResetHoldFirst, -1).toInt();
        m_controllerCfg.button_hansokumake_first_raw =
            settings.value(settings::str_raw_buttonHansokumakeFirst, -1).toInt();
        m_controllerCfg.button_osaekomi_toketa_second_raw =
            settings.value(settings::str_raw_buttonSecondHolding, -1).toInt();
        m_controllerCfg.button_reset_hold_second_raw =
            settings.value(settings::str_raw_buttonResetHoldSecond, -1).toInt();
        m_controllerCfg.button_hansokumake_second_raw =
            settings.value(settings::str_raw_buttonHansokumakeSecond, -1).toInt();

        m_controllerCfg.axis_left_x = settings.value(settings::str_axisLeftX, -1).toInt();
        m_controllerCfg.axis_left_y = settings.value(settings::str_axisLeftY, -1).toInt();
        m_controllerCfg.axis_right_x = settings.value(settings::str_axisRightX, -1).toInt();
        m_controllerCfg.axis_right_y = settings.value(settings::str_axisRightY, -1).toInt();

        m_controllerCfg.axis_left_invert_x =
            settings.value(settings::str_axisLeftInvertX, false).toBool();
        m_controllerCfg.axis_left_invert_y =
            settings.value(settings::str_axisLeftInvertY, false).toBool();
        m_controllerCfg.axis_right_invert_x =
            settings.value(settings::str_axisRightInvertX, false).toBool();
        m_controllerCfg.axis_right_invert_y =
            settings.value(settings::str_axisRightInvertY, false).toBool();
    }
    settings.endGroup();

    settings.beginGroup(settings::str_Sounds);
    {
        m_pController->SetMatSignal(settings.value(settings::str_MatSignal, "Gong").toString());
    }
    settings.endGroup();

    settings.beginGroup(settings::str_Options);
    {
        const auto isAutoAdjust = settings.value(settings::str_autoAdjustPoints, true).toBool();
        m_pController->SetAutoAdjustPoints(isAutoAdjust);
    }
    settings.endGroup();

    // update views
    update_views();
}

void MainWindowBase::load_fighters()
{
    QString csvFile(fm::GetConfigFilePath(GetFighterFileName().toLatin1()));

    QString errorMsg;

    if (!QFile::exists(csvFile))
    {
        // silently ignore
        return;
    }

    if (!m_fighterManager.ImportFighters(csvFile, FighterManager::DefaultExportFormat(), errorMsg))
    {
        QMessageBox::critical(this, QCoreApplication::applicationName(), errorMsg);
    }
}

void MainWindowBase::save_fighters()
{
    QString csvFile(fm::GetConfigFilePath(GetFighterFileName().toLatin1()));
    QString errorMsg;

    if (!m_fighterManager.ExportFighters(csvFile, FighterManager::DefaultExportFormat(), errorMsg))
    {
        QMessageBox::critical(this, QCoreApplication::applicationName(), errorMsg);
    }
}

void MainWindowBase::update_views()
{
    m_pPrimaryView->UpdateView();
    m_pSecondaryView->UpdateView();
}

void MainWindowBase::on_actionTest_Gong_triggered() { m_pController->Gong(); }

void MainWindowBase::on_actionShow_SecondaryView_triggered()
{
    show_hide_view();

    if (m_pSecondaryView->isVisible())
    {
    }
}

void MainWindowBase::on_actionPreferences_triggered()
{
    SettingsDlg dlg(Edition(), this);
    dlg.SetInfoHeaderSettings(m_pPrimaryView->GetInfoHeaderFont(),
                              m_pPrimaryView->GetInfoTextColor(),
                              m_pPrimaryView->GetInfoTextBgColor());

    dlg.SetFighterNameFont(m_FighterNameFont);
    dlg.SetTextColorsFirst(m_pPrimaryView->GetTextColorFirst(),
                           m_pPrimaryView->GetTextBgColorFirst());
    dlg.SetTextColorsSecond(m_pPrimaryView->GetTextColorSecond(),
                            m_pPrimaryView->GetTextBgColorSecond());
    dlg.SetScreensSettings(m_secondScreenNo, m_secondScreenSize, m_secondScreenOffset);
    dlg.SetGamepad(m_pGamepad.get());
    dlg.SetControllerConfig(&m_controllerCfg);
    dlg.SetLabels(m_MatLabel, m_pController->GetHomeLabel(), m_pController->GetGuestLabel());
    dlg.SetMatSignal(m_pController->GetMatSignal());
    dlg.SetCheckVersionOnStartup(m_checkVersionOnStartup);

    const bool wasSuppressed = is_input_suppressed();
    set_input_suppressed(true);
    const int dialogResult = dlg.exec();
    set_input_suppressed(wasSuppressed);

    if (QDialog::Accepted == dialogResult)
    {
        m_pPrimaryView->SetInfoHeaderFont(dlg.GetInfoHeaderFont());
        m_pSecondaryView->SetInfoHeaderFont(dlg.GetInfoHeaderFont());
        update_fighter_name_font(dlg.GetFighterNameFont());
        update_info_text_color(dlg.GetInfoTextColor(), dlg.GetInfoTextBgColor());
        update_text_color_first(dlg.GetTextColorFirst(), dlg.GetTextBgColorFirst());
        update_text_color_second(dlg.GetTextColorSecond(), dlg.GetTextBgColorSecond());

        m_secondScreenNo = dlg.GetSelectedScreen();
        m_secondScreenSize = dlg.GetSize();
        m_secondScreenOffset = dlg.GetOffset();

        dlg.GetControllerConfig(&m_controllerCfg);
        // apply settings to gamepad
        m_pGamepad->SetInverted(GamepadLib::EAxis::X, m_controllerCfg.axis_inverted_X);
        m_pGamepad->SetInverted(GamepadLib::EAxis::Y, m_controllerCfg.axis_inverted_Y);
        m_pGamepad->SetInverted(GamepadLib::EAxis::R, m_controllerCfg.axis_inverted_R);
        m_pGamepad->SetInverted(GamepadLib::EAxis::Z, m_controllerCfg.axis_inverted_Z);
        //#endif
        m_MatLabel = dlg.GetMatLabel();
        m_pController->SetLabels(dlg.GetHomeLabel(), dlg.GetGuestLabel());
        m_checkVersionOnStartup = dlg.GetCheckVersionOnStartup();

        m_pPrimaryView->SetMat(m_MatLabel);
        m_pSecondaryView->SetMat(m_MatLabel);
        m_pController->SetMatSignal(dlg.GetMatSignal());

        // save changes to file
        write_settings();

        update_statebar();
        update_views();
    }
}

void MainWindowBase::update_screen_visibility(QWidget* pView) const
{
    const auto screens = QGuiApplication::screens();
    QPoint screenOrigin(0, 0);
    bool hasTargetScreen = false;
    if (!screens.isEmpty() && m_secondScreenNo >= 0 && m_secondScreenNo < screens.size())
    {
        const auto screenRes = screens.at(m_secondScreenNo)->geometry();
        screenOrigin = screenRes.topLeft();
        hasTargetScreen = true;
    }

    const QPoint targetPos =
        hasTargetScreen ? screenOrigin + m_secondScreenOffset : m_secondScreenOffset;

    if (m_secondScreenSize.isNull())
    {
        pView->showFullScreen();
        pView->move(targetPos);
    }
    else
    {
        pView->move(targetPos);
        pView->resize(m_secondScreenSize);
        pView->show();
    }
}

void MainWindowBase::show_hide_view() const
{
    static bool isAlreadyCalled = false; // this line will only be called once!

    if (isAlreadyCalled)
    {
        return;
    }

    isAlreadyCalled = true; // prevents recursive calls (TODO: implement better)

    if (m_pSecondaryView->isHidden())
    {
        update_screen_visibility(m_pSecondaryView.get());
    }
    else
    {
        m_pSecondaryView->hide();
    }

    ui_check_show_secondary_view(!m_pSecondaryView->isHidden());
    isAlreadyCalled = false;
}

void MainWindowBase::EvaluateInput()
{
    if (is_input_suppressed())
    {
        return;
    }

    if (GamepadLib::EGamepadState::ok != m_pGamepad->GetState())
    {
        return;
    }

    m_pGamepad->ReadData();

    if (EvaluateSpecificInput(m_pGamepad.get()))
    {
        return;
    }

    const auto wasPressed = [this](int rawCode, int standardCode)
    {
        return ResolveRawFirst(
            rawCode,
            standardCode,
            [this](std::uint16_t code) { return m_pGamepad->WasPressedRaw(code); },
            [this](GamepadLib::EButton button) { return m_pGamepad->WasPressed(button); });
    };

    const auto isPressed = [this](int rawCode, int standardCode)
    {
        return ResolveRawFirst(
            rawCode,
            standardCode,
            [this](std::uint16_t code) { return m_pGamepad->IsPressedRaw(code); },
            [this](GamepadLib::EButton button) { return m_pGamepad->IsPressed(button); });
    };

    if (wasPressed(m_controllerCfg.button_hajime_mate_raw, m_controllerCfg.button_hajime_mate))
    {
        m_pController->DoAction(eAction_Hajime_Mate, FighterEnum::Nobody);
    }
    else if (wasPressed(m_controllerCfg.button_reset_hold_first_raw,
                        m_controllerCfg.button_reset_hold_first))
    {
        m_pController->DoAction(eAction_ResetOsaeKomi, FighterEnum::First, true);
    }
    else if (wasPressed(m_controllerCfg.button_reset_hold_second_raw,
                        m_controllerCfg.button_reset_hold_second))
    {
        m_pController->DoAction(eAction_ResetOsaeKomi, FighterEnum::Second, true);
    }
    else if (wasPressed(m_controllerCfg.button_osaekomi_toketa_first_raw,
                        m_controllerCfg.button_osaekomi_toketa_first))
    {
        if (eState_Holding == m_pController->GetCurrentState() &&
            FighterEnum::First != m_pController->GetLead())
        {
            m_pController->DoAction(eAction_SetOsaekomi, FighterEnum::First);
        }
        else
        {
            m_pController->DoAction(eAction_OsaeKomi_Toketa, FighterEnum::First);
        }
    }
    else if (wasPressed(m_controllerCfg.button_osaekomi_toketa_second_raw,
                        m_controllerCfg.button_osaekomi_toketa_second))
    {
        if (eState_Holding == m_pController->GetCurrentState() &&
            FighterEnum::Second != m_pController->GetLead())
        {
            m_pController->DoAction(eAction_SetOsaekomi, FighterEnum::Second);
        }
        else
        {
            m_pController->DoAction(eAction_OsaeKomi_Toketa, FighterEnum::Second);
        }
    }
    // reset
    else if (isPressed(m_controllerCfg.button_reset_raw, m_controllerCfg.button_reset) &&
             isPressed(m_controllerCfg.button_reset2_raw, m_controllerCfg.button_reset_2))
    {
        m_pController->DoAction(eAction_ResetAll, FighterEnum::Nobody);
    }

    // hansokumake fighter1
    else if (wasPressed(m_controllerCfg.button_hansokumake_first_raw,
                        m_controllerCfg.button_hansokumake_first))
    {
        const bool revoke(m_pController->GetScore(FighterEnum::First, Point::Hansokumake) != 0);
        m_pController->DoAction(eAction_Hansokumake, FighterEnum::First, revoke);
    }
    // hansokumake fighter2
    else if (wasPressed(m_controllerCfg.button_hansokumake_second_raw,
                        m_controllerCfg.button_hansokumake_second))
    {
        const bool revoke(m_pController->GetScore(FighterEnum::Second, Point::Hansokumake) != 0);
        m_pController->DoAction(eAction_Hansokumake, FighterEnum::Second, revoke);
    }
    else
    {
        const auto& actions = GamepadSectionMapper::Actions();

        const auto lastSectionXY = m_pGamepad->GetLastSection(EAxis::X, EAxis::Y);
        const auto currSectionXY = m_pGamepad->GetSection(EAxis::X, EAxis::Y);
        if (lastSectionXY != currSectionXY && currSectionXY != 0)
        {
            const auto& action = actions[currSectionXY];
            m_pController->DoAction(action.action, FighterEnum::First, action.revoke);
            return;
        }

        const auto lastSectionRZ = m_pGamepad->GetLastSection(EAxis::R, EAxis::Z);
        const auto currSectionRZ = m_pGamepad->GetSection(EAxis::R, EAxis::Z);
        if (lastSectionRZ != currSectionRZ && currSectionRZ != 0)
        {
            const auto& action = actions[currSectionRZ];
            m_pController->DoAction(action.action, FighterEnum::First, action.revoke);
            return;
        }
    }
}

void MainWindowBase::update_info_text_color(const QColor& color, const QColor& bgColor)
{
    m_pPrimaryView->SetInfoTextColor(color, bgColor);
    m_pSecondaryView->SetInfoTextColor(color, bgColor);
}

void MainWindowBase::update_text_color_first(const QColor& color, const QColor& bgColor)
{
    m_pPrimaryView->SetTextColorFirst(color, bgColor);
    m_pSecondaryView->SetTextColorFirst(color, bgColor);
}

void MainWindowBase::update_text_color_second(const QColor& color, const QColor& bgColor)
{
    m_pPrimaryView->SetTextColorSecond(color, bgColor);
    m_pSecondaryView->SetTextColorSecond(color, bgColor);
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
    m_pController->DoAction(eAction_ResetAll, FighterEnum::Nobody, false);
}

void MainWindowBase::on_action_Info_Header_triggered(bool val)
{
    m_pPrimaryView->SetShowInfoHeader(val);
    m_pSecondaryView->SetShowInfoHeader(val);
}

void MainWindowBase::on_actionSet_Hold_Timer_triggered()
{
    bool ok(false);
    const int seconds = QInputDialog::getInt(this,
                                             tr("Set Value"),
                                             tr("Set value to (ss):"),
                                             0,  // value
                                             0,  // min
                                             59, // max
                                             1,  // step
                                             &ok);

    if (ok)
    {
        m_pController->SetTimerValue(eTimer_Hold, QString::number(seconds));
    }
}

void MainWindowBase::on_actionSet_Main_Timer_triggered()
{
    // Note: this is implemented in the view as well!

    //	if( m_pController->GetCurrentState() == EGamepadState::SonoMama ||
    //		m_pController->GetCurrentState() == EGamepadState::TimerStopped )
    {
        bool ok(false);
        const QString time = QInputDialog::getText(this,
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

void MainWindowBase::update_statebar()
{
    qDebug() << "virtual function not implemented: " << __FUNCTION__;
}
