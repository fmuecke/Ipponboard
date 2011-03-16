#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "../base/clubmanagerdlg.h"
#include "../base/fightcategorymanagerdlg.h"
#include "../base/view.h"
#include "../base/controller.h"
#include "../base/clubmanager.h"
#include "../base/fightcategorymanager.h"
#include "../base/versioninfo.h"
#include "../base/tournamentmodel.h"
#include "../widgets/scaledimage.h"
#include "../widgets/scaledtext.h"
#ifdef TEAM_VIEW
  #include "scorescreen.h"
  #include "../base/tournament.h"
#endif
#include "../gamepad/gamepad.h"
#include "../base/controlconfig.h"
#include "../base/settingsdlg.h"
#include "../util/helpers.h"

#include <QComboBox>
#include <QMessageBox>
#include <QDesktopWidget>
#include <QFontDialog>
#include <QColorDialog>
#include <QInputDialog>
#include <QSettings>
#include <QTimer>
#include <QSplashScreen>
#include <functional>
#include <QUrl>
#include <QDesktopServices>

using namespace FMlib;
using namespace Ipponboard;

//=========================================================
MainWindow::MainWindow(QWidget *parent)
	: QMainWindow(parent)
	, m_pUi(new Ui::MainWindow)
	, m_pPrimaryView(nullptr)
	, m_pSecondaryView(nullptr)
	, m_pController(nullptr)
#ifdef TEAM_VIEW
	, m_pScoreScreen(nullptr)
	, m_pClubManager(nullptr)
	, fighters_home()
	, fighters_guest()
#else
	, m_pCategoryManager(nullptr)
#endif
	, m_pGamePad(new Gamepad)
	, m_FighterNameFont("Calibri", 12, QFont::Bold, false)
	, m_secondScreenNo(0)
	, m_bAlwaysShow(true)
	, m_bAutoSize(true)
	, m_secondScreenSize()
	, m_Language("en")
	, m_controlCfg()
//=========================================================
{
	m_pUi->setupUi(this);

	//
	// setup controller
	//
	m_pController = new Ipponboard::Controller();
#ifdef TEAM_VIEW
	m_pClubManager = new Ipponboard::ClubManager();
#else
	m_pCategoryManager = new FightCategoryMgr();
#endif

	//
	// setup view(s)
	//
	m_pPrimaryView = new Ipponboard::View(
			m_pController, Ipponboard::View::eTypePrimary );
	m_pUi->verticalLayout_3->insertWidget( 0, m_pPrimaryView, 0 );
	m_pSecondaryView = new Ipponboard::View(
			m_pController, Ipponboard::View::eTypeSecondary );
#ifdef TEAM_VIEW
	m_pScoreScreen = new Ipponboard::ScoreScreen();
#endif

	// set default background
	QString styleSheet("background-color:black; color:white");
	m_pUi->frame_primary_view->setStyleSheet(styleSheet);
	m_pSecondaryView->setStyleSheet(styleSheet);
#ifdef TEAM_VIEW
	m_pScoreScreen->setStyleSheet(styleSheet);
#endif

	//
	// setup data
	//
	m_pController->ClearFights();

#ifdef TEAM_VIEW
	update_club_views();

	m_pUi->tableView_tournament_list1->setModel(m_pController->GetTournamentScoreModel(0));
	m_pUi->tableView_tournament_list2->setModel(m_pController->GetTournamentScoreModel(1));
	m_pUi->tableView_tournament_list1->resizeColumnsToContents();
	m_pUi->tableView_tournament_list2->resizeColumnsToContents();
	m_pController->GetTournamentScoreModel(0)->SetExternalDisplays(
			m_pUi->lineEdit_wins_intermediate,
			m_pUi->lineEdit_score_intermediate );
	m_pController->GetTournamentScoreModel(1)->SetExternalDisplays(
			m_pUi->lineEdit_wins,
			m_pUi->lineEdit_score );
	m_pController->GetTournamentScoreModel(1)->SetIntermediateModel(
			m_pController->GetTournamentScoreModel(0));

	int index = m_pUi->comboBox_club_home->findText("TSV Königsbrunn");
	m_pUi->comboBox_club_home->setCurrentIndex(index);
	m_pUi->comboBox_club_guest->setCurrentIndex(index+1);
	//m_pUi->tableView_tournament_list1->setSpan(2,2,1,2);

	m_pUi->tableView_tournament_list1->selectRow(0);
	m_pUi->tableView_tournament_list2->selectRow(0);
#endif

	//
	// load stored settings
	//
	read_settings();
	if( m_bAlwaysShow )
	{
		m_pUi->actionShow_SecondaryView->setChecked(true);
		on_actionShow_SecondaryView_triggered();
	}
	change_lang(true);

#ifdef TEAM_VIEW
	m_pUi->dateEdit->setDate(QDate::currentDate());
	//m_pUi->comboBox_mode->addItem(str_mode_bundesliga);
	//m_pUi->comboBox_mode->addItem(str_mode_bayernliga);
	//const int modeIndex = m_pUi->comboBox_mode->findText(str_mode_bundesliga);
	//m_pUi->comboBox_mode->setCurrentIndex(modeIndex);
	m_pUi->comboBox_mode->hide();
	m_pUi->label_mode->hide();

	update_weights("-66;-73;-81;-90;+90");
#else
	// init tournament classes (if there are none present)
	for(int i(0); i<m_pCategoryManager->CategoryCount(); ++i)
	{
		FightCategory t("");
		m_pCategoryManager->GetCategory(i, t);
		m_pUi->comboBox_weight_class->addItem(t.ToString());
	}

	// round times
	m_pUi->comboBox_time->addItem(str_normal_round_time);
	m_pUi->comboBox_time->addItem(str_golden_score);


	// trigger tournament class combobox update
	on_comboBox_weight_class_currentIndexChanged(
			m_pUi->comboBox_weight_class->currentText());

	m_pUi->lineEdit_name_blue->setText(tr("Blue"));
	m_pUi->lineEdit_name_white->setText(tr("White"));

#endif

	//
	// init gamepad
	//
	QTimer* m_pTimer = new QTimer;
	connect( m_pTimer, SIGNAL(timeout()), this, SLOT(EvaluateInput()) );
	m_pTimer->start(75);

#ifdef TEAM_VIEW
	UpdateFightNumber_();

	//m_pUi->button_pause->click();	// we start with pause!
#endif
}

//=========================================================
MainWindow::~MainWindow()
//=========================================================
{
	delete m_pSecondaryView;
	delete m_pPrimaryView;
	delete m_pController;
#ifdef TEAM_VIEW
	delete m_pScoreScreen;
	delete m_pClubManager;
#else
	delete m_pCategoryManager;
#endif
	delete m_pUi;
}

//=========================================================
void MainWindow::changeEvent(QEvent *e)
//=========================================================
{
	QMainWindow::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		m_pUi->retranslateUi(this);
		break;
	default:
		break;
	}
}

//=========================================================
void MainWindow::closeEvent(QCloseEvent* event)
//=========================================================
{
	write_settings();

#ifdef TEAM_VIEW
	if( m_pScoreScreen )
	{
		m_pScoreScreen->close();
	}
#endif
	if( m_pSecondaryView )
	{
		m_pSecondaryView->close();
	}
	event->accept();
}

//=========================================================
void MainWindow::write_settings()
//=========================================================
{
	QString iniFile(
			QString::fromStdString(
					FMU::GetSettingsFilePath(str_ini_name)));

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

	settings.beginGroup(str_tag_Fonts);
	settings.setValue(str_tag_TextFont1, m_pPrimaryView->GetInfoHeaderFont().toString() );
	settings.setValue(str_tag_FighterNameFont, m_FighterNameFont.toString() );
	settings.setValue(str_tag_DigitFont, m_pPrimaryView->GetDigitFont().toString() );
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

	settings.beginGroup( str_tag_Input );
	settings.setValue( str_tag_buttonHajimeMate, m_controlCfg.button_hajime_mate );
	settings.setValue( str_tag_buttonNext, m_controlCfg.button_next );
	settings.setValue( str_tag_buttonPrev, m_controlCfg.button_prev );
	settings.setValue( str_tag_buttonPause, m_controlCfg.button_pause );
	settings.setValue( str_tag_buttonReset, m_controlCfg.button_reset );
	settings.setValue( str_tag_buttonReset2, m_controlCfg.button_reset_2 );
	settings.setValue( str_tag_buttonResetHoldBlue, m_controlCfg.button_reset_hold_blue );
	settings.setValue( str_tag_buttonResetHoldWhite, m_controlCfg.button_reset_hold_white );
	settings.setValue( str_tag_buttonBlueHolding, m_controlCfg.button_osaekomi_toketa_blue );
	settings.setValue( str_tag_buttonWhiteHolding, m_controlCfg.button_osaekomi_toketa_white );
	settings.setValue( str_tag_buttonHansokumakeBlue, m_controlCfg.button_hansokumake_blue );
	settings.setValue( str_tag_buttonHansokumakeWhite, m_controlCfg.button_hansokumake_white );
	settings.setValue( str_tag_invertX, m_controlCfg.axis_inverted_X );
	settings.setValue( str_tag_invertY, m_controlCfg.axis_inverted_Y );
	settings.setValue( str_tag_invertR, m_controlCfg.axis_inverted_R );
	settings.setValue( str_tag_invertZ, m_controlCfg.axis_inverted_Z );
	settings.endGroup();

	settings.beginGroup( str_tag_Sounds );
	settings.setValue( str_tag_sound_time_ends, m_pController->GetGongFile() );
	settings.endGroup();
}

//=========================================================
void MainWindow::read_settings()
//=========================================================
{
	QString iniFile(
			QString::fromStdString(
					FMU::GetSettingsFilePath(str_ini_name)));

	QSettings settings(iniFile, QSettings::IniFormat, this);

	//
	// MainWindow
	//
	settings.beginGroup(str_tag_Main);
	m_Language = settings.value(str_tag_Language,"en").toString();
	//resize(settings.value(str_tag_size, size()).toSize());
	move(settings.value(str_tag_pos, QPoint(200, 200)).toPoint());
	m_secondScreenNo = settings.value(str_tag_SecondScreen, 0).toInt();
	m_secondScreenSize = settings.value(str_tag_SecondScreenSize,
										QSize(1024,768)).toSize();
	m_bAutoSize = settings.value(str_tag_AutoSize, true).toBool();
	m_bAlwaysShow = settings.value(str_tag_AlwaysShow, true).toBool();
	m_MatLabel = settings.value(str_tag_MatLabel,"    Ipponboard    ").toString();
	m_pPrimaryView->SetMat(m_MatLabel);
	m_pSecondaryView->SetMat(m_MatLabel);
	settings.endGroup();

	//
	// Fonts
	//
	settings.beginGroup(str_tag_Fonts);
	QFont font = m_pPrimaryView->GetInfoHeaderFont();
	font.fromString( settings.value(str_tag_TextFont1, font.toString()).toString() );
	m_pPrimaryView->SetInfoHeaderFont(m_pPrimaryView->GetInfoHeaderFont());
	m_pSecondaryView->SetInfoHeaderFont(m_pPrimaryView->GetInfoHeaderFont());

	font = m_pPrimaryView->GetFighterNameFont();
		font.fromString( settings.value(str_tag_FighterNameFont, font.toString()).toString() );
	update_fighter_name_font(font);

	font = m_pPrimaryView->GetDigitFont();
	font.fromString( settings.value(str_tag_DigitFont, font.toString()).toString() );
	m_pPrimaryView->SetDigitFont(font);
	m_pSecondaryView->SetDigitFont(font);
	//m_pScoreScreen->SetDigitFont(font);
	settings.endGroup();

	//
	// Colors
	//
	settings.beginGroup( str_tag_Colors );
	QColor fgColor = m_pSecondaryView->GetInfoTextColor();
	QColor bgColor = m_pSecondaryView->GetInfoTextBgColor();
	if( settings.contains(str_tag_InfoTextColor) )
		fgColor = settings.value(str_tag_InfoTextColor).value<QColor>();
	if( settings.contains(str_tag_InfoTextBgColor) )
		bgColor = settings.value(str_tag_InfoTextBgColor).value<QColor>();
	update_info_text_color(fgColor, bgColor);

	fgColor = m_pSecondaryView->GetTextColorBlue();
	bgColor = m_pSecondaryView->GetTextBgColorBlue();
	if( settings.contains(str_tag_TextColorBlue) )
		fgColor = settings.value(str_tag_TextColorBlue).value<QColor>();
	if( settings.contains(str_tag_TextBgColorBlue) )
		bgColor = settings.value(str_tag_TextBgColorBlue).value<QColor>();
	update_text_color_blue(fgColor, bgColor);

	fgColor = m_pPrimaryView->GetTextColorWhite();
	bgColor = m_pSecondaryView->GetTextBgColorWhite();
	if( settings.contains(str_tag_TextColorWhite) )
		fgColor = settings.value(str_tag_TextColorWhite).value<QColor>();
	if( settings.contains(str_tag_TextBgColorWhite) )
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
	settings.beginGroup( str_tag_Styles );
	if( settings.contains( str_tag_BgStyle ) )
	{
		const QString styleSheet = settings.value(str_tag_BgStyle).toString();
		m_pUi->frame_primary_view->setStyleSheet( styleSheet );
		m_pSecondaryView->setStyleSheet( styleSheet );
#ifdef TEAM_VIEW
		m_pScoreScreen->setStyleSheet( styleSheet );
#endif
	}
	settings.endGroup();

	settings.beginGroup( str_tag_Input );

	m_controlCfg.button_hajime_mate =
			settings.value( str_tag_buttonHajimeMate, Gamepad::eButton_pov_back ).toInt();

	m_controlCfg.button_next =
			settings.value( str_tag_buttonNext, Gamepad::eButton10 ).toInt();

	m_controlCfg.button_prev =
			settings.value( str_tag_buttonPrev, Gamepad::eButton9 ).toInt();

	m_controlCfg.button_pause =
			settings.value( str_tag_buttonPause, Gamepad::eButton2 ).toInt();

	m_controlCfg.button_reset =
			settings.value( str_tag_buttonReset, Gamepad::eButton1 ).toInt();

	m_controlCfg.button_reset_2 =
			settings.value( str_tag_buttonReset2, Gamepad::eButton4 ).toInt();

	m_controlCfg.button_reset_hold_blue =
			settings.value( str_tag_buttonResetHoldBlue, Gamepad::eButton6 ).toInt();

	m_controlCfg.button_reset_hold_white =
			settings.value( str_tag_buttonResetHoldWhite, Gamepad::eButton8 ).toInt();

	m_controlCfg.button_osaekomi_toketa_blue =
			settings.value( str_tag_buttonBlueHolding, Gamepad::eButton5 ).toInt();

	m_controlCfg.button_osaekomi_toketa_white =
			settings.value( str_tag_buttonWhiteHolding, Gamepad::eButton7 ).toInt();

	m_controlCfg.button_hansokumake_blue =
			settings.value( str_tag_buttonHansokumakeBlue, Gamepad::eButton11 ).toInt();

	m_controlCfg.button_hansokumake_white =
			settings.value( str_tag_buttonHansokumakeWhite, Gamepad::eButton12 ).toInt();

	m_controlCfg.axis_inverted_X = settings.value( str_tag_invertX, false ).toBool();
	m_controlCfg.axis_inverted_Y = settings.value( str_tag_invertY, true ).toBool();
	m_controlCfg.axis_inverted_R = settings.value( str_tag_invertR, true ).toBool();
	m_controlCfg.axis_inverted_Z = settings.value( str_tag_invertZ, true ).toBool();
	// apply settings to gamepad controller
	m_pGamePad->SetInverted( FMlib::Gamepad::eAxis_X, m_controlCfg.axis_inverted_X );
	m_pGamePad->SetInverted( FMlib::Gamepad::eAxis_Y, m_controlCfg.axis_inverted_Y );
	m_pGamePad->SetInverted( FMlib::Gamepad::eAxis_R, m_controlCfg.axis_inverted_R );
	m_pGamePad->SetInverted( FMlib::Gamepad::eAxis_Z, m_controlCfg.axis_inverted_Z );
	settings.endGroup();

	settings.beginGroup( str_tag_Sounds );
	m_pController->SetGongFile(settings.value( str_tag_sound_time_ends,
									  "sounds/buzzer1.wav" ).toString());
	settings.endGroup();

	// update views
	m_pPrimaryView->UpdateView();
	m_pSecondaryView->UpdateView();
#ifdef TEAM_VIEW
	m_pScoreScreen->update();
#endif
}

//=========================================================
void MainWindow::update_info_text_color(const QColor& color, const QColor& bgColor)
//=========================================================
{
	m_pPrimaryView->SetInfoTextColor(color, bgColor);
	m_pSecondaryView->SetInfoTextColor(color, bgColor);
#ifdef TEAM_VIEW
	//m_pScoreScreen->SetInfoTextColor(color, bgColor);
#endif

}

//=========================================================
void MainWindow::update_text_color_blue(const QColor& color, const QColor& bgColor)
//=========================================================
{
	m_pPrimaryView->SetTextColorBlue(color, bgColor);
	m_pSecondaryView->SetTextColorBlue(color, bgColor);
#ifdef TEAM_VIEW
	m_pScoreScreen->SetTextColorBlue(color, bgColor);
#endif
}

//=========================================================
void MainWindow::update_text_color_white(const QColor& color, const QColor& bgColor)
//=========================================================
{
	m_pPrimaryView->SetTextColorWhite(color, bgColor);
	m_pSecondaryView->SetTextColorWhite(color, bgColor);
#ifdef TEAM_VIEW
	m_pScoreScreen->SetTextColorWhite(color, bgColor);
#endif
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void MainWindow::update_fighter_name_font(const QFont & font)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{
	m_FighterNameFont = font;
	m_pPrimaryView->SetFighterNameFont(font);
	m_pSecondaryView->SetFighterNameFont(font);
#ifdef TEAM_VIEW
	m_pScoreScreen->SetTextFont(font);
#endif
}

//=========================================================
void MainWindow::show_hide_view() const
//=========================================================
{
	if( m_pSecondaryView->isHidden() )
	{
		const int nScreens(QApplication::desktop()->numScreens());
		if( nScreens > 0 && nScreens > m_secondScreenNo )
		{
			QRect screenres =
				QApplication::desktop()->screenGeometry(m_secondScreenNo);
			m_pSecondaryView->move(QPoint(screenres.x(), screenres.y()));
		}
		if( m_bAutoSize )
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

//=========================================================
void MainWindow::update_views()
//=========================================================
{
	m_pPrimaryView->UpdateView();
	m_pSecondaryView->UpdateView();
#ifdef TEAM_VIEW
	UpdateScoreScreen_();
#endif
}


#ifdef TEAM_VIEW
//=========================================================
void MainWindow::update_club_views()
//=========================================================
{
	m_pUi->comboBox_club_home->clear();
	m_pUi->comboBox_club_guest->clear();

	for( int i = 0; i < m_pClubManager->ClubCount(); ++i )
	{
		Ipponboard::Club club;
		m_pClubManager->GetClub(i, club);
		QIcon icon(club.logoFile);
		m_pUi->comboBox_club_home->addItem(icon, club.name);
		m_pUi->comboBox_club_guest->addItem(icon, club.name);
	}
}

//=========================================================
void MainWindow::UpdateFightNumber_()
//=========================================================
{
	const int current = m_pController->GetCurrentFightIndex() + 1;
	m_pUi->label_fight->setText( QString::number(current) + " / 10" );
}
//=========================================================
void MainWindow::UpdateScoreScreen_()
//=========================================================
{
	const QString home = m_pUi->comboBox_club_home->currentText();
	const QString guest = m_pUi->comboBox_club_guest->currentText();
	m_pScoreScreen->SetClubs( home, guest );
	const QString logo_home = m_pClubManager->GetLogo(home);
	const QString logo_guest = m_pClubManager->GetLogo(guest);
	m_pScoreScreen->SetLogos( logo_home, logo_guest );
	const int score_blue = m_pController->GetTeamScore(Ipponboard::eFighter_Blue);
	const int score_white = m_pController->GetTeamScore(Ipponboard::eFighter_White);
	m_pScoreScreen->SetScore( score_blue, score_white );
}

//=========================================================
void MainWindow::WriteScoreToHtml_()
//=========================================================
{
	QFile file("templates\\list_output.html");
	if( !file.open(QFile::ReadOnly) )
	{
		QMessageBox::critical( this, tr("File open error"),
			tr("Datei kann nicht geöffnet werden: ") + file.fileName() );
		return;
	}
	QTextStream ts(&file);

	m_htmlScore = ts.readAll();
	file.close();

	m_htmlScore.replace( "%DATE%", m_pUi->dateEdit->text() );
	m_htmlScore.replace( "%LOCATION%", m_pUi->lineEdit_location->text() );
	m_htmlScore.replace( "%HOME%", m_pUi->comboBox_club_home->currentText() );
	m_htmlScore.replace( "%GUEST%", m_pUi->comboBox_club_guest->currentText() );

	// intermediate score
	std::pair<unsigned,unsigned> wins =
		m_pController->GetTournamentScoreModel(0)->GetTotalWins();
	m_htmlScore.replace( "%WINS_HOME%", QString::number(wins.first) );
	m_htmlScore.replace( "%WINS_GUEST%", QString::number(wins.second) );
	std::pair<unsigned,unsigned> score =
		m_pController->GetTournamentScoreModel(0)->GetTotalScore();
	m_htmlScore.replace( "%SCORE_HOME%", QString::number(score.first) );
	m_htmlScore.replace( "%SCORE_GUEST%", QString::number(score.second) );

	// final score
	std::pair<unsigned,unsigned> wins2 =
		m_pController->GetTournamentScoreModel(1)->GetTotalWins();
	m_htmlScore.replace( "%TOTAL_WINS_HOME%", QString::number(wins.first + wins2.first) );
	m_htmlScore.replace( "%TOTAL_WINS_GUEST%", QString::number(wins.second + wins2.second) );
	std::pair<unsigned,unsigned> score2 =
		m_pController->GetTournamentScoreModel(1)->GetTotalScore();
	m_htmlScore.replace( "%TOTAL_SCORE_HOME%", QString::number(score.first + score2.first) );
	m_htmlScore.replace( "%TOTAL_SCORE_GUEST%", QString::number(score.second + score2.second) );

	// first round
	QString rounds;
	for(int i(0); i<Ipponboard::eTournament_FightCount; ++i)
	{
		const Fight& fight(m_pController->GetFight(0, i));

		QString name_blue( fight.fighters[eFighter_Blue].name );
		QString name_white( fight.fighters[eFighter_White].name );
		const Score& score_blue( fight.scores[eFighter_Blue] );
		const Score& score_white( fight.scores[eFighter_White] );

		QString round("<tr>");
		round.append("<td><center>" + QString::number(i+1) + "</center></td>"); // number
		round.append("<td><center>" + fight.weight + "</center></td>"); // weight
		round.append("<td><center>" + name_blue + "</center></td>"); // name
		round.append("<td><center>" + QString::number(score_blue.Yuko()) + "</center></td>"); // Y
		round.append("<td><center>" + QString::number(score_blue.Wazaari()) + "</center></td>"); // W
		round.append("<td><center>" + QString::number(score_blue.Ippon()) + "</center></td>"); // I
		round.append("<td><center>" + QString::number(score_blue.Shido()) + "</center></td>"); // S
		round.append("<td><center>" + QString::number(score_blue.Hansokumake()) + "</center></td>"); // H
		round.append("<td><center>" + QString::number(fight.HasWon(eFighter_Blue)) + "</center></td>"); // won
		round.append("<td><center>" + QString::number(fight.ScorePoints(eFighter_Blue)) + "</center></td>"); // score
		round.append("<td><center>" + name_white + "</center></td>"); // name
		round.append("<td><center>" + QString::number(score_white.Yuko()) + "</center></td>"); // Y
		round.append("<td><center>" + QString::number(score_white.Wazaari()) + "</center></td>"); // W
		round.append("<td><center>" + QString::number(score_white.Ippon()) + "</center></td>"); // I
		round.append("<td><center>" + QString::number(score_white.Shido()) + "</center></td>"); // S
		round.append("<td><center>" + QString::number(score_white.Hansokumake()) + "</center></td>"); // H
		round.append("<td><center>" + QString::number(fight.HasWon(eFighter_White)) + "</center></td>"); // won
		round.append("<td><center>" + QString::number(fight.ScorePoints(eFighter_White)) + "</center></td>"); // score
		round.append("<td><center>" + fight.GetRoundTimeText() + "</center></td>"); // time
		round.append("</tr>\n");
		rounds.append(round);
	}
	m_htmlScore.replace( "%FIRST_ROUND%", rounds );

	// second round
	rounds.clear();
	for(int i(0); i<Ipponboard::eTournament_FightCount; ++i)
	{
		const Fight& fight(m_pController->GetFight(1, i));

		QString name_blue( fight.fighters[eFighter_Blue].name );
		QString name_white( fight.fighters[eFighter_White].name );
		const Score& score_blue( fight.scores[eFighter_Blue] );
		const Score& score_white( fight.scores[eFighter_White] );

		QString round("<tr>");
		round.append("<td><center>" + QString::number(i+1) + "</center></td>"); // number
		round.append("<td><center>" + fight.weight + "</center></td>"); // weight
		round.append("<td><center>" + name_blue + "</center></td>"); // name
		round.append("<td><center>" + QString::number(score_blue.Yuko()) + "</center></td>"); // Y
		round.append("<td><center>" + QString::number(score_blue.Wazaari()) + "</center></td>"); // W
		round.append("<td><center>" + QString::number(score_blue.Ippon()) + "</center></td>"); // I
		round.append("<td><center>" + QString::number(score_blue.Shido()) + "</center></td>"); // S
		round.append("<td><center>" + QString::number(score_blue.Hansokumake()) + "</center></td>"); // H
		round.append("<td><center>" + QString::number(fight.HasWon(eFighter_Blue)) + "</center></td>"); // won
		round.append("<td><center>" + QString::number(fight.ScorePoints(eFighter_Blue)) + "</center></td>"); // score
		round.append("<td><center>" + name_white + "</center></td>"); // name
		round.append("<td><center>" + QString::number(score_white.Yuko()) + "</center></td>"); // Y
		round.append("<td><center>" + QString::number(score_white.Wazaari()) + "</center></td>"); // W
		round.append("<td><center>" + QString::number(score_white.Ippon()) + "</center></td>"); // I
		round.append("<td><center>" + QString::number(score_white.Shido()) + "</center></td>"); // S
		round.append("<td><center>" + QString::number(score_white.Hansokumake()) + "</center></td>"); // H
		round.append("<td><center>" + QString::number(fight.HasWon(eFighter_White)) + "</center></td>"); // won
		round.append("<td><center>" + QString::number(fight.ScorePoints(eFighter_White)) + "</center></td>"); // score
		round.append("<td><center>" + fight.GetRoundTimeText() + "</center></td>"); // time
		round.append("</tr>\n");
		rounds.append(round);
	}
	m_htmlScore.replace( "%SECOND_ROUND%", rounds );

	const QString copyright = tr("List generated with Ipponboard v") +
		QApplication::applicationVersion() +
		", &copy; " + QApplication::organizationName() + ", 2010";
	m_htmlScore.replace( "</body>", "<small><center>" + copyright + "</center></small></body>" );
}
#endif


//=========================================================
void MainWindow::on_actionAbout_Ipponboard_triggered()
//=========================================================
{
	QMessageBox::about(
		this,
		tr("About %1").arg(QCoreApplication::applicationName()),
		tr( "<h3>%1 v%2</h3>"
			"<p>%1 was entirely written in advanced C++ using the Qt toolkit %3.</p>"
			"<p>Revision: %4</p>"
			"<p>Author: Florian M&uuml;cke, <a href=\"http://flo.mueckeimnetz.de\">homepage</a></p>"
			"<p>The %1 project is hosted at <a href=\"http://ipponboard.origo.ethz.ch\">http://ipponboard.origo.ethz.ch</a>.</p>"
			"<p>&copy; 2010 Florian M&uuml;cke. All rights reserved.</p>"
			"<p>This program is provided AS IS with NO WARRANTY OF ANY KIND, "
			"INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A "
			"PARTICULAR PURPOSE.<br/>"
		).arg(	QCoreApplication::applicationName(),
				QCoreApplication::applicationVersion(),
				QLatin1String(QT_VERSION_STR),
				VersionInfo::Revision
				//QLatin1String(__DATE__),
				//QLatin1String(__TIME__)
	));
}

//=========================================================
void MainWindow::on_actionTest_Gong_triggered()
//=========================================================
{
	m_pController->Gong();
}

//=========================================================
void MainWindow::on_actionShow_SecondaryView_triggered()
//=========================================================
{
	show_hide_view();
}


////=========================================================
//void MainWindow::on_actionSelect_Color_triggered()
////=========================================================
//{
//	QColor color = m_pPrimaryView->GetTextColor();
//	color = QColorDialog::getColor(color, this);
//	if( color.isValid() )
//	{
//		m_pPrimaryView->SetTextColor(color);
//		m_pSecondaryView->SetTextColor(color);
//	}
//}
//
////=========================================================
//void MainWindow::on_actionChange_Background_triggered()
////=========================================================
//{
//	bool ok;
//	QString styleSheet = QInputDialog::getText(
//			this,
//			tr("Change Style Sheet"),
//			tr("Style sheet"),
//			QLineEdit::Normal,
//			m_pUi->frame_primary_view->styleSheet(),
//			&ok);
//
//	if (ok && !styleSheet.isEmpty())
//	{
//		m_pUi->frame_primary_view->setStyleSheet(styleSheet);
//		m_pSecondaryView->setStyleSheet(styleSheet);
//		m_pScoreScreen->setStyleSheet(styleSheet);
//	}
//}

//=========================================================
void MainWindow::on_actionReset_Scores_triggered()
//=========================================================
{
	if( QMessageBox::warning(
			this,
			tr("Reset Scores"),
			tr("Really reset complete score table?"),
			QMessageBox::Yes|QMessageBox::No) == QMessageBox::Yes )
		m_pController->ClearFights();
}

//=========================================================
void MainWindow::on_actionPreferences_triggered()
//=========================================================
{
	SettingsDlg dlg(this);
	dlg.SetInfoHeaderSettings( m_pPrimaryView->GetInfoHeaderFont(),
						 m_pPrimaryView->GetInfoTextColor(),
						 m_pPrimaryView->GetInfoTextBgColor() );
	dlg.SetFighterNameFont( m_FighterNameFont );
	dlg.SetTextColorsBlue( m_pPrimaryView->GetTextColorBlue(),
						   m_pPrimaryView->GetTextBgColorBlue() );
	dlg.SetTextColorsWhite( m_pPrimaryView->GetTextColorWhite(),
						   m_pPrimaryView->GetTextBgColorWhite() );
	dlg.SetScreensSettings(m_bAlwaysShow, m_secondScreenNo, m_bAutoSize,
						   m_secondScreenSize);

	dlg.SetControlConfig(&m_controlCfg);
	dlg.SetMatLabel(m_MatLabel);
	dlg.SetGongFile(m_pController->GetGongFile());

	if( QDialog::Accepted == dlg.exec() )
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

		dlg.GetControlConfig(&m_controlCfg);
		// apply settings to gamepad
		m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_X, m_controlCfg.axis_inverted_X);
		m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_Y, m_controlCfg.axis_inverted_Y);
		m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_R, m_controlCfg.axis_inverted_R);
		m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_Z, m_controlCfg.axis_inverted_Z);


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

//=========================================================
void MainWindow::on_button_reset_clicked()
//=========================================================
{
//	QMessageBox::StandardButton answer =
//		QMessageBox::question( this,
//							   tr("Reset"),
//							   tr("Really reset current fight?"),
//							   QMessageBox::No | QMessageBox::Yes );
//	if( QMessageBox::Yes == answer )
		m_pController->DoAction(Ipponboard::eAction_Reset,
								Ipponboard::eFighter_Nobody,
								false);
}

//=========================================================
void MainWindow::EvaluateInput()
//=========================================================
{
	if( Gamepad::eState_ok != m_pGamePad->GetState() )
		return;

	m_pGamePad->ReadData();

	if( m_pGamePad->WasPressed( Gamepad::EButton(m_controlCfg.button_hajime_mate) ) )
	{
		m_pController->DoAction( eAction_Hajime_Mate, eFighter_Nobody );
	}
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_controlCfg.button_reset_hold_blue)) ||
			 m_pGamePad->WasPressed(Gamepad::EButton(m_controlCfg.button_reset_hold_white)) )
	{
		m_pController->DoAction( eAction_ResetOsaeKomi, eFighter_Nobody );
	}
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_controlCfg.button_osaekomi_toketa_blue)) )
	{
		if( eState_Holding == m_pController->GetCurrentState() &&
			eFighter_Blue != m_pController->GetLead() )
		{
			m_pController->DoAction( eAction_SetOsaekomi, eFighter_Blue );
		}
		else
		{
			m_pController->DoAction( eAction_OsaeKomi_Toketa, eFighter_Blue );
		}
	}
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_controlCfg.button_osaekomi_toketa_white)) )
	{
		if( eState_Holding == m_pController->GetCurrentState() &&
			eFighter_White != m_pController->GetLead() )
		{
			m_pController->DoAction( eAction_SetOsaekomi, eFighter_White );
		}
		else
		{
			m_pController->DoAction( eAction_OsaeKomi_Toketa, eFighter_White );
		}
	}
	// reset
	else if(
		m_pGamePad->IsPressed(Gamepad::EButton(m_controlCfg.button_reset)) &&
		m_pGamePad->IsPressed(Gamepad::EButton(m_controlCfg.button_reset_2)) )
	{
		m_pController->DoAction(eAction_Reset, eFighter_Nobody );
	}
	// back
#ifdef TEAM_VIEW
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_controlCfg.button_prev)) )
	{
		on_button_prev_clicked();
		// TODO: check: is UpdateViews_(); necessary here?
		// --> handle update views outside of this function
	}
	// next
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_controlCfg.button_next)) )
	{
		on_button_next_clicked();
		// TODO: check: is UpdateViews_(); necessary here?
		// --> handle update views outside of this function
	}
#endif
	// hansokumake blue
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_controlCfg.button_hansokumake_blue)) )
	{
		const bool revoke(m_pController->GetScore(
			eFighter_Blue, ePoint_Hansokumake) != 0);
		m_pController->DoAction( eAction_Hansokumake, eFighter_Blue, revoke);
	}
	// hansokumake white
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_controlCfg.button_hansokumake_white)) )
	{
		const bool revoke(m_pController->GetScore(
			eFighter_White, ePoint_Hansokumake) != 0);
		m_pController->DoAction( eAction_Hansokumake, eFighter_White, revoke );

	}
	else
	{
		// TODO: don't calc this every time...
		float sections[8][2] = {0};
		float angle = 360.0f;
		float deadSpace = 2.0f;
		float angle_adjustment = 5.0f;
		sections[0][0] = 360-45/2+1 + deadSpace + angle_adjustment;
		sections[0][1] = 45/2+1 - deadSpace + angle_adjustment;
		angle = sections[0][1];
		for(int i=1; i<8; ++i)
		{
			sections[i][0] = angle + 2*deadSpace;
			sections[i][1] = angle + 45;
			angle = sections[i][1];
		}
		if( m_pGamePad->WasSectionEnteredXY(sections[0][0], sections[0][1]) )
		{
			m_pController->DoAction( eAction_Ippon, eFighter_Blue );
		}
		else if( m_pGamePad->WasSectionEnteredXY(sections[1][0], sections[1][1]) )
		{
			m_pController->DoAction( eAction_Wazaari, eFighter_Blue );
		}
		else if( m_pGamePad->WasSectionEnteredXY(sections[2][0], sections[2][1]) )
		{
			m_pController->DoAction( eAction_Yuko, eFighter_Blue );
		}
		else if( m_pGamePad->WasSectionEnteredXY(sections[3][0], sections[3][1]) )
		{
			m_pController->DoAction( eAction_Shido, eFighter_Blue, true );
		}
		else if( m_pGamePad->WasSectionEnteredXY(sections[4][0], sections[4][1]) )
		{
			m_pController->DoAction( eAction_Ippon, eFighter_Blue, true );
		}
		else if( m_pGamePad->WasSectionEnteredXY(sections[5][0], sections[5][1]) )
		{
			m_pController->DoAction( eAction_Wazaari, eFighter_Blue, true );
		}
		else if( m_pGamePad->WasSectionEnteredXY(sections[6][0], sections[6][1]) )
		{
			m_pController->DoAction( eAction_Yuko, eFighter_Blue, true );
		}
		else if( m_pGamePad->WasSectionEnteredXY(sections[7][0], sections[7][1]) )
		{
			m_pController->DoAction( eAction_Shido, eFighter_Blue);
		}

		// evaluate white actions
		else if( m_pGamePad->WasSectionEnteredRZ(sections[0][0], sections[0][1]) )
		{
			m_pController->DoAction( eAction_Ippon, eFighter_White );
		}
		else if( m_pGamePad->WasSectionEnteredRZ(sections[1][0], sections[1][1]) )
		{
			m_pController->DoAction( eAction_Wazaari, eFighter_White );
		}
		else if( m_pGamePad->WasSectionEnteredRZ(sections[2][0], sections[2][1]) )
		{
			m_pController->DoAction( eAction_Yuko, eFighter_White );
		}
		else if( m_pGamePad->WasSectionEnteredRZ(sections[3][0], sections[3][1]) )
		{
			m_pController->DoAction( eAction_Shido, eFighter_White, true );
		}
		else if( m_pGamePad->WasSectionEnteredRZ(sections[4][0], sections[4][1]) )
		{
			m_pController->DoAction( eAction_Ippon, eFighter_White, true );
		}
		else if( m_pGamePad->WasSectionEnteredRZ(sections[5][0], sections[5][1]) )
		{
			m_pController->DoAction( eAction_Wazaari, eFighter_White, true );
		}
		else if( m_pGamePad->WasSectionEnteredRZ(sections[6][0], sections[6][1]) )
		{
			m_pController->DoAction( eAction_Yuko, eFighter_White, true );
		}
		else if( m_pGamePad->WasSectionEnteredRZ(sections[7][0], sections[7][1]) )
		{
			m_pController->DoAction( eAction_Shido, eFighter_White);
		}
	}
}

#ifdef TEAM_VIEW
//=========================================================
void MainWindow::on_tabWidget_currentChanged(int /*index*/)
//=========================================================
{
	update_views();
}

//=========================================================
void MainWindow::on_actionManage_Clubs_triggered()
//=========================================================
{
	ClubManagerDlg dlg(m_pClubManager, this);
	dlg.exec();

	update_club_views();
}

//=========================================================
void MainWindow::on_actionLoad_Demo_Data_triggered()
//=========================================================
{
	m_pController->ClearFights();																				//  Y  W  I  S  H  Y  W  I  S  H
	update_weights("-90;+90;-73;-66;-81");
	m_pController->SetFight( 0, 0, "-90", "Sebastian Hölzl", "TG Landshut", "Oliver Sach", "TSV Königsbrunn",			3, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetFight( 0, 1, "-90", "Stefan Grünert", "TG Landshut", "Marc Schäfer", "TSV Königsbrunn",			3, 2, 0, 0, 0, 0, 0, 0, 1, 0);
	m_pController->SetFight( 0, 2, "+90", "Andreas Neumaier", "TG Landshut", "Daniel Nussbächer", "TSV Königsbrunn",	0, 0, 0, 1, 0, 0, 0, 1, 1, 0);
	m_pController->SetFight( 0, 3, "+90", "Jürgen Neumeier", "TG Landshut", "Anderas Mayer", "TSV Königsbrunn",			1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetFight( 0, 4, "-73", "Benny Mahl", "TG Landshut", "Christopher Benka", "TSV Königsbrunn"	,		2, 0, 1, 1, 0, 0, 0, 0, 3, 0);
	m_pController->SetFight( 0, 5, "-73", "Josef Sellmaier", "TG Landshut", "Jan-Michael König", "TSV Königsbrunn",		0, 1, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetFight( 0, 6, "-66", "Alexander Keil", "TG Landshut", "Arthur Sipple", "TSV Königsbrunn",			2, 1, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetFight( 0, 7, "-66", "Dominic Bogner", "TG Landshut", "Thomas Schaller", "TSV Königsbrunn",		0, 0, 1, 0, 0, 2, 0, 0, 0, 0);
	m_pController->SetFight( 0, 8, "-81", "Sebastian Schmieder", "TG Landshut", "Gerhard Wessinger", "TSV Königsbrunn",	0, 1, 1, 1, 0, 1, 0, 0, 0, 0);
	m_pController->SetFight( 0, 9, "-81", "Rainer Neumaier", "TG Landshut", "Georg Benka", "TSV Königsbrunn",			1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
																												//  Y  W  I  S  H  Y  W  I  S  H
	m_pController->SetFight( 1, 0, "-90", "Sebastian Hölzl", "TG Landshut", "Marc Schäfer", "TSV Königsbrunn",		0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetFight( 1, 1, "-90", "Stefan Grunert", "TG Landshut", "Florian Kürten", "TSV Königsbrunn",		0, 1, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetFight( 1, 2, "+90", "Andreas Neumaier", "TG Landshut", "Andreas Mayer", "TSV Königsbrunn",	1, 2, 0, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetFight( 1, 3, "+90", "Jürgen Neumaier", "TG Landshut", "Daniel Nussbächer", "TSV Königsbrunn",	0, 0, 0, 2, 0, 0, 0, 1, 2, 0);
	m_pController->SetFight( 1, 4, "-73", "Matthias Feigl", "TG Landshut", "Jan-Michael König", "TSV Königsbrunn",	2, 1, 0, 1, 0, 0, 0, 0, 1, 0);
	m_pController->SetFight( 1, 5, "-73", "Josef Sellmaier", "TG Landshut", "Christopher Benka", "TSV Königsbrunn",	0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetFight( 1, 6, "-66", "Jörg Herzog", "TG Landshut", "Thomas Schaller", "TSV Königsbrunn",		0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
	m_pController->SetFight( 1, 7, "-66", "Alex Selwitschka", "TG Landshut", "Jonas Allinger", "TSV Königsbrunn",	0, 1, 1, 0, 0, 1, 0, 0, 0, 0);
	m_pController->SetFight( 1, 8, "-81", "Eugen Makaritsch", "TG Landshut", "Georg Benka", "TSV Königsbrunn",		0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
	m_pController->SetFight( 1, 9, "-81", "Rainer Neumaier", "TG Landshut", "Gerhard Wessinger", "TSV Königsbrunn",	0, 0, 1, 1, 0, 0, 0, 0, 0, 0);
	//m_pController->SetCurrentFight(0);

	m_pUi->tableView_tournament_list1->viewport()->update();
	m_pUi->tableView_tournament_list2->viewport()->update();
}

//=========================================================
void MainWindow::on_button_pause_clicked()
//=========================================================
{
	if( m_pScoreScreen->isVisible() )
	{
		m_pScoreScreen->hide();
	}
	else
	{
		UpdateScoreScreen_();
		const int nScreens(QApplication::desktop()->numScreens());
		if( nScreens > 0 && nScreens > m_secondScreenNo )
		{
			// move to second screen
			QRect screenres =
					QApplication::desktop()->screenGeometry(m_secondScreenNo);
			m_pScoreScreen->move(QPoint(screenres.x(), screenres.y()));
		}
		if( m_bAutoSize )
		{
			m_pScoreScreen->showFullScreen();
		}
		else
		{
			m_pScoreScreen->resize(m_secondScreenSize);
			m_pScoreScreen->show();
		}
	}
}

//=========================================================
void MainWindow::on_button_prev_clicked()
//=========================================================
{
	if( 0 == m_pController->GetCurrentFightIndex() )
		return;

	m_pController->SetCurrentFight( m_pController->GetCurrentFightIndex() - 1 );

	UpdateFightNumber_();
}

//=========================================================
void MainWindow::on_button_next_clicked()
//=========================================================
{
	if( m_pController->GetCurrentFightIndex() == m_pController->GetFightCount() - 1 )
		m_pController->SetCurrentFight( m_pController->GetCurrentFightIndex() );
	else
		m_pController->SetCurrentFight( m_pController->GetCurrentFightIndex() + 1 );

	UpdateFightNumber_();
}

//=========================================================
void MainWindow::on_comboBox_club_home_currentIndexChanged(const QString& s)
//=========================================================
{
	m_pController->SetClub(Ipponboard::eFighter_Blue, s);
	//UpdateViews_(); --> already done by controller
	UpdateScoreScreen_();
}

//=========================================================
void MainWindow::on_comboBox_club_guest_currentIndexChanged(const QString& s)
//=========================================================
{
	m_pController->SetClub(Ipponboard::eFighter_White, s);
	//UpdateViews_(); --> already done by controller
	UpdateScoreScreen_();
}

//=========================================================
void MainWindow::on_actionPrint_triggered()
//=========================================================
{
	WriteScoreToHtml_();

	QPrinter printer(QPrinter::HighResolution);
	printer.setOrientation(QPrinter::Landscape);
	printer.setPaperSize(QPrinter::A4);
	printer.setPageMargins(15, 10, 15, 5, QPrinter::Millimeter);
	QPrintPreviewDialog preview(&printer, this);
	connect(&preview, SIGNAL(paintRequested(QPrinter *)), SLOT(Print(QPrinter *)));
	preview.exec();
}

//=========================================================
void MainWindow::on_actionExport_triggered()
//=========================================================
{
	WriteScoreToHtml_();

	// save file to...
	QString selectedFilter;
	QString dateStr(m_pUi->dateEdit->text());
	dateStr.replace('.', '-');
	QString fileName = QFileDialog::getSaveFileName(this,
								tr("Export file to..."),
								tr("ScoreList_") + dateStr,
								tr("PDF File (*.pdf);;HTML File (*.html)"),
								&selectedFilter);
	if (!fileName.isEmpty())
	{
		// set wait cursor
		QApplication::setOverrideCursor(Qt::WaitCursor);

		if( fileName.endsWith(".html") )
		{
			QFile html(fileName);
			if( html.open(QFile::WriteOnly) )
			{
				QTextStream ts(&html);
				ts << m_htmlScore;
				ts.flush();
				html.close();
			}
		}
		else
		{
			QPrinter printer(QPrinter::HighResolution);
			printer.setOrientation(QPrinter::Landscape);
			printer.setOutputFormat(QPrinter::PdfFormat);
			printer.setPaperSize(QPrinter::A4);
			printer.setPageMargins(15, 10, 15, 5, QPrinter::Millimeter);
			printer.setOutputFileName(fileName);
			QTextEdit edit( m_htmlScore, this );
			edit.document()->print(&printer);
		}
		QApplication::restoreOverrideCursor();
	}
}

#else // TEAM_VIEW
//=========================================================
void MainWindow::on_actionManage_Classes_triggered()
//=========================================================
{
	FightCategoryManagerDlg dlg(m_pCategoryManager, this);
	if( QDialog::Accepted == dlg.exec() )
	{
		QString currentClass =
				m_pUi->comboBox_weight_class->currentText();

		m_pUi->comboBox_weight_class->clear();
		for(int i(0); i<m_pCategoryManager->CategoryCount(); ++i)
		{
			FightCategory t("");
			m_pCategoryManager->GetCategory(i, t);
			m_pUi->comboBox_weight_class->addItem(t.ToString());
		}
		int index = m_pUi->comboBox_weight_class->findText(currentClass);
		if( -1 == index )
		{
			index = 0;
			currentClass = m_pUi->comboBox_weight_class->itemText(index);
		}

		m_pUi->comboBox_weight_class->setCurrentIndex(index);
		on_comboBox_weight_class_currentIndexChanged(currentClass);
	}
}

//=========================================================
void MainWindow::on_comboBox_weight_currentIndexChanged(const QString& s)
//=========================================================
{
	m_pPrimaryView->SetWeight(s);
	m_pSecondaryView->SetWeight(s);
	m_pPrimaryView->UpdateView();
	m_pSecondaryView->UpdateView();
}

//=========================================================
void MainWindow::on_lineEdit_name_blue_textChanged(const QString& s)
//=========================================================
{
	m_pController->SetFighterName( eFighter_Blue, s );
}

//=========================================================
void MainWindow::on_lineEdit_name_white_textChanged(const QString& s)
//=========================================================
{
	m_pController->SetFighterName( eFighter_White, s );
}

//=========================================================
void MainWindow::on_comboBox_time_currentIndexChanged(const QString& s)
//=========================================================
{
	const QString name = m_pUi->comboBox_weight_class->currentText();
	FightCategory t(name);
	m_pCategoryManager->GetCategory(name, t);

	if( s == str_golden_score )
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

//=========================================================
void MainWindow::on_comboBox_weight_class_currentIndexChanged(const QString& s)
//=========================================================
{
	FightCategory t(s);
	m_pCategoryManager->GetCategory(s, t);

	// add weights
	m_pUi->comboBox_weight->clear();
	m_pUi->comboBox_weight->addItems(t.GetWeightsList());

	// trigger rount time update
	on_comboBox_time_currentIndexChanged(m_pUi->comboBox_time->currentText());

	m_pPrimaryView->SetCategory(s);
	m_pSecondaryView->SetCategory(s);
	m_pPrimaryView->UpdateView();
	m_pSecondaryView->UpdateView();
}
#endif //TEAM_VIEW else

void MainWindow::on_actionVisit_Project_Homepage_triggered()
{
	QUrl url("http://ipponboard.origo.ethz.ch");
	QDesktopServices::openUrl(url);
}

void MainWindow::on_actionOnline_Feedback_triggered()
{
	QUrl url("http://flo.mueckeimnetz.de/ipponboard/survey_de");
	QDesktopServices::openUrl(url);
}

void MainWindow::on_actionContact_Author_triggered()
{
	QUrl url("mailto:dev@mueckeimnetz.de?"
			 "subject=Ipponboard_v" + QCoreApplication::applicationVersion() +
			 "&body=Please tell us what you want to know/suggest...");
	QDesktopServices::openUrl(url);
}

void MainWindow::change_lang(bool beQuiet)
{
	// set checks
	m_pUi->actionLang_Deutsch->setChecked("de" == m_Language);
	m_pUi->actionLang_English->setChecked("en" == m_Language);

	if( !beQuiet )
	{
		QMessageBox::information(this, QCoreApplication::applicationName(),
			tr("Please restart the application so that the changes can take effect."));
	}
}

void MainWindow::on_actionLang_Deutsch_triggered(bool val)
{
	if( val )
	{
		m_Language = "de";
		change_lang();
	}
}

void MainWindow::on_actionLang_English_triggered(bool val)
{
	if( val )
	{
		m_Language = "en";
		change_lang();
	}
}

void MainWindow::on_action_Info_Header_triggered(bool val)
{
	m_pPrimaryView->SetShowInfoHeader(val);
	m_pSecondaryView->SetShowInfoHeader(val);
}

void MainWindow::on_actionSet_Hold_Timer_triggered()
{
	bool ok(false);
	const int seconds = QInputDialog::getInt(
		0,
		tr("Set Value"),
		tr("Set value to (ss):"),
		0,	// value
		0,	// min
		59,	// max
		1,	// step
		&ok);
	if( ok )
		m_pController->SetTimerValue(eTimer_Hold, QString::number(seconds));
}

void MainWindow::on_actionSet_Main_Timer_triggered()
{
//	if( m_pController->GetCurrentState() == eState_SonoMama ||
//		m_pController->GetCurrentState() == eState_TimerStopped )
	{
		bool ok(false);
		const QString time = QInputDialog::getText(
			0,
			tr("Set Value"),
			tr("Set value to (m:ss):"),
			QLineEdit::Normal,
			m_pController->GetTimeText(eTimer_Main),
			&ok);
		if( ok )
			m_pController->SetTimerValue(eTimer_Main, time);
	}
}

#ifdef TEAM_VIEW
void MainWindow::on_pushButton_weights_pressed()
{
	bool ok(false);
	const QString weights = QInputDialog::getText(
		0,
		tr("Set Weights"),
		tr("Set weights (separated by ';'):"),
		QLineEdit::Normal,
		m_pUi->lineEdit_weights->text(),
		&ok);
	if( ok )
	{
		if( 4 != weights.count(';') )
		{
			QMessageBox::critical(this, "Wrong values",
				"You need to specify 5 weight classes separated by ';'!" );
			on_pushButton_weights_pressed();
		}
		else
		{
			update_weights(weights);
		}
	}
}

void MainWindow::update_weights(QString weightString)
{
	m_pUi->lineEdit_weights->setText(weightString);
	m_pController->SetWeights(weightString.split(';'));
}

void MainWindow::on_pushButton_copySwitched_pressed()
{
	m_pController->CopyAndSwitchGuestFighters();
}

void MainWindow::on_actionSet_Round_Time_triggered()
{
	bool ok(false);
	const QString time = QInputDialog::getText(
		0,
		tr("Set Value"),
		tr("Set value to (m:ss):"),
		QLineEdit::Normal,
		m_pController->GetRoundTime(),
		&ok);

	if( ok )
		m_pController->SetRoundTime(time);
}
void MainWindow::on_comboBox_mode_currentIndexChanged(QString s)
{
	if( s == str_mode_bundesliga )
	{
		m_pController->GetTournamentScoreModel(0)->SetNumRows(7);
		m_pController->GetTournamentScoreModel(1)->SetNumRows(7);
	}
	else	// str_mode_bayernliga
	{
		m_pController->GetTournamentScoreModel(0)->SetNumRows(10);
		m_pController->GetTournamentScoreModel(1)->SetNumRows(10);
	}
}

void MainWindow::on_button_current_round_clicked(bool checked)
{
	m_pController->SetCurrentFight( 0 );

	if( checked )
	{
		m_pUi->button_current_round->setText(tr("2nd Round"));
		m_pController->SetCurrentTournament(1);
	}
	else
	{
		m_pUi->button_current_round->setText(tr("1st Round"));
		m_pController->SetCurrentTournament(0);
	}

	UpdateFightNumber_();
}
#endif


