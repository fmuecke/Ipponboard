// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
#include "MainWindowBase.h"
#include "ui_MainWindow.h"

#include "View.h"
#include "../core/Controller.h"
#include "../core/Fighter.h"
#include "../base/versioninfo.h"
#include "../base/SettingsDlg.h"
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
	, m_fighterManager()
	, m_Language("en")
	, m_MatLabel()
	, m_weights()
	, m_FighterNameFont("Calibri", 12, QFont::Bold, false)
	, m_secondScreenNo(0)
	, m_secondScreenSize()
	, m_bAutoSize(true)
	, m_bAlwaysShow(true)
	, m_controllerCfg()
	, m_pGamepad(new Gamepad)
{
}

MainWindowBase::~MainWindowBase()
{
}

void MainWindowBase::Init()
{
	setWindowFlags(Qt::Window);
	setWindowState(Qt::WindowMaximized);

	load_fighters();

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

	// set default background
	QString styleSheet("background-color:black; color:white");

	m_pUi->frame_primary_view->setStyleSheet(styleSheet);
	m_pSecondaryView->setStyleSheet(styleSheet);

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

	update_statebar();

    m_pController->RegisterView(m_pPrimaryView.get());
    m_pController->RegisterView(m_pSecondaryView.get());
    m_pController->RegisterView(this);
}

QString MainWindowBase::GetConfigFileName() const
{
	return QString("Ipponboard%1.ini").arg(EditionNameShort());
}

QString MainWindowBase::GetFighterFileName() const
{
    return QString("Fighters%1.csv").arg(EditionNameShort());
}

void MainWindowBase::UpdateView()
{
    update_views();
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
		m_pController->DoAction(Ipponboard::eAction_Hajime_Mate, Ipponboard::eFighterNobody);
		qDebug() << "Action [ Hajime/Mate ] was triggered by keyboard";
		break;

	case Qt::Key_Backspace:
		if (isCtrlPressed)
		{
			m_pController->DoAction(Ipponboard::eAction_ResetAll, Ipponboard::eFighterNobody);
			qDebug() << "Action [ Reset ] was triggered by keyboard";
		}

		break;

	case Qt::Key_Left:
		{
			if (eState_Holding == m_pController->GetCurrentState() &&
					Ipponboard::eFighter1 != m_pController->GetLead())
			{
				m_pController->DoAction(Ipponboard::eAction_SetOsaekomi,
										Ipponboard::eFighter1);
			}
			else
			{
				m_pController->DoAction(Ipponboard::eAction_OsaeKomi_Toketa,
										Ipponboard::eFighter1);
			}

			qDebug() << "Action [ Osaekomi/Toketa for fighter1 ] was triggered by keyboard";
		}

		break;

	case Qt::Key_Right:
		{
			if (eState_Holding == m_pController->GetCurrentState() &&
					Ipponboard::eFighter2 != m_pController->GetLead())
			{
				m_pController->DoAction(Ipponboard::eAction_SetOsaekomi,
										Ipponboard::eFighter2);
			}
			else
			{
				m_pController->DoAction(Ipponboard::eAction_OsaeKomi_Toketa,
										Ipponboard::eFighter2);
			}

			qDebug() << "Action [ Osaekomi/Toketa for fighter2 ] was triggered by keyboard";
		}

		break;

	case Qt::Key_Down:
		//if (isCtrlPressed)
		{
			m_pController->DoAction(Ipponboard::eAction_ResetOsaeKomi,
									Ipponboard::eFighterNobody,
									true);
			qDebug() << "Action [ Reset Osaekomi ] was triggered by keyboard";
		}
		break;

	case Qt::Key_F5:
		m_pController->DoAction(Ipponboard::eAction_Ippon,
								Ipponboard::eFighter1,
								isCtrlPressed);
		qDebug() << "Action [ Ippon for fighter1, revoke="
				 << isCtrlPressed
				 << "] was triggered by keyboard";
		break;

	case Qt::Key_F6:
		m_pController->DoAction(Ipponboard::eAction_Wazaari,
								Ipponboard::eFighter1,
								isCtrlPressed);
		qDebug() << "Action [ Wazaari for fighter1, revoke="
				 << isCtrlPressed
				 << "] was triggered by keyboard";
		break;

	case Qt::Key_F7:
		m_pController->DoAction(Ipponboard::eAction_Yuko,
								Ipponboard::eFighter1,
								isCtrlPressed);
		qDebug() << "Action [ Yuko for fighter1, revoke="
				 << isCtrlPressed
				 << "] was triggered by keyboard";
		break;

	case Qt::Key_F8:
		m_pController->DoAction(Ipponboard::eAction_Shido,
								Ipponboard::eFighter1,
								isCtrlPressed);
		qDebug() << "Action [ Shido for fighter1, revoke="
				 << isCtrlPressed
				 << "] was triggered by keyboard";
		break;

	case Qt::Key_F9:
		m_pController->DoAction(Ipponboard::eAction_Ippon,
								Ipponboard::eFighter2,
								isCtrlPressed);
		qDebug() << "Action [ Ippon for fighter2, revoke="
				 << isCtrlPressed
				 << "] was triggered by keyboard";
		break;

	case Qt::Key_F10:
		m_pController->DoAction(Ipponboard::eAction_Wazaari,
								Ipponboard::eFighter2,
								isCtrlPressed);
		qDebug() << "Action [ Wazaari for fighter2, revoke="
				 << isCtrlPressed
				 << "] was triggered by keyboard";
		break;

	case Qt::Key_F11:
		m_pController->DoAction(Ipponboard::eAction_Yuko,
								Ipponboard::eFighter2,
								isCtrlPressed);
		qDebug() << "Action [ Yuko for fighter2, revoke="
				 << isCtrlPressed
				 << "] was triggered by keyboard";
		break;

	case Qt::Key_F12:
		m_pController->DoAction(Ipponboard::eAction_Shido,
								Ipponboard::eFighter2,
								isCtrlPressed);
		qDebug() << "Action [ Shido for fighter2, revoke="
				 << isCtrlPressed
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
		tr("<h3>%1 v%2</h3>"
		   "<p>Build: %3, Revision: %4</p>"
		   "<p><a href=\"http://www.ipponboard.info\">www.ipponboard.info</a></p>"
		   "<p>&copy; 2010-2013 Florian M&uuml;cke. All rights reserved.</p>"
		   "<p>Some icons by <a href=\"http://p.yusukekamiyamane.com/\">Yusuke Kamiyamane</a>. All rights reserved.</p>"
		   "<p>This program is provided AS IS with NO WARRANTY OF ANY KIND, "
		   "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A "
		   "PARTICULAR PURPOSE.</p>"
		  ).arg(QCoreApplication::applicationName(),
				QCoreApplication::applicationVersion(),
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
	settings.setValue(str_tag_AutoIncrementPoints, m_pController->GetOption(eOption_AutoIncrementPoints));
	settings.setValue(str_tag_Use2013Rules, m_pController->GetOption(eOption_Use2013Rules));
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
	settings.setValue(str_tag_TextColorFirst, m_pPrimaryView->GetTextColorFirst());
	settings.setValue(str_tag_TextBgColorFirst, m_pPrimaryView->GetTextBgColorFirst());
	settings.setValue(str_tag_TextColorSecond, m_pPrimaryView->GetTextColorSecond());
	settings.setValue(str_tag_TextBgColorSecond, m_pPrimaryView->GetTextBgColorSecond());
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
	settings.setValue(str_tag_buttonResetHoldFirst, m_controllerCfg.button_reset_hold_first);
	settings.setValue(str_tag_buttonResetHoldSecond, m_controllerCfg.button_reset_hold_second);
	settings.setValue(str_tag_buttonFirstHolding, m_controllerCfg.button_osaekomi_toketa_first);
	settings.setValue(str_tag_buttonSecondHolding, m_controllerCfg.button_osaekomi_toketa_second);
	settings.setValue(str_tag_buttonHansokumakeFirst, m_controllerCfg.button_hansokumake_first);
	settings.setValue(str_tag_buttonHansokumakeSecond, m_controllerCfg.button_hansokumake_second);
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
	m_MatLabel = settings.value(str_tag_MatLabel, "  www.ipponboard.info   ").toString(); // value is also in settings dialog!
	m_pPrimaryView->SetMat(m_MatLabel);
	m_pSecondaryView->SetMat(m_MatLabel);

	// rules
	m_pController->SetOption(eOption_AutoIncrementPoints,
							 settings.value(str_tag_AutoIncrementPoints, true).toBool());

	m_pController->SetOption(eOption_Use2013Rules,
							 settings.value(str_tag_Use2013Rules, false).toBool());
	update_statebar();

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

	fgColor = m_pSecondaryView->GetTextColorFirst();
	bgColor = m_pSecondaryView->GetTextBgColorFirst();

	if (settings.contains(str_tag_TextColorFirst))
		fgColor = settings.value(str_tag_TextColorFirst).value<QColor>();

	if (settings.contains(str_tag_TextBgColorFirst))
		bgColor = settings.value(str_tag_TextBgColorFirst).value<QColor>();

	update_text_color_first(fgColor, bgColor);

	fgColor = m_pPrimaryView->GetTextColorSecond();
	bgColor = m_pSecondaryView->GetTextBgColorSecond();

	if (settings.contains(str_tag_TextColorSecond))
		fgColor = settings.value(str_tag_TextColorSecond).value<QColor>();

	if (settings.contains(str_tag_TextBgColorSecond))
		bgColor = settings.value(str_tag_TextBgColorSecond).value<QColor>();

	update_text_color_second(fgColor, bgColor);

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
		m_pUi->frame_primary_view->setStyleSheet(styleSheet);
		m_pSecondaryView->setStyleSheet(styleSheet);
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

	m_controllerCfg.button_reset_hold_first =
		settings.value(str_tag_buttonResetHoldFirst, Gamepad::eButton6).toInt();

	m_controllerCfg.button_reset_hold_second =
		settings.value(str_tag_buttonResetHoldSecond, Gamepad::eButton8).toInt();

	m_controllerCfg.button_osaekomi_toketa_first =
		settings.value(str_tag_buttonFirstHolding, Gamepad::eButton5).toInt();

	m_controllerCfg.button_osaekomi_toketa_second =
		settings.value(str_tag_buttonSecondHolding, Gamepad::eButton7).toInt();

	m_controllerCfg.button_hansokumake_first =
		settings.value(str_tag_buttonHansokumakeFirst, Gamepad::eButton11).toInt();

	m_controllerCfg.button_hansokumake_second =
		settings.value(str_tag_buttonHansokumakeSecond, Gamepad::eButton12).toInt();

	m_controllerCfg.axis_inverted_X = settings.value(str_tag_invertX, false).toBool();
	m_controllerCfg.axis_inverted_Y = settings.value(str_tag_invertY, true).toBool();
	m_controllerCfg.axis_inverted_R = settings.value(str_tag_invertR, true).toBool();
	m_controllerCfg.axis_inverted_Z = settings.value(str_tag_invertZ, true).toBool();
	// apply settings to gamepad controller
	m_pGamepad->SetInverted(FMlib::Gamepad::eAxis_X, m_controllerCfg.axis_inverted_X);
	m_pGamepad->SetInverted(FMlib::Gamepad::eAxis_Y, m_controllerCfg.axis_inverted_Y);
	m_pGamepad->SetInverted(FMlib::Gamepad::eAxis_R, m_controllerCfg.axis_inverted_R);
	m_pGamepad->SetInverted(FMlib::Gamepad::eAxis_Z, m_controllerCfg.axis_inverted_Z);
	settings.endGroup();

	settings.beginGroup(str_tag_Sounds);
	m_pController->SetGongFile(settings.value(str_tag_sound_time_ends,
							   "sounds/buzzer1.wav").toString());
	settings.endGroup();

	// update views
	update_views();
}

void MainWindowBase::load_fighters()
{
	QString csvFile(
		QString::fromStdString(
			fmu::GetSettingsFilePath(GetFighterFileName().toAscii())));

	QString errorMsg;

	if (!m_fighterManager.ImportFighters(csvFile, FighterManager::DefaultExportFormat(), errorMsg))
	{
		QMessageBox::critical(
			this,
			QCoreApplication::applicationName(),
			errorMsg);
	}
}

void MainWindowBase::save_fighters()
{
	QString csvFile(
		QString::fromStdString(
			fmu::GetSettingsFilePath(GetFighterFileName().toAscii())));
	QString errorMsg;

	if (!m_fighterManager.ExportFighters(csvFile, FighterManager::DefaultExportFormat(), errorMsg))
	{
		QMessageBox::critical(
			this,
			QCoreApplication::applicationName(),
			errorMsg);
	}
}

void MainWindowBase::update_views()
{
    //m_pPrimaryView->UpdateView();
    //m_pSecondaryView->UpdateView();
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

	dlg.SetTextColorsFirst(m_pPrimaryView->GetTextColorFirst(),
						   m_pPrimaryView->GetTextBgColorFirst());

	dlg.SetTextColorsSecond(m_pPrimaryView->GetTextColorSecond(),
							m_pPrimaryView->GetTextBgColorSecond());

	dlg.SetScreensSettings(m_bAlwaysShow, m_secondScreenNo, m_bAutoSize,
						   m_secondScreenSize);

	dlg.SetRules(m_pController->GetOption(eOption_AutoIncrementPoints),
				 m_pController->GetOption(eOption_Use2013Rules));

	dlg.SetControllerConfig(&m_controllerCfg);

	dlg.SetLabels(m_MatLabel,
				  m_pController->GetHomeLabel(),
				  m_pController->GetGuestLabel());

	dlg.SetGongFile(m_pController->GetGongFile());

	if (QDialog::Accepted == dlg.exec())
	{
		m_pPrimaryView->SetInfoHeaderFont(dlg.GetInfoHeaderFont());
		m_pSecondaryView->SetInfoHeaderFont(dlg.GetInfoHeaderFont());
		update_fighter_name_font(dlg.GetFighterNameFont());
		update_info_text_color(dlg.GetInfoTextColor(), dlg.GetInfoTextBgColor());
		update_text_color_first(dlg.GetTextColorFirst(), dlg.GetTextBgColorFirst());
		update_text_color_second(dlg.GetTextColorSecond(), dlg.GetTextBgColorSecond());

		m_bAlwaysShow = dlg.IsShowAlways();
		m_secondScreenNo = dlg.GetSelectedScreen();
		m_bAutoSize = dlg.IsAutoSize();
		m_secondScreenSize = dlg.GetSize();

		// rules
		m_pController->SetOption(eOption_AutoIncrementPoints, dlg.IsAutoIncrementRule());
		m_pController->SetOption(eOption_Use2013Rules, dlg.IsUse2013Rules());

		dlg.GetControllerConfig(&m_controllerCfg);
		// apply settings to gamepad
		m_pGamepad->SetInverted(FMlib::Gamepad::eAxis_X, m_controllerCfg.axis_inverted_X);
		m_pGamepad->SetInverted(FMlib::Gamepad::eAxis_Y, m_controllerCfg.axis_inverted_Y);
		m_pGamepad->SetInverted(FMlib::Gamepad::eAxis_R, m_controllerCfg.axis_inverted_R);
		m_pGamepad->SetInverted(FMlib::Gamepad::eAxis_Z, m_controllerCfg.axis_inverted_Z);

		m_MatLabel = dlg.GetMatLabel();
		m_pController->SetLabels(dlg.GetHomeLabel(), dlg.GetGuestLabel());

		m_pPrimaryView->SetMat(m_MatLabel);
		m_pSecondaryView->SetMat(m_MatLabel);
		//m_pPrimaryView->UpdateView();
		//m_pSecondaryView->UpdateView();
		m_pController->SetGongFile(dlg.GetGongFile());

		// save changes to file
		write_settings();

		update_statebar();
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
	if (Gamepad::eState_ok != m_pGamepad->GetState())
		return;

	m_pGamepad->ReadData();

	if (EvaluateSpecificInput(m_pGamepad.get()))
		return;

	if (m_pGamepad->WasPressed(Gamepad::EButton(m_controllerCfg.button_hajime_mate)))
	{
		m_pController->DoAction(eAction_Hajime_Mate, eFighterNobody);
	}
	else if (m_pGamepad->WasPressed(Gamepad::EButton(m_controllerCfg.button_reset_hold_first)))
	{
		m_pController->DoAction(eAction_ResetOsaeKomi, eFighter1, true);
	}
	else if (m_pGamepad->WasPressed(Gamepad::EButton(m_controllerCfg.button_reset_hold_second)))
	{
		m_pController->DoAction(eAction_ResetOsaeKomi, eFighter2, true);
	}
	else if (m_pGamepad->WasPressed(Gamepad::EButton(m_controllerCfg.button_osaekomi_toketa_first)))
	{
		if (eState_Holding == m_pController->GetCurrentState() &&
				eFighter1 != m_pController->GetLead())
		{
			m_pController->DoAction(eAction_SetOsaekomi, eFighter1);
		}
		else
		{
			m_pController->DoAction(eAction_OsaeKomi_Toketa, eFighter1);
		}
	}
	else if (m_pGamepad->WasPressed(Gamepad::EButton(m_controllerCfg.button_osaekomi_toketa_second)))
	{
		if (eState_Holding == m_pController->GetCurrentState() &&
				eFighter2 != m_pController->GetLead())
		{
			m_pController->DoAction(eAction_SetOsaekomi, eFighter2);
		}
		else
		{
			m_pController->DoAction(eAction_OsaeKomi_Toketa, eFighter2);
		}
	}
	// reset
	else if (
		m_pGamepad->IsPressed(Gamepad::EButton(m_controllerCfg.button_reset)) &&
		m_pGamepad->IsPressed(Gamepad::EButton(m_controllerCfg.button_reset_2)))
	{
		m_pController->DoAction(eAction_ResetAll, eFighterNobody);
	}

	// hansokumake fighter1
	else if (m_pGamepad->WasPressed(Gamepad::EButton(m_controllerCfg.button_hansokumake_first)))
	{
		const bool revoke(m_pController->GetScore(
							  eFighter1, ePoint_Hansokumake) != 0);
		m_pController->DoAction(eAction_Hansokumake, eFighter1, revoke);
	}
	// hansokumake fighter2
	else if (m_pGamepad->WasPressed(Gamepad::EButton(m_controllerCfg.button_hansokumake_second)))
	{
		const bool revoke(m_pController->GetScore(
							  eFighter2, ePoint_Hansokumake) != 0);
		m_pController->DoAction(eAction_Hansokumake, eFighter2, revoke);

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

		if (m_pGamepad->WasSectionEnteredXY(sections[0][0], sections[0][1]))
		{
			m_pController->DoAction(eAction_Ippon, eFighter1);
		}
		else if (m_pGamepad->WasSectionEnteredXY(sections[1][0], sections[1][1]))
		{
			m_pController->DoAction(eAction_Wazaari, eFighter1);
		}
		else if (m_pGamepad->WasSectionEnteredXY(sections[2][0], sections[2][1]))
		{
			m_pController->DoAction(eAction_Yuko, eFighter1);
		}
		else if (m_pGamepad->WasSectionEnteredXY(sections[3][0], sections[3][1]))
		{
			m_pController->DoAction(eAction_Shido, eFighter1, true);
		}
		else if (m_pGamepad->WasSectionEnteredXY(sections[4][0], sections[4][1]))
		{
			m_pController->DoAction(eAction_Ippon, eFighter1, true);
		}
		else if (m_pGamepad->WasSectionEnteredXY(sections[5][0], sections[5][1]))
		{
			m_pController->DoAction(eAction_Wazaari, eFighter1, true);
		}
		else if (m_pGamepad->WasSectionEnteredXY(sections[6][0], sections[6][1]))
		{
			m_pController->DoAction(eAction_Yuko, eFighter1, true);
		}
		else if (m_pGamepad->WasSectionEnteredXY(sections[7][0], sections[7][1]))
		{
			m_pController->DoAction(eAction_Shido, eFighter1);
		}

		// evaluate fighter2 actions
		else if (m_pGamepad->WasSectionEnteredRZ(sections[0][0], sections[0][1]))
		{
			m_pController->DoAction(eAction_Ippon, eFighter2);
		}
		else if (m_pGamepad->WasSectionEnteredRZ(sections[1][0], sections[1][1]))
		{
			m_pController->DoAction(eAction_Wazaari, eFighter2);
		}
		else if (m_pGamepad->WasSectionEnteredRZ(sections[2][0], sections[2][1]))
		{
			m_pController->DoAction(eAction_Yuko, eFighter2);
		}
		else if (m_pGamepad->WasSectionEnteredRZ(sections[3][0], sections[3][1]))
		{
			m_pController->DoAction(eAction_Shido, eFighter2, true);
		}
		else if (m_pGamepad->WasSectionEnteredRZ(sections[4][0], sections[4][1]))
		{
			m_pController->DoAction(eAction_Ippon, eFighter2, true);
		}
		else if (m_pGamepad->WasSectionEnteredRZ(sections[5][0], sections[5][1]))
		{
			m_pController->DoAction(eAction_Wazaari, eFighter2, true);
		}
		else if (m_pGamepad->WasSectionEnteredRZ(sections[6][0], sections[6][1]))
		{
			m_pController->DoAction(eAction_Yuko, eFighter2, true);
		}
		else if (m_pGamepad->WasSectionEnteredRZ(sections[7][0], sections[7][1]))
		{
			m_pController->DoAction(eAction_Shido, eFighter2);
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
	m_pController->DoAction(Ipponboard::eAction_ResetAll,
							Ipponboard::eFighterNobody,
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

void MainWindowBase::update_statebar()
{
	qDebug() << "virtual function not implemented: " << __FUNCTION__;
}

void MainWindowBase::write_specific_settings(QSettings&)
{
	qDebug() << "virtual function not implemented: " << __FUNCTION__;
}

void MainWindowBase::read_specific_settings(QSettings&)
{
	qDebug() << "virtual function not implemented: " << __FUNCTION__;
}
