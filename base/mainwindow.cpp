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
#include "../base/settingsdlg.h"

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
	, m_secondScreenNo(0)
	, m_bAlwaysShow(true)
	, m_bAutoSize(true)
	, m_secondScreenSize()
	, m_buttonHajimeMatte(-1)
	, m_buttonOsaekomiToketa(-1)
	, m_buttonNext(-1)
	, m_buttonPrev(-1)
	, m_buttonPause(-1)
	, m_buttonReset(-1)
	, m_buttonReset2(-1)
	, m_buttonResetHoldBlue(-1)
	, m_buttonResetHoldWhite(-1)
	, m_buttonBlueHolding(-1)
	, m_buttonWhiteHolding(-1)
	, m_buttonHansokumakeBlue(-1)
	, m_buttonHansokumakeWhite(-1)
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
	m_pController->ClearMatches();

	updateLangCheckStates_();

#ifdef TEAM_VIEW
	for( int i = 0; i < m_pClubManager->ClubCount(); ++i )
	{
		Ipponboard::Club club;
		m_pClubManager->GetClub(i, club);
		QIcon icon(club.logoFile);
		m_pUi->comboBox_club_home->addItem(icon, club.ToString());
		m_pUi->comboBox_club_guest->addItem(icon, club.ToString());
	}

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
	index = m_pUi->comboBox_club_guest->findText("TG Landshut");
	m_pUi->comboBox_club_guest->setCurrentIndex(index);
	//m_pUi->tableView_tournament_list1->setSpan(2,2,1,2);
#endif

	//
	// load stored settings
	//
	ReadSettings_();
	if( m_bAlwaysShow )
	{
		m_pUi->actionShow_SecondaryView->setChecked(true);
		on_actionShow_SecondaryView_triggered();
	}

#ifdef TEAM_VIEW
	m_pUi->dateEdit->setDate(QDate::currentDate());
#else
	// mat
	const QString mat = tr("Mat");
	m_pUi->comboBox_mat->addItem(mat + " 1");
	m_pUi->comboBox_mat->addItem(mat + " 2");
	m_pUi->comboBox_mat->addItem(mat + " 3");
	m_pUi->comboBox_mat->addItem(mat + " 4");
	m_pUi->comboBox_mat->addItem(mat + " 5");
	m_pUi->comboBox_mat->addItem(mat + " 6");
	m_pUi->comboBox_mat->addItem(mat + " 7");
	m_pUi->comboBox_mat->addItem(mat + " 8");
	m_pUi->comboBox_mat->addItem(mat + " 9");

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
	UpdateMatchNumber_();

	m_pUi->button_pause->click();	// we start with pause!
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
	WriteSettings_();

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
void MainWindow::WriteSettings_()
//=========================================================
{
	//QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	const QString ini(str_ini_name);
	QSettings settings(ini, QSettings::IniFormat, this);

	settings.beginGroup(str_tag_Main);
	settings.setValue(str_tag_size, size());
	settings.setValue(str_tag_pos, pos());
	settings.setValue(str_tag_SecondScreen, m_secondScreenNo);
	settings.setValue(str_tag_SecondScreenSize, m_secondScreenSize);
	settings.setValue(str_tag_AutoSize, m_bAutoSize);
	settings.setValue(str_tag_AlwaysShow, m_bAlwaysShow);
	settings.endGroup();

	settings.beginGroup(str_tag_Fonts);
	settings.setValue(str_tag_TextFont1, m_textFont.toString() );
	settings.setValue(str_tag_DigitFont, m_pPrimaryView->GetDigitFont().toString() );
	settings.endGroup();

	settings.beginGroup(str_tag_Colors);
	settings.setValue(str_tag_InfoTextColor, m_pPrimaryView->GetInfoTextColor());
	settings.setValue(str_tag_InfoTextBgColor, m_pPrimaryView->GetInfoTextBgColor());
	settings.setValue(str_tag_TextColorBlue, m_pPrimaryView->GetTextColorBlue());
	settings.setValue(str_tag_TextBgColorBlue, m_pPrimaryView->GetTextBgColorBlue());
	settings.setValue(str_tag_TextColorWhite, m_pPrimaryView->GetTextColorWhite());
	settings.setValue(str_tag_TextBgColorWhite, m_pPrimaryView->GetTextBgColorWhite());
	settings.setValue(str_tag_MainClockColorRunning, m_pPrimaryView->GetMainClockColor1());
	settings.setValue(str_tag_MainClockColorStopped, m_pPrimaryView->GetMainClockColor2());
	settings.endGroup();

	// Styles
	settings.beginGroup(str_tag_Styles);
	settings.setValue(str_tag_BgStyle, m_pUi->frame_primary_view->styleSheet());
	settings.endGroup();

	settings.beginGroup( str_tag_Input );
	settings.setValue( str_tag_buttonHajimeMatte, m_buttonHajimeMatte );
	settings.setValue( str_tag_buttonOsaekomiToketa, m_buttonOsaekomiToketa );
	settings.setValue( str_tag_buttonNext, m_buttonNext );
	settings.setValue( str_tag_buttonPrev, m_buttonPrev );
	settings.setValue( str_tag_buttonPause, m_buttonPause );
	settings.setValue( str_tag_buttonReset, m_buttonReset );
	settings.setValue( str_tag_buttonReset2, m_buttonReset2 );
	settings.setValue( str_tag_buttonResetHoldBlue, m_buttonResetHoldBlue );
	settings.setValue( str_tag_buttonResetHoldWhite, m_buttonResetHoldWhite );
	settings.setValue( str_tag_buttonBlueHolding, m_buttonBlueHolding );
	settings.setValue( str_tag_buttonWhiteHolding, m_buttonWhiteHolding );
	settings.setValue( str_tag_buttonHansokumakeBlue, m_buttonHansokumakeBlue );
	settings.setValue( str_tag_buttonHansokumakeWhite, m_buttonHansokumakeWhite );
	settings.setValue( str_tag_invertX, m_pGamePad->IsInverted( FMlib::Gamepad::eAxis_X ) );
	settings.setValue( str_tag_invertY, m_pGamePad->IsInverted( FMlib::Gamepad::eAxis_Y ) );
	settings.setValue( str_tag_invertR, m_pGamePad->IsInverted( FMlib::Gamepad::eAxis_R ) );
	settings.setValue( str_tag_invertZ, m_pGamePad->IsInverted( FMlib::Gamepad::eAxis_Z ) );
	settings.endGroup();

	settings.beginGroup( str_tag_Sounds );
	settings.setValue( str_tag_sound_time_ends, m_pController->GetGongFile() );
	settings.endGroup();
}

//=========================================================
void MainWindow::ReadSettings_()
//=========================================================
{
	//QSettings settings(QCoreApplication::organizationName(), QCoreApplication::applicationName());
	const QString ini(str_ini_name);
	QSettings settings(ini, QSettings::IniFormat, this);

	// MainWindow
	settings.beginGroup(str_tag_Main);
	//resize(settings.value(str_tag_size, size()).toSize());
	move(settings.value(str_tag_pos, QPoint(200, 200)).toPoint());
	m_secondScreenNo = settings.value(str_tag_SecondScreen, 0).toInt();
	m_secondScreenSize = settings.value(str_tag_SecondScreenSize,
										QSize(1024,768)).toSize();
	m_bAutoSize = settings.value(str_tag_AutoSize, true).toBool();
	m_bAlwaysShow = settings.value(str_tag_AlwaysShow, false).toBool();
	settings.endGroup();

	// Fonts
	settings.beginGroup(str_tag_Fonts);
	QFont font = m_pPrimaryView->GetTextFont();
	font.fromString( settings.value(str_tag_TextFont1, font.toString()).toString() );
	UpdateTextFont_(font);
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
	UpdateInfoTextColor_(fgColor, bgColor);

	fgColor = m_pSecondaryView->GetTextColorBlue();
	bgColor = m_pSecondaryView->GetTextBgColorBlue();
	if( settings.contains(str_tag_TextColorBlue) )
		fgColor = settings.value(str_tag_TextColorBlue).value<QColor>();
	if( settings.contains(str_tag_TextBgColorBlue) )
		bgColor = settings.value(str_tag_TextBgColorBlue).value<QColor>();
	UpdateTextColorBlue_(fgColor, bgColor);

	fgColor = m_pPrimaryView->GetTextColorWhite();
	bgColor = m_pSecondaryView->GetTextBgColorWhite();
	if( settings.contains(str_tag_TextColorWhite) )
		fgColor = settings.value(str_tag_TextColorWhite).value<QColor>();
	if( settings.contains(str_tag_TextBgColorWhite) )
		bgColor = settings.value(str_tag_TextBgColorWhite).value<QColor>();
	UpdateTextColorWhite_(fgColor, bgColor);

	fgColor = m_pPrimaryView->GetMainClockColor1();
	bgColor = m_pPrimaryView->GetMainClockColor2();
	if( settings.contains(str_tag_MainClockColorRunning) )
		fgColor = settings.value(str_tag_MainClockColorRunning).value<QColor>();
	if( settings.contains(str_tag_MainClockColorStopped) )
		bgColor = settings.value(str_tag_MainClockColorStopped).value<QColor>();
	m_pPrimaryView->SetMainClockColor(fgColor, bgColor);
	m_pSecondaryView->SetMainClockColor(fgColor, bgColor);
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
	m_buttonHajimeMatte = settings.value( str_tag_buttonHajimeMatte,
										  Gamepad::ePov_down ).toInt();
	m_buttonOsaekomiToketa = settings.value( str_tag_buttonOsaekomiToketa,
											 Gamepad::ePov_up ).toInt();
	m_buttonNext = settings.value( str_tag_buttonNext,
								   Gamepad::eButton10 ).toInt();
	m_buttonPrev = settings.value( str_tag_buttonPrev,
								   Gamepad::eButton9 ).toInt();
	m_buttonPause = settings.value( str_tag_buttonPause,
									Gamepad::eButton2 ).toInt();
	m_buttonReset = settings.value( str_tag_buttonReset,
									Gamepad::eButton1 ).toInt();
	m_buttonReset2 = settings.value( str_tag_buttonReset2,
									Gamepad::eButton4 ).toInt();
	m_buttonResetHoldBlue = settings.value( str_tag_buttonResetHoldBlue,
									Gamepad::eButton6 ).toInt();
	m_buttonResetHoldWhite = settings.value( str_tag_buttonResetHoldWhite,
									Gamepad::eButton8 ).toInt();
	m_buttonBlueHolding = settings.value( str_tag_buttonBlueHolding,
										  Gamepad::eButton5 ).toInt();
	m_buttonWhiteHolding = settings.value( str_tag_buttonWhiteHolding,
										   Gamepad::eButton7 ).toInt();
	m_buttonHansokumakeBlue = settings.value( str_tag_buttonHansokumakeBlue,
											  Gamepad::eButton11 ).toInt();
	m_buttonHansokumakeWhite = settings.value( str_tag_buttonHansokumakeWhite,
											   Gamepad::eButton12 ).toInt();

	m_pGamePad->SetInverted( FMlib::Gamepad::eAxis_X, settings.value( str_tag_invertX, false ).toBool());
	m_pGamePad->SetInverted( FMlib::Gamepad::eAxis_Y, settings.value( str_tag_invertY, true ).toBool());
	m_pGamePad->SetInverted( FMlib::Gamepad::eAxis_R, settings.value( str_tag_invertR, true ).toBool());
	m_pGamePad->SetInverted( FMlib::Gamepad::eAxis_Z, settings.value( str_tag_invertZ, true ).toBool());
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
void MainWindow::UpdateInfoTextColor_(const QColor& color, const QColor& bgColor)
//=========================================================
{
	m_pPrimaryView->SetInfoTextColor(color, bgColor);
	m_pSecondaryView->SetInfoTextColor(color, bgColor);
#ifdef TEAM_VIEW
	//m_pScoreScreen->SetInfoTextColor(color, bgColor);
#endif

}

//=========================================================
void MainWindow::UpdateTextColorBlue_(const QColor& color, const QColor& bgColor)
//=========================================================
{
	m_pPrimaryView->SetTextColorBlue(color, bgColor);
	m_pSecondaryView->SetTextColorBlue(color, bgColor);
#ifdef TEAM_VIEW
	m_pScoreScreen->SetTextColorBlue(color, bgColor);
#endif
}

//=========================================================
void MainWindow::UpdateTextColorWhite_(const QColor& color, const QColor& bgColor)
//=========================================================
{
	m_pPrimaryView->SetTextColorWhite(color, bgColor);
	m_pSecondaryView->SetTextColorWhite(color, bgColor);
#ifdef TEAM_VIEW
	m_pScoreScreen->SetTextColorWhite(color, bgColor);
#endif
}

//=========================================================
void MainWindow::UpdateTextFont_(const QFont & font)
//=========================================================
{
	m_textFont = font;
	m_pPrimaryView->SetTextFont(font);
	m_pSecondaryView->SetTextFont(font);
#ifdef TEAM_VIEW
	m_pScoreScreen->SetTextFont(font);
#endif

}

//=========================================================
void MainWindow::ShowHideView_() const
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
void MainWindow::UpdateViews_()
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
void MainWindow::UpdateMatchNumber_()
//=========================================================
{
	const int current = m_pController->GetCurrentMatchIndex() + 1;
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
	QFile file("list_output.html");
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
	for(int i(0); i<Ipponboard::eTournament_MatchCount; ++i)
	{
		const Match& match(m_pController->GetMatch(0, i));

		QString name_blue( match.fighters[eFighter_Blue].name );
		QString name_white( match.fighters[eFighter_White].name );
		const Score& score_blue( match.scores[eFighter_Blue] );
		const Score& score_white( match.scores[eFighter_White] );

		QString round("<tr>");
		round.append("<td><center>" + QString::number(i+1) + "</center></td>"); // number
		round.append("<td><center>" + match.weight + "</center></td>"); // weight
		round.append("<td><center>" + name_blue + "</center></td>"); // name
		round.append("<td><center>" + QString::number(score_blue.Yuko()) + "</center></td>"); // Y
		round.append("<td><center>" + QString::number(score_blue.Wazaari()) + "</center></td>"); // W
		round.append("<td><center>" + QString::number(score_blue.Ippon()) + "</center></td>"); // I
		round.append("<td><center>" + QString::number(score_blue.Shido()) + "</center></td>"); // S
		round.append("<td><center>" + QString::number(score_blue.Hansokumake()) + "</center></td>"); // H
		round.append("<td><center>" + QString::number(match.HasWon(eFighter_Blue)) + "</center></td>"); // won
		round.append("<td><center>" + QString::number(match.ScorePoints(eFighter_Blue)) + "</center></td>"); // score
		round.append("<td><center>" + name_white + "</center></td>"); // name
		round.append("<td><center>" + QString::number(score_white.Yuko()) + "</center></td>"); // Y
		round.append("<td><center>" + QString::number(score_white.Wazaari()) + "</center></td>"); // W
		round.append("<td><center>" + QString::number(score_white.Ippon()) + "</center></td>"); // I
		round.append("<td><center>" + QString::number(score_white.Shido()) + "</center></td>"); // S
		round.append("<td><center>" + QString::number(score_white.Hansokumake()) + "</center></td>"); // H
		round.append("<td><center>" + QString::number(match.HasWon(eFighter_White)) + "</center></td>"); // won
		round.append("<td><center>" + QString::number(match.ScorePoints(eFighter_White)) + "</center></td>"); // score
		round.append("<td><center>" + match.GetRoundTimeText() + "</center></td>"); // time
		round.append("</tr>\n");
		rounds.append(round);
	}
	m_htmlScore.replace( "%FIRST_ROUND%", rounds );

	// second round
	rounds.clear();
	for(int i(0); i<Ipponboard::eTournament_MatchCount; ++i)
	{
		const Match& match(m_pController->GetMatch(1, i));

		QString name_blue( match.fighters[eFighter_Blue].name );
		QString name_white( match.fighters[eFighter_White].name );
		const Score& score_blue( match.scores[eFighter_Blue] );
		const Score& score_white( match.scores[eFighter_White] );

		QString round("<tr>");
		round.append("<td><center>" + QString::number(i+1) + "</center></td>"); // number
		round.append("<td><center>" + match.weight + "</center></td>"); // weight
		round.append("<td><center>" + name_blue + "</center></td>"); // name
		round.append("<td><center>" + QString::number(score_blue.Yuko()) + "</center></td>"); // Y
		round.append("<td><center>" + QString::number(score_blue.Wazaari()) + "</center></td>"); // W
		round.append("<td><center>" + QString::number(score_blue.Ippon()) + "</center></td>"); // I
		round.append("<td><center>" + QString::number(score_blue.Shido()) + "</center></td>"); // S
		round.append("<td><center>" + QString::number(score_blue.Hansokumake()) + "</center></td>"); // H
		round.append("<td><center>" + QString::number(match.HasWon(eFighter_Blue)) + "</center></td>"); // won
		round.append("<td><center>" + QString::number(match.ScorePoints(eFighter_Blue)) + "</center></td>"); // score
		round.append("<td><center>" + name_white + "</center></td>"); // name
		round.append("<td><center>" + QString::number(score_white.Yuko()) + "</center></td>"); // Y
		round.append("<td><center>" + QString::number(score_white.Wazaari()) + "</center></td>"); // W
		round.append("<td><center>" + QString::number(score_white.Ippon()) + "</center></td>"); // I
		round.append("<td><center>" + QString::number(score_white.Shido()) + "</center></td>"); // S
		round.append("<td><center>" + QString::number(score_white.Hansokumake()) + "</center></td>"); // H
		round.append("<td><center>" + QString::number(match.HasWon(eFighter_White)) + "</center></td>"); // won
		round.append("<td><center>" + QString::number(match.ScorePoints(eFighter_White)) + "</center></td>"); // score
		round.append("<td><center>" + match.GetRoundTimeText() + "</center></td>"); // time
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
			"<p>&copy; Copyright 2010 Florian M&uuml;cke. All rights reserved.</p>"
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
	ShowHideView_();
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
		m_pController->ClearMatches();
}

//=========================================================
void MainWindow::on_actionPreferences_triggered()
//=========================================================
{
	SettingsDlg dlg(this);
	dlg.SetTextSettings( m_textFont,
						 m_pPrimaryView->GetInfoTextColor(),
						 m_pPrimaryView->GetInfoTextBgColor() );
	dlg.SetTextColorsBlue( m_pPrimaryView->GetTextColorBlue(),
						   m_pPrimaryView->GetTextBgColorBlue() );
	dlg.SetTextColorsWhite( m_pPrimaryView->GetTextColorWhite(),
						   m_pPrimaryView->GetTextBgColorWhite() );
	dlg.SetScreensSettings(m_bAlwaysShow, m_secondScreenNo, m_bAutoSize,
						   m_secondScreenSize);

	dlg.SetButtonHajimeMatte(m_buttonHajimeMatte);
	dlg.SetButtonOsaekomiToketa(m_buttonOsaekomiToketa);
	dlg.SetButtonNext(m_buttonNext);
	dlg.SetButtonPrev(m_buttonPrev);
	dlg.SetButtonPause(m_buttonPause);
	dlg.SetButtonReset(m_buttonReset);
	dlg.SetButtonReset2(m_buttonReset2);
	dlg.SetButtonResetHoldBlue(m_buttonResetHoldBlue);
	dlg.SetButtonResetHoldWhite(m_buttonResetHoldWhite);
	dlg.SetButtonBlueHolding(m_buttonBlueHolding);
	dlg.SetButtonWhiteHolding(m_buttonWhiteHolding);
	dlg.SetButtonHansokumakeBlue(m_buttonHansokumakeBlue);
	dlg.SetButtonHansokumakeWhite(m_buttonHansokumakeWhite);
	dlg.SetInvertedX(m_pGamePad->IsInverted(FMlib::Gamepad::eAxis_X));
	dlg.SetInvertedY(m_pGamePad->IsInverted(FMlib::Gamepad::eAxis_Y));
	dlg.SetInvertedR(m_pGamePad->IsInverted(FMlib::Gamepad::eAxis_R));
	dlg.SetInvertedZ(m_pGamePad->IsInverted(FMlib::Gamepad::eAxis_Z));
	dlg.SetGongFile(m_pController->GetGongFile());

	if( QDialog::Accepted == dlg.exec() )
	{
		UpdateTextFont_(dlg.GetTextFont());
		UpdateInfoTextColor_(dlg.GetInfoTextColor(), dlg.GetInfoTextBgColor());
		UpdateTextColorBlue_(dlg.GetTextColorBlue(), dlg.GetTextBgColorBlue());
		UpdateTextColorWhite_(dlg.GetTextColorWhite(), dlg.GetTextBgColorWhite());

		m_bAlwaysShow = dlg.IsShowAlways();
		m_secondScreenNo = dlg.GetSelectedScreen();
		m_bAutoSize = dlg.IsAutoSize();
		m_secondScreenSize = dlg.GetSize();

		m_buttonHajimeMatte = dlg.GetButtonHajimeMatte();
		m_buttonOsaekomiToketa = dlg.GetButtonOsaekomiToketa();
		m_buttonNext = dlg.GetButtonNext();
		m_buttonPrev = dlg.GetButtonPrev();
		m_buttonPause = dlg.GetButtonPause();
		m_buttonReset = dlg.GetButtonReset();
		m_buttonReset2 = dlg.GetButtonReset2();
		m_buttonResetHoldBlue = dlg.GetButtonResetHoldBlue();
		m_buttonResetHoldWhite = dlg.GetButtonResetHoldWhite();
		m_buttonBlueHolding = dlg.GetButtonBlueHolding();
		m_buttonWhiteHolding = dlg.GetButtonWhiteHolding();
		m_buttonHansokumakeBlue = dlg.GetButtonHansokumakeBlue();
		m_buttonHansokumakeWhite = dlg.GetButtonHansokumakeWhite();
		m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_X, dlg.IsInvertedX());
		m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_Y, dlg.IsInvertedY());
		m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_R, dlg.IsInvertedR());
		m_pGamePad->SetInverted(FMlib::Gamepad::eAxis_Z, dlg.IsInvertedZ());
		m_pController->SetGongFile(dlg.GetGongFile());

		// save changes to file
		WriteSettings_();
	}
}

//=========================================================
void MainWindow::on_button_reset_clicked()
//=========================================================
{
//	QMessageBox::StandardButton answer =
//		QMessageBox::question( this,
//							   tr("Reset"),
//							   tr("Really reset current match?"),
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

	if( m_pGamePad->WasPovPressed( Gamepad::ePov_down ) )
	{
		m_pController->DoAction( eAction_Hajime_Matte, eFighter_Nobody );
	}
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_buttonResetHoldBlue)) )
	{
		m_pController->DoAction( eAction_ResetOsaeKomi, eFighter_Blue );
	}
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_buttonResetHoldWhite)) )
	{
		m_pController->DoAction( eAction_ResetOsaeKomi, eFighter_White );
	}
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_buttonBlueHolding)) )
	{
		if( eState_Holding != m_pController->GetCurrentState() )
		{
			m_pController->DoAction( eAction_OsaeKomi_Toketa, eFighter_Blue );
		}
		else
		{
//			if( eFighter_Blue != m_pController->GetLastHolder() )
//				m_pController->DoAction( eAction_SetOsaekomi, eFighter_Blue );
//			else
//				// Toketa!
				m_pController->DoAction( eAction_OsaeKomi_Toketa, eFighter_Blue );
		}
	}
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_buttonWhiteHolding)) )
	{
		if( eState_Holding != m_pController->GetCurrentState() )
		{
			m_pController->DoAction( eAction_OsaeKomi_Toketa, eFighter_White );
		}
		else
		{
//			if( eFighter_White != m_pController->GetLastHolder() )
//				m_pController->DoAction( eAction_SetOsaekomi, eFighter_White );
//			else
//				// Toketa!
				m_pController->DoAction( eAction_OsaeKomi_Toketa, eFighter_White );
		}
	}
	// reset
	else if(
		m_pGamePad->IsPressed(Gamepad::EButton(m_buttonReset)) &&
		m_pGamePad->IsPressed(Gamepad::EButton(m_buttonReset2)) )
	{
		m_pController->DoAction(eAction_Reset, eFighter_Nobody );
	}
	// back
#ifdef TEAM_VIEW
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_buttonPrev)) )
	{
		on_button_prev_clicked();
	}
	// next
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_buttonNext)) )
	{
		on_button_next_clicked();
	}
#endif
	// hansokumake blue
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_buttonHansokumakeBlue)) )
	{
		const bool revoke(m_pController->GetScore(
			eFighter_Blue, ePoint_Hansokumake) != 0);
		m_pController->DoAction( eAction_Hansokumake, eFighter_Blue, revoke);
	}
	// hansokumake white
	else if( m_pGamePad->WasPressed(Gamepad::EButton(m_buttonHansokumakeWhite)) )
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
	UpdateViews_();
}

#ifdef TEAM_VIEW
//=========================================================
void MainWindow::on_tabWidget_currentChanged(int /*index*/)
//=========================================================
{
	UpdateViews_();
}

//=========================================================
void MainWindow::on_actionManage_Clubs_triggered()
//=========================================================
{
	ClubManagerDlg dlg(m_pClubManager, this);
	dlg.exec();
}

//=========================================================
void MainWindow::on_actionLoad_Demo_Data_triggered()
//=========================================================
{
	m_pController->ClearMatches();																					//  Y  W  I  S  H  Y  W  I  S  H
	m_pController->SetMatch( 0, 0, "-90", "Sebastian Hölzl", "TG Landshut", "Oliver Sach", "TSV Königsbrunn",			3, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetMatch( 0, 1, "-90", "Stefan Grünert", "TG Landshut", "Marc Schäfer", "TSV Königsbrunn",			3, 2, 0, 0, 0, 0, 0, 0, 1, 0);
	m_pController->SetMatch( 0, 2, "+90", "Andreas Neumaier", "TG Landshut", "Daniel Nussbächer", "TSV Königsbrunn",	0, 0, 0, 1, 0, 0, 0, 1, 1, 0);
	m_pController->SetMatch( 0, 3, "+90", "Jürgen Neumeier", "TG Landshut", "Anderas Mayer", "TSV Königsbrunn",			1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetMatch( 0, 4, "-73", "Benny Mahl", "TG Landshut", "Christopher Benka", "TSV Königsbrunn"	,		2, 0, 1, 1, 0, 0, 0, 0, 3, 0);
	m_pController->SetMatch( 0, 5, "-73", "Josef Sellmaier", "TG Landshut", "Jan-Michael König", "TSV Königsbrunn",		0, 1, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetMatch( 0, 6, "-66", "Alexander Keil", "TG Landshut", "Arthur Sipple", "TSV Königsbrunn",			2, 1, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetMatch( 0, 7, "-66", "Dominic Bogner", "TG Landshut", "Thomas Schaller", "TSV Königsbrunn",		0, 0, 1, 0, 0, 2, 0, 0, 0, 0);
	m_pController->SetMatch( 0, 8, "-81", "Sebastian Schmieder", "TG Landshut", "Gerhard Wessinger", "TSV Königsbrunn",	0, 1, 1, 1, 0, 1, 0, 0, 0, 0);
	m_pController->SetMatch( 0, 9, "-81", "Rainer Neumaier", "TG Landshut", "Georg Benka", "TSV Königsbrunn",			1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
																												//  Y  W  I  S  H  Y  W  I  S  H
	m_pController->SetMatch( 1, 0, "-90", "Sebastian Hölzl", "TG Landshut", "Marc Schäfer", "TSV Königsbrunn",		0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetMatch( 1, 1, "-90", "Stefan Grunert", "TG Landshut", "Florian Kürten", "TSV Königsbrunn",		0, 1, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetMatch( 1, 2, "+90", "Andreas Neumaier", "TG Landshut", "Andreas Mayer", "TSV Königsbrunn",	1, 2, 0, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetMatch( 1, 3, "+90", "Jürgen Neumaier", "TG Landshut", "Daniel Nussbächer", "TSV Königsbrunn",	0, 0, 0, 2, 0, 0, 0, 1, 2, 0);
	m_pController->SetMatch( 1, 4, "-73", "Matthias Feigl", "TG Landshut", "Jan-Michael König", "TSV Königsbrunn",	2, 1, 0, 1, 0, 0, 0, 0, 1, 0);
	m_pController->SetMatch( 1, 5, "-73", "Josef Sellmaier", "TG Landshut", "Christopher Benka", "TSV Königsbrunn",	0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	m_pController->SetMatch( 1, 6, "-66", "Jörg Herzog", "TG Landshut", "Thomas Schaller", "TSV Königsbrunn",		0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
	m_pController->SetMatch( 1, 7, "-66", "Alex Selwitschka", "TG Landshut", "Jonas Allinger", "TSV Königsbrunn",	0, 1, 1, 0, 0, 1, 0, 0, 0, 0);
	m_pController->SetMatch( 1, 8, "-81", "Eugen Makaritsch", "TG Landshut", "Georg Benka", "TSV Königsbrunn",		0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
	m_pController->SetMatch( 1, 9, "-81", "Rainer Neumaier", "TG Landshut", "Gerhard Wessinger", "TSV Königsbrunn",	0, 0, 1, 1, 0, 0, 0, 0, 0, 0);
	//m_pController->SetCurrentMatch(0);
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
	if( 0 == m_pController->GetCurrentMatchIndex() )
		return;

	m_pController->SetCurrentMatch( m_pController->GetCurrentMatchIndex() - 1 );

	UpdateMatchNumber_();
}

//=========================================================
void MainWindow::on_button_next_clicked()
//=========================================================
{
	if( m_pController->GetCurrentMatchIndex() == m_pController->GetMatchCount() - 1 )
		m_pController->SetCurrentMatch( m_pController->GetCurrentMatchIndex() );
	else
		m_pController->SetCurrentMatch( m_pController->GetCurrentMatchIndex() + 1 );

	UpdateMatchNumber_();
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
void MainWindow::on_comboBox_round_currentIndexChanged(const QString& s)
//=========================================================
{
	if( s == tr("START") )
	{
		if( !m_pUi->button_pause->isChecked() )
			m_pUi->button_pause->click();
		//on_button_pause_clicked();
		return;
	}
	else if( s == tr("1st") )
	{
		if( m_pUi->button_pause->isChecked() )
			m_pUi->button_pause->click();

		m_pController->SetCurrentMatch( 0 );
		m_pController->SetCurrentTournament(0);
	}
	else if( s == tr("BREAK") )
	{
		if( !m_pUi->button_pause->isChecked() )
			m_pUi->button_pause->click();
		return;
	}
	else if( s == tr("2nd") )
	{
		if( m_pUi->button_pause->isChecked() )
			m_pUi->button_pause->click();

		m_pController->SetCurrentMatch( 0 );
		m_pController->SetCurrentTournament(1);
	}
	else if( s == tr("FINISHED") )
	{
		// be sure that all pages are saved
		for( int i = m_pController->GetCurrentTournamentIndex(); i<2; ++i )
		{
			while( eTournament_MatchCount >
				m_pController->GetCurrentMatchIndex() + 1)
			{
				m_pUi->button_next->click();
			}
		}

		if( !m_pUi->button_pause->isChecked() )
			m_pUi->button_pause->click();
		return;
	}

	UpdateMatchNumber_();
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
void MainWindow::on_comboBox_mat_currentIndexChanged(const QString& s)
//=========================================================
{
	m_pPrimaryView->SetMat(s);
	m_pSecondaryView->SetMat(s);
	m_pPrimaryView->UpdateView();
	m_pSecondaryView->UpdateView();
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
}
#endif //TEAM_VIEW else

void MainWindow::changeLang_(QString const& langStr)
{
	// load translation file(s)
	QSettings settings(//QSettings::NativeFormat,
							 QSettings::UserScope,
							 QCoreApplication::applicationName(),
							 QCoreApplication::applicationName());
	settings.setValue("Language", langStr);

	QMessageBox::information(this, QCoreApplication::applicationName(),
		tr("Please restart the application so that the changes can take effect."));

	updateLangCheckStates_();
}

void MainWindow::updateLangCheckStates_() const
{
	// load translation file(s)
	const QSettings settings(//QSettings::NativeFormat,
							 QSettings::UserScope,
							 QCoreApplication::applicationName(),
							 QCoreApplication::applicationName());

	if( settings.value("Language", "en").toString() == "de" )
	{
		m_pUi->actionDeutsch->setChecked(true);
		m_pUi->actionEnglish->setChecked(false);
	}
	else
	{
		m_pUi->actionDeutsch->setChecked(false);
		m_pUi->actionEnglish->setChecked(true);
	}
}

void MainWindow::on_actionDeutsch_triggered()
{
	changeLang_("de");
}

void MainWindow::on_actionEnglish_triggered()
{
	changeLang_("en");
}
