// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "MainWindowTeam.h"
#include "ui_MainWindowTeam.h"

#include "ScoreScreen.h"
#include "../base/ComboboxDelegate.h"
#include "../base/ClubManager.h"
#include "../base/ClubManagerDlg.h"
#include "../base/FighterManagerDlg.h"
#include "ModeManagerDlg.h"
//#include "../base/FightCategoryManager.h"
//#include "../base/FightCategoryManagerDlg.h"
#include "../base/View.h"
#include "../base/versioninfo.h"
#include "../core/Controller.h"
#include "../core/ControllerConfig.h"
#include "../core/Tournament.h"
#include "../core/TournamentModel.h"
#include "../gamepad/gamepad.h"
#include "../util/path_helpers.h"
#include "../Widgets/ScaledImage.h"
#include "../Widgets/ScaledText.h"

#include <QClipboard>
#include <QColorDialog>
#include <QComboBox>
#include <QCompleter>
#include <QDebug>
#include <QDesktopServices>
#include <QDesktopWidget>
#include <QDir>
#include <QFileDialog>
#include <QFontDialog>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPrintPreviewDialog>
#include <QPrinter>
#include <QSettings>
#include <QSplashScreen>
#include <QTableView>
#include <QTextEdit>
#include <QTimer>
#include <QUrl>

#include <functional>

namespace StrTags
{
static const char* const mode = "Mode";
static const char* const host = "Host";
}

using namespace FMlib;
using namespace Ipponboard;

namespace { bool initialized = false; }

MainWindowTeam::MainWindowTeam(QWidget* parent)
	: MainWindowBase(parent)
	, m_pUi(new Ui::MainWindowTeam)
	, m_pScoreScreen()
	, m_pClubManager()
	, m_htmlScore()
	, m_currentMode()
	, m_host()
	, m_FighterNamesHome()
	, m_FighterNamesGuest()
	, m_modes()
{
	m_pUi->setupUi(this);
}

MainWindowTeam::~MainWindowTeam()
{}

void MainWindowTeam::Init()
{
	m_pClubManager.reset(new Ipponboard::ClubManager());
	m_pScoreScreen.reset(new Ipponboard::ScoreScreen());

	MainWindowBase::Init();

	// set default background
	m_pScoreScreen->setStyleSheet(m_pUi->frame_primary_view->styleSheet());

	//
	// load tournament modes
	//
	QString errMsg;
	Ipponboard::TournamentMode::List modes;

	if (!Ipponboard::TournamentMode::ReadModes(MainWindowTeam::ModeConfigurationFileName(), modes, errMsg))
	{
		QMessageBox::critical(0,
							  QCoreApplication::tr("Error reading mode configurations"),
							  errMsg);

        throw std::runtime_error("Initialization failed!");
	}

	SetModes(modes);

	// load modes
	for (auto const & mode : m_modes)
	{
		m_pUi->comboBox_mode->addItem(mode.Description(), QVariant(mode.id));
	}

	initialized = true;

	int modeIndex = m_pUi->comboBox_mode->findData(QVariant(m_currentMode));

	if (-1 == modeIndex)
	{
		modeIndex = 0;
	}

	if (m_pUi->comboBox_mode->currentIndex() != modeIndex)
	{
		m_pUi->comboBox_mode->setCurrentIndex(modeIndex);
	}
	else
	{
		// be sure to trigger
		on_comboBox_mode_currentIndexChanged(m_pUi->comboBox_mode->currentIndex());
	}

	//
	// setup data
	//
	m_pUi->dateEdit->setDate(QDate::currentDate());
	update_club_views();

	//m_pUi->comboBox_club_guest->setCurrentIndex(0);

	// set fighter comboboxes
	//m_FighterNamesHome.push_back(QString::fromUtf8("Florian Mücke"));
	//m_FighterNamesHome.push_back(QString::fromUtf8("Wolfgang Schmied"));
	//m_FighterNamesHome.push_back(QString::fromUtf8("Tino Rupp"));
#if 0
	auto cbxFightersHome = new ComboBoxDelegate(this);
	cbxFightersHome->SetItems(m_FighterNamesHome);

	//m_FighterNamesGuest.push_back(QString::fromUtf8("Hans Dampf"));
	//m_FighterNamesGuest.push_back(QString::fromUtf8("Hans Wurst"));
	//m_FighterNamesGuest.push_back(QString::fromUtf8("Hans Im Glück"));
	auto cbxFightersGuest = new ComboBoxDelegate(this);
	cbxFightersGuest->SetItems(m_FighterNamesGuest);
	m_pUi->tableView_tournament_list1->setItemDelegateForColumn(TournamentModel::eCol_name1, cbxFightersHome);
	m_pUi->tableView_tournament_list2->setItemDelegateForColumn(TournamentModel::eCol_name1, cbxFightersHome);
	m_pUi->tableView_tournament_list1->setItemDelegateForColumn(TournamentModel::eCol_name2, cbxFightersGuest);
	m_pUi->tableView_tournament_list2->setItemDelegateForColumn(TournamentModel::eCol_name2, cbxFightersGuest);
#endif
	// make name columns auto-resizable
	m_pUi->tableView_tournament_list1->horizontalHeader()->setResizeMode(TournamentModel::eCol_name1, QHeaderView::Stretch);
	m_pUi->tableView_tournament_list1->horizontalHeader()->setResizeMode(TournamentModel::eCol_name2, QHeaderView::Stretch);
	m_pUi->tableView_tournament_list2->horizontalHeader()->setResizeMode(TournamentModel::eCol_name1, QHeaderView::Stretch);
	m_pUi->tableView_tournament_list2->horizontalHeader()->setResizeMode(TournamentModel::eCol_name2, QHeaderView::Stretch);

	// TEMP: hide weight cotrol
//	m_pUi->label_weight->hide();
//	m_pUi->lineEdit_weights->hide();
//	m_pUi->toolButton_weights->hide();
//	m_pUi->gridLayout_main->removeItem(m_pUi->horizontalSpacer_4);
//	delete m_pUi->horizontalSpacer_4;

	//update_weights("-66;-73;-81;-90;+90");
	//FIXME: check why this has not been in branch

	m_pUi->actionAutoAdjustPoints->setChecked(m_pController->IsAutoAdjustPoints());

	UpdateFightNumber_();
	UpdateButtonText_();

	//m_pUi->button_pause->click();	// we start with pause!
}

void MainWindowTeam::UpdateGoldenScoreView()
{
	m_pUi->button_golden_score->setEnabled(m_pController->GetRules()->IsOption_OpenEndGoldenScore());
	m_pUi->button_golden_score->setChecked(m_pController->IsGoldenScore());
}

void MainWindowTeam::closeEvent(QCloseEvent* event)
{
	MainWindowBase::closeEvent(event);

	if (m_pScoreScreen)
	{
		m_pScoreScreen->close();
	}
}

void MainWindowTeam::keyPressEvent(QKeyEvent* event)
{
	const bool isCtrlPressed = event->modifiers().testFlag(Qt::ControlModifier);
	const bool isAltPressed = event->modifiers().testFlag(Qt::AltModifier);

	//FIXME: copy and paste handling should be part of the table class!
	if (m_pUi->tabWidget->currentWidget() == m_pUi->tab_view)
	{
		switch (event->key())
		{
		case Qt::Key_Left:
			if (isCtrlPressed && isAltPressed)
			{
				m_pUi->button_prev->click();
				qDebug() << "Button [ Prev ] was triggered by keyboard";
			}
			else
			{
				MainWindowBase::keyPressEvent(event);
			}

			break;

		case Qt::Key_Right:
			if (isCtrlPressed && isAltPressed)
			{
				m_pUi->button_next->click();
				qDebug() << "Button [ Next ] was triggered by keyboard";
			}
			else
			{
				MainWindowBase::keyPressEvent(event);
			}

			break;

		case Qt::Key_F4:
			m_pUi->button_pause->click();
			qDebug() << "Button [ ResultScreen ] was triggered by keyboard";
			break;

		default:
			MainWindowBase::keyPressEvent(event);
			break;
		}
	}
	else if (m_pUi->tabWidget->currentWidget() == m_pUi->tab_score_table)
	{
		if (event->matches(QKeySequence::Copy))
		{
			if (QApplication::focusWidget() == m_pUi->tableView_tournament_list1)
			{
				slot_copy_cell_content_list1();
			}
			else if (QApplication::focusWidget() == m_pUi->tableView_tournament_list2)
			{
				slot_copy_cell_content_list2();
			}
		}
		else if (event->matches(QKeySequence::Paste))
		{
			if (QApplication::focusWidget() == m_pUi->tableView_tournament_list1)
			{
				slot_paste_cell_content_list1();
			}
			else if (QApplication::focusWidget() == m_pUi->tableView_tournament_list2)
			{
				slot_paste_cell_content_list2();
			}
		}
		else if (event->matches(QKeySequence::Delete))
		{
			if (QApplication::focusWidget() == m_pUi->tableView_tournament_list1)
			{
				slot_clear_cell_content_list1();
			}
			else if (QApplication::focusWidget() == m_pUi->tableView_tournament_list2)
			{
				slot_clear_cell_content_list2();
			}
		}
		else
		{
			MainWindowBase::keyPressEvent(event);
		}
	}
	else
	{
		//TODO: handle view keys
		//FIXME: handling should be part of the view class!
		//switch (event->key())
		//{
		//default:
		MainWindowBase::keyPressEvent(event);
		//    break;
		//}
	}
}

QStringList MainWindowTeam::get_list_templates()
{
	QDir dir(TournamentMode::TemplateDirName());
	QStringList filters;
	filters.append("*.html");
	return dir.entryList(filters, QDir::Files, QDir::Name);
}

void MainWindowTeam::write_specific_settings(QSettings& settings)
{
	settings.beginGroup(EditionNameShort());
	{
		settings.remove("");
		settings.setValue(StrTags::mode, m_currentMode);
		settings.setValue(StrTags::host, m_host);
		settings.setValue(str_tag_LabelHome, m_pController->GetHomeLabel());
		settings.setValue(str_tag_LabelGuest, m_pController->GetGuestLabel());
	}
	settings.endGroup();
}

void MainWindowTeam::read_specific_settings(QSettings& settings)
{
	settings.beginGroup(EditionNameShort());
	{
		m_currentMode = settings.value(StrTags::mode, "").toString();
		m_host = settings.value(StrTags::host, "").toString();

		m_pController->SetLabels(
			settings.value(str_tag_LabelHome, tr("Home")).toString(),
			settings.value(str_tag_LabelGuest, tr("Guest")).toString());
	}
	settings.endGroup();
}

void MainWindowTeam::on_actionManageFighters_triggered()
{
	MainWindowBase::on_actionManageFighters_triggered();

	FighterManagerDlg dlg(m_fighterManager, this);
	dlg.exec();
}

void MainWindowTeam::update_info_text_color(const QColor& color, const QColor& bgColor)
{
	MainWindowBase::update_info_text_color(color, bgColor);
	//m_pScoreScreen->SetInfoTextColor(color, bgColor);
}

void MainWindowTeam::update_text_color_first(const QColor& color, const QColor& bgColor)
{
	MainWindowBase::update_text_color_first(color, bgColor);
	m_pScoreScreen->SetTextColorFirst(color, bgColor);
}

void MainWindowTeam::update_text_color_second(const QColor& color, const QColor& bgColor)
{
	MainWindowBase::update_text_color_second(color, bgColor);
	m_pScoreScreen->SetTextColorSecond(color, bgColor);
}

void MainWindowTeam::update_fighter_name_font(const QFont& font)
{
	MainWindowBase::update_fighter_name_font(font);
	m_pScoreScreen->SetTextFont(font);
}

void MainWindowTeam::update_views()
{
	MainWindowBase::update_views();
	update_score_screen(); // TODO: should be an IView!

	UpdateFightNumber_();
	UpdateButtonText_();
}

void MainWindowTeam::update_club_views()
{
	QString oldHost = m_host;

	m_pUi->comboBox_club_host->clear();
	m_pUi->comboBox_club_home->clear();
	m_pUi->comboBox_club_guest->clear();

	for (int i = 0; i < m_pClubManager->ClubCount(); ++i)
	{
		Ipponboard::Club club;
		m_pClubManager->GetClub(i, club);
		QIcon icon(club.logoFile);
		m_pUi->comboBox_club_host->addItem(icon, club.name);
		m_pUi->comboBox_club_home->addItem(icon, club.name);
		m_pUi->comboBox_club_guest->addItem(icon, club.name);
	}

	m_host = oldHost;

	int index = m_pUi->comboBox_club_host->findText(m_host);

	if (-1 == index)
	{
		index = 0;
	}

	m_pUi->comboBox_club_host->setCurrentIndex(index);
	m_pUi->comboBox_club_home->setCurrentIndex(index);

	// set location from host
	m_pUi->lineEdit_location->setText(m_pClubManager->GetAddress(m_host));
}

void MainWindowTeam::UpdateFightNumber_()
{
	const int currentFight = m_pController->GetCurrentFight() + 1;

	const bool isSaved = m_pController->GetFight(
							 m_pController->GetCurrentRound(),
							 m_pController->GetCurrentFight()).is_saved;

	QString formatStr("%1 / %2");

	if (isSaved)
	{
		formatStr.append(tr(" (saved)"));
	}

	m_pUi->label_fight->setText(
		formatStr
		.arg(QString::number(currentFight))
		.arg(QString::number(m_pController->GetFightCount())));

	const int currentRound = m_pController->GetCurrentRound();

	if (currentRound == 0)
	{
		m_pUi->widget_currentRound->UpdateImage(":res/images/one_blue.png");
	}
	else
	{
		m_pUi->widget_currentRound->UpdateImage(":res/images/two_green.png");
	}
}

void MainWindowTeam::attach_primary_view()
{
	auto widget = dynamic_cast<QWidget*>(m_pPrimaryView.get());

	if (widget)
	{
		m_pUi->verticalLayout_3->insertWidget(0, widget, 0);
	}
}

void MainWindowTeam::retranslate_Ui()
{
	m_pUi->retranslateUi(this);
}

void MainWindowTeam::ui_check_language_items()
{
	m_pUi->actionLang_Deutsch->setChecked("de" == m_Language);
	m_pUi->actionLang_English->setChecked("en" == m_Language);
	m_pUi->actionLang_Dutch->setChecked("nl" == m_Language);

	// don't forget second implementation!
}

void MainWindowTeam::ui_check_show_secondary_view(bool checked) const
{
	m_pUi->actionShow_SecondaryView->setChecked(checked);
}

void MainWindowTeam::UpdateButtonText_()
{
	const bool isSaved = m_pController->GetFight(
							 m_pController->GetCurrentRound(),
							 m_pController->GetCurrentFight()).is_saved;

	const bool isLastFight =
		m_pController->GetCurrentFight() ==
		m_pController->GetFightCount() - 1
		&& m_pController->GetCurrentRound() ==
		m_pController->GetRoundCount() - 1;

	const bool isFirstFight = m_pController->GetCurrentFight() == 0
							  && m_pController->GetCurrentRound() == 0;

	QString textSave = tr("Save");
	QString textNext = tr("Next");

	m_pUi->button_next->setEnabled(true);
	m_pUi->button_prev->setEnabled(!isFirstFight);

	if (isLastFight)
	{
		m_pUi->button_next->setText(textSave);

		if (isSaved)
		{
			m_pUi->button_next->setEnabled(false);
		}
	}
	else
	{
		m_pUi->button_next->setText(textNext);
	}
}

void MainWindowTeam::update_score_screen()
{
	const QString home = m_pUi->comboBox_club_home->currentText();
	const QString guest = m_pUi->comboBox_club_guest->currentText();
	m_pScoreScreen->SetClubs(home, guest);
	const QString logo_home = m_pClubManager->GetLogo(home);
	const QString logo_guest = m_pClubManager->GetLogo(guest);
	m_pScoreScreen->SetLogos(logo_home, logo_guest);
	const int score_first = m_pController->GetTeamScore(Ipponboard::FighterEnum::First);
	const int score_second = m_pController->GetTeamScore(Ipponboard::FighterEnum::Second);
	m_pScoreScreen->SetScore(score_first, score_second);

	m_pScoreScreen->update();
}

QString MainWindowTeam::GetRoundDataAsHtml(const Fight& fight, int fightNo)
{
	// little helper to hide initial zeros for early print outs
	auto getNum = [&](int val)
	{
		return (!fight.is_saved && val == 0) ? QString() : QString::number(val);
	};

	auto getTime = [&](QString const & timeStr)
	{
		return !fight.is_saved ? QString() : timeStr;
	};

	auto first = FighterEnum::First;
	auto second = FighterEnum::Second;
	auto const& score_first = fight.GetScore1();
	auto const& score_second = fight.GetScore2();

	QString roundData("<tr>");

	roundData.append("<td><center>" + QString::number(fightNo + 1) + "</center></td>"); // number
	roundData.append("<td><center>" + fight.weight + "</center></td>"); // weight
	roundData.append("<td><center>" + fight.fighters[first].name + "</center></td>"); // name
	roundData.append("<td><center>" + getNum(score_first.Ippon()) + "</center></td>"); // I
	roundData.append("<td><center>" + getNum(score_first.Wazaari()) + "</center></td>"); // W
	roundData.append("<td><center>" + getNum(score_first.Yuko()) + "</center></td>"); // Y
	roundData.append("<td><center>" + getNum(score_first.Shido()) + "</center></td>"); // S
	roundData.append("<td><center>" + getNum(score_first.Hansokumake()) + "</center></td>"); // H
	roundData.append("<td><center>" + getNum(fight.HasWon(first)) + "</center></td>"); // won
	roundData.append("<td><center>" + getNum(fight.GetScorePoints(first)) + "</center></td>"); // score
	roundData.append("<td><center>" + fight.fighters[second].name + "</center></td>"); // name
	roundData.append("<td><center>" + getNum(score_second.Ippon()) + "</center></td>"); // I
	roundData.append("<td><center>" + getNum(score_second.Wazaari()) + "</center></td>"); // W
	roundData.append("<td><center>" + getNum(score_second.Yuko()) + "</center></td>"); // Y
	roundData.append("<td><center>" + getNum(score_second.Shido()) + "</center></td>"); // S
	roundData.append("<td><center>" + getNum(score_second.Hansokumake()) + "</center></td>"); // H
	roundData.append("<td><center>" + getNum(fight.HasWon(second)) + "</center></td>"); // won
	roundData.append("<td><center>" + getNum(fight.GetScorePoints(second)) + "</center></td>"); // score
	roundData.append("<td><center>" + getTime(fight.GetTimeRemainingString()) + "</center></td>"); // time
	roundData.append("<td><center>" + getTime(fight.GetTotalTimeElapsedString()) + "</center></td>"); // time
	roundData.append("</tr>\n");

	return roundData;
}

void MainWindowTeam::WriteScoreToHtml_()
{
	QString modeText = get_full_mode_title(m_currentMode);
	QString templateFile = get_template_file(m_currentMode);
	const QString filePath(
		fm::GetSettingsFilePath(templateFile.toStdString().c_str()).c_str());

	QFile file(filePath);

	if (!file.open(QFile::ReadOnly))
	{
		QMessageBox::critical(this, tr("File open error"),
							  tr("File could not be opened: ") + file.fileName());
		return;
	}

	QTextStream ts(&file);

	m_htmlScore = ts.readAll();
	file.close();

	m_htmlScore.replace("%TITLE%", modeText);

	m_htmlScore.replace("%HOST%", m_pUi->comboBox_club_host->currentText());
	m_htmlScore.replace("%DATE%", m_pUi->dateEdit->text());
	m_htmlScore.replace("%LOCATION%", m_pUi->lineEdit_location->text());
	m_htmlScore.replace("%HOME%", m_pUi->comboBox_club_home->currentText());
	m_htmlScore.replace("%GUEST%", m_pUi->comboBox_club_guest->currentText());

	// intermediate score
	auto wins1st = m_pController->GetTournamentScoreModel(0)->GetTotalWins();
	m_htmlScore.replace("%WINS_HOME%", QString::number(wins1st.first));
	m_htmlScore.replace("%WINS_GUEST%", QString::number(wins1st.second));
	auto score1st = m_pController->GetTournamentScoreModel(0)->GetTotalScore();
	m_htmlScore.replace("%SCORE_HOME%", QString::number(score1st.first));
	m_htmlScore.replace("%SCORE_GUEST%", QString::number(score1st.second));

	// final score
	auto wins2nd = m_pController->GetRoundCount() > 1 ?
				   m_pController->GetTournamentScoreModel(1)->GetTotalWins() : std::make_pair(0, 0);
	auto score2nd = m_pController->GetRoundCount() > 1 ?
					m_pController->GetTournamentScoreModel(1)->GetTotalScore() : std::make_pair(0, 0);
	auto totalWins = std::make_pair(wins1st.first + wins2nd.first, wins1st.second + wins2nd.second);
	auto totalScore = std::make_pair(score1st.first + score2nd.first, score1st.second + score2nd.second);

	m_htmlScore.replace("%TOTAL_WINS_HOME%", QString::number(totalWins.first));
	m_htmlScore.replace("%TOTAL_WINS_GUEST%", QString::number(totalWins.second));
	m_htmlScore.replace("%TOTAL_SCORE_HOME%", QString::number(totalScore.first));
	m_htmlScore.replace("%TOTAL_SCORE_GUEST%", QString::number(totalScore.second));

	QString winner = tr("tie");

	if (totalWins.first > totalWins.second)
	{
		winner = m_pUi->comboBox_club_home->currentText();
	}
	else if (totalWins.first < totalWins.second)
	{
		winner = m_pUi->comboBox_club_guest->currentText();
	}

	m_htmlScore.replace("%WINNER%", winner);

	// first round
	QString scoreData;

	for (int fightNo(0); fightNo < m_pController->GetFightCount(); ++fightNo)
	{
		const auto& fight = m_pController->GetFight(0, fightNo);
		scoreData.append(GetRoundDataAsHtml(fight, fightNo));
	}

	m_htmlScore.replace("%FIRST_ROUND%", scoreData);

	// second round
	scoreData.clear();

	for (int roundNo(1); roundNo < m_pController->GetRoundCount(); ++roundNo)
	{
		for (int fightNo(0); fightNo < m_pController->GetFightCount(); ++fightNo)
		{
			const auto& fight = m_pController->GetFight(roundNo, fightNo);
			scoreData.append(GetRoundDataAsHtml(fight, fightNo + m_pController->GetFightCount()));
		}
	}

	m_htmlScore.replace("%SECOND_ROUND%", scoreData);

	const QString copyright = tr("List generated with Ipponboard v") +
							  QApplication::applicationVersion() +
							  ", &copy; " + QApplication::organizationName() + ", 2010-" + VersionInfo::CopyrightYear;
	m_htmlScore.replace("</body>", "<small><center>" + copyright + "</center></small></body>");
}

void MainWindowTeam::on_actionReset_Scores_triggered()
{
	if (QMessageBox::warning(
				this,
				tr("Reset Scores"),
				tr("Really reset complete score table?"),
				QMessageBox::Yes | QMessageBox::No) == QMessageBox::Yes)
	{
		m_pController->ClearFightsAndResetTimers();
	}

	UpdateFightNumber_();
	UpdateButtonText_();
}

bool MainWindowTeam::EvaluateSpecificInput(const Gamepad* pGamepad)
{
	// back
	if (pGamepad->WasPressed(Gamepad::EButton(m_controllerCfg.button_prev)))
	{
		on_button_prev_clicked();
		// TODO: check: is UpdateViews_(); necessary here?
		// --> handle update views outside of this function
		return true;
	}
	// next
	else if (pGamepad->WasPressed(Gamepad::EButton(m_controllerCfg.button_next)))
	{
		on_button_next_clicked();
		// TODO: check: is UpdateViews_(); necessary here?
		// --> handle update views outside of this function
		return true;
	}

	return false;
}

void MainWindowTeam::on_tabWidget_currentChanged(int /*index*/)
{
	update_views();
}

void MainWindowTeam::on_actionManageModes_triggered()
{
	QStringList templates = get_list_templates();
	ModeManagerDlg dlg(m_modes, templates, m_currentMode, this);

	if (dlg.exec() == QDialog::Accepted)
	{
		QString errMsg;

		if (!Ipponboard::TournamentMode::WriteModes(MainWindowTeam::ModeConfigurationFileName(), dlg.Result(), errMsg))
		{
			QMessageBox::critical(this,
								  QCoreApplication::tr("Error writing mode configurations"),
								  errMsg);

			return;
		}

		m_pUi->comboBox_mode->clear();
		SetModes(dlg.Result());

		for (auto const & mode : m_modes)
		{
			m_pUi->comboBox_mode->addItem(mode.Description(), QVariant(mode.id));
		}

		auto pos = m_pUi->comboBox_mode->findData(QVariant(m_currentMode));
		pos = pos == -1 ? 0 : pos;

		if (pos != m_pUi->comboBox_mode->currentIndex())
		{
			m_pUi->comboBox_mode->setCurrentIndex(pos);
		}
		else
		{
			on_comboBox_mode_currentIndexChanged(pos);
		}
	}
}

void MainWindowTeam::on_actionManage_Clubs_triggered()
{
	ClubManagerDlg dlg(m_pClubManager, this);
	dlg.exec();
}

void MainWindowTeam::on_actionLoad_Demo_Data_triggered()
{
	//const QString modeBayernliga("bayernliga_m");

	//auto iter = std::find_if(begin(m_modes), end(m_modes),
	//	[&](TournamentMode const& mode)
	//{
	//	return mode.name == modeBayernliga;
	//});

	//if (iter == end(m_modes) || m_pUi->comboBox_mode->findText(iter->Description()) < 0)
	//{
	//	QMessageBox::critical(this, tr("Load demo data error"),
	//		tr("Tournament mode settings for [%1] could not be found.").arg(modeBayernliga));

	//	return;
	//}

	//int modeIndex = m_pUi->comboBox_mode->findText(iter->Description());
	//m_pUi->comboBox_mode->setCurrentIndex(modeIndex);

	//m_pController->ClearFights();																				//  Y  W  I  S  H  Y  W  I  S  H
	//m_pController->InitTournament(*iter);
	//update_weights("-90;+90;-73;-66;-81");
	//m_pController->SetFight(0, 0, "-90", "Sebastian Hölzl", "TG Landshut", "Oliver Sach", "TSV Königsbrunn",			3, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	//m_pController->SetFight(0, 1, "-90", "Stefan Grünert", "TG Landshut", "Marc Schäfer", "TSV Königsbrunn",			3, 2, 0, 0, 0, 0, 0, 0, 1, 0);
	//m_pController->SetFight(0, 2, "+90", "Andreas Neumaier", "TG Landshut", "Daniel Nussbächer", "TSV Königsbrunn",	0, 0, 0, 1, 0, 0, 0, 1, 1, 0);
	//m_pController->SetFight(0, 3, "+90", "Jürgen Neumeier", "TG Landshut", "Anderas Mayer", "TSV Königsbrunn",			1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	//m_pController->SetFight(0, 4, "-73", "Benny Mahl", "TG Landshut", "Christopher Benka", "TSV Königsbrunn"	,		2, 0, 1, 1, 0, 0, 0, 0, 3, 0);
	//m_pController->SetFight(0, 5, "-73", "Josef Sellmaier", "TG Landshut", "Jan-Michael König", "TSV Königsbrunn",		0, 1, 1, 0, 0, 0, 0, 0, 0, 0);
	//m_pController->SetFight(0, 6, "-66", "Alexander Keil", "TG Landshut", "Arthur Sipple", "TSV Königsbrunn",			2, 1, 1, 0, 0, 0, 0, 0, 0, 0);
	//m_pController->SetFight(0, 7, "-66", "Dominic Bogner", "TG Landshut", "Thomas Schaller", "TSV Königsbrunn",		0, 0, 1, 0, 0, 2, 0, 0, 0, 0);
	//m_pController->SetFight(0, 8, "-81", "Sebastian Schmieder", "TG Landshut", "Gerhard Wessinger", "TSV Königsbrunn",	0, 1, 1, 1, 0, 1, 0, 0, 0, 0);
	//m_pController->SetFight(0, 9, "-81", "Rainer Neumaier", "TG Landshut", "Georg Benka", "TSV Königsbrunn",			1, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	////  Y  W  I  S  H  Y  W  I  S  H
	//m_pController->SetFight(1, 0, "-90", "Sebastian Hölzl", "TG Landshut", "Marc Schäfer", "TSV Königsbrunn",		0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	//m_pController->SetFight(1, 1, "-90", "Stefan Grunert", "TG Landshut", "Florian Kürten", "TSV Königsbrunn",		0, 1, 1, 0, 0, 0, 0, 0, 0, 0);
	//m_pController->SetFight(1, 2, "+90", "Andreas Neumaier", "TG Landshut", "Andreas Mayer", "TSV Königsbrunn",	1, 2, 0, 0, 0, 0, 0, 0, 0, 0);
	//m_pController->SetFight(1, 3, "+90", "Jürgen Neumaier", "TG Landshut", "Daniel Nussbächer", "TSV Königsbrunn",	0, 0, 0, 2, 0, 0, 0, 1, 2, 0);
	//m_pController->SetFight(1, 4, "-73", "Matthias Feigl", "TG Landshut", "Jan-Michael König", "TSV Königsbrunn",	2, 1, 0, 1, 0, 0, 0, 0, 1, 0);
	//m_pController->SetFight(1, 5, "-73", "Josef Sellmaier", "TG Landshut", "Christopher Benka", "TSV Königsbrunn",	0, 0, 1, 0, 0, 0, 0, 0, 0, 0);
	//m_pController->SetFight(1, 6, "-66", "Jörg Herzog", "TG Landshut", "Thomas Schaller", "TSV Königsbrunn",		0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
	//m_pController->SetFight(1, 7, "-66", "Alex Selwitschka", "TG Landshut", "Jonas Allinger", "TSV Königsbrunn",	0, 1, 1, 0, 0, 1, 0, 0, 0, 0);
	//m_pController->SetFight(1, 8, "-81", "Eugen Makaritsch", "TG Landshut", "Georg Benka", "TSV Königsbrunn",		0, 0, 0, 0, 0, 0, 0, 1, 0, 0);
	//m_pController->SetFight(1, 9, "-81", "Rainer Neumaier", "TG Landshut", "Gerhard Wessinger", "TSV Königsbrunn",	0, 0, 1, 1, 0, 0, 0, 0, 0, 0);
	////m_pController->SetCurrentFight(0);

	//m_pUi->tableView_tournament_list1->viewport()->update();
	//m_pUi->tableView_tournament_list2->viewport()->update();
}

void MainWindowTeam::on_button_pause_clicked()
{
	if (m_pScoreScreen->isVisible())
	{
		m_pScoreScreen->hide();
		m_pUi->button_pause->setText(tr("Show results"));
	}
	else
	{
		update_score_screen();
		const int nScreens(QApplication::desktop()->numScreens());

		if (nScreens > 0 && nScreens > m_secondScreenNo)
		{
			// move to second screen
			QRect screenres =
				QApplication::desktop()->screenGeometry(m_secondScreenNo);
			m_pScoreScreen->move(QPoint(screenres.x(), screenres.y()));
		}

		if (m_secondScreenSize.isNull())
		{
			m_pScoreScreen->showFullScreen();
		}
		else
		{
			m_pScoreScreen->resize(m_secondScreenSize);
			m_pScoreScreen->show();
		}

		m_pUi->button_pause->setText(tr("Hide results"));
	}
}

void MainWindowTeam::on_button_prev_clicked()
{
	//if (0 == m_pController->GetCurrentFightIndex())
	//	return;

	m_pController->PrevFight();
	//m_pController->SetCurrentFight(m_pController->GetCurrentFightIndex() - 1);
}

void MainWindowTeam::on_button_next_clicked()
{
	/*
	if (m_pController->GetCurrentFightIndex() == m_pController->GetFightCount() - 1)
	{
		m_pController->SetCurrentFight(m_pController->GetCurrentFightIndex());
	}
	else
	{
		m_pController->SetCurrentFight(m_pController->GetCurrentFightIndex() + 1);
	}
	*/
	m_pController->NextFight();

	// reset osaekomi view (to reset active colors of previous fight)
    m_pController->DoAction(eAction_ResetOsaeKomi, FighterEnum::Nobody, true /*doRevoke*/);
}

void MainWindowTeam::on_comboBox_mode_currentIndexChanged(int i)
{
	if (!initialized)
	{
		return;
	}

	m_currentMode = m_pUi->comboBox_mode->itemData(i).toString();
	QString modeDescription = m_pUi->comboBox_mode->currentText();

	// FIXME2014: use this???
	//m_pController->SetOption(eOption_Use2013Rules, true);

	auto iter = std::find_if(begin(m_modes), end(m_modes), [&](TournamentMode const & mode)
	{
		return mode.id == m_currentMode;
	});

	if (iter != end(m_modes))
	{
		m_pController->InitTournament(*iter);
		update_weights(iter->weights); // TODO: don't set weights twice

		// disable "copy & switch" button if no duplicate weight classes are used (issue #42)
		if (iter->weightsAreDoubled)
		{
			m_pUi->pushButton_copySwitched->show();
		}
		else
		{
			m_pUi->pushButton_copySwitched->hide();
		}
	}
	else
	{
		Q_ASSERT("invalid mode");
	}

	// update table views
	m_pUi->tableView_tournament_list1->setModel(m_pController->GetTournamentScoreModel(0).get());
	m_pUi->tableView_tournament_list1->resizeColumnsToContents();

	m_pController->GetTournamentScoreModel(0)->SetExternalDisplays(
		m_pUi->lineEdit_wins_intermediate,
		m_pUi->lineEdit_score_intermediate);

	m_pUi->tableView_tournament_list1->selectRow(0);

	if (m_pController->GetRoundCount() == 1)
	{
		m_pUi->label_intermediate_result->setText(m_pUi->label_final_score->text()); //TODO: make this better!
		m_pUi->tableView_tournament_list2->hide();
		m_pUi->pushButton_copySwitched->hide();
		m_pUi->label_final_score->hide();
		m_pUi->label_final_wins->hide();
		m_pUi->label_final_sub_score->hide();
		m_pUi->lineEdit_score->hide();
		m_pUi->lineEdit_wins->hide();
	}
	else
	{
		m_pUi->tableView_tournament_list2->setModel(m_pController->GetTournamentScoreModel(1).get());
		m_pUi->tableView_tournament_list2->resizeColumnsToContents();
		m_pController->GetTournamentScoreModel(1)->SetExternalDisplays(
			m_pUi->lineEdit_wins,
			m_pUi->lineEdit_score);

		m_pController->GetTournamentScoreModel(1)->SetIntermediateModel(
			m_pController->GetTournamentScoreModel(0).get());

		m_pUi->tableView_tournament_list2->selectRow(0);

		m_pUi->tableView_tournament_list2->show();
		m_pUi->label_final_score->show();
		m_pUi->label_final_wins->show();
		m_pUi->label_final_sub_score->show();
		m_pUi->lineEdit_score->show();
		m_pUi->lineEdit_wins->show();
	}

	// set mode text as mat label
	m_MatLabel = modeDescription;
	m_pPrimaryView->SetMat(modeDescription);
	m_pSecondaryView->SetMat(modeDescription);

	m_pPrimaryView->UpdateView();
	m_pSecondaryView->UpdateView();

	UpdateFightNumber_();
}

void MainWindowTeam::on_comboBox_club_host_currentIndexChanged(const QString& s)
{
	m_host = s;

	// set location from host
	m_pUi->lineEdit_location->setText(m_pClubManager->GetAddress(m_host));
}

void MainWindowTeam::on_comboBox_club_home_currentIndexChanged(const QString& s)
{
	m_pController->SetClub(Ipponboard::FighterEnum::First, s);

#if 0
	ComboBoxDelegate* pCbx = dynamic_cast<ComboBoxDelegate*>
							 (m_pUi->tableView_tournament_list1->itemDelegateForColumn(TournamentModel::eCol_name1));

	if (pCbx)
	{
		pCbx->SetItems(m_fighterManager.GetClubFighterNames(s));
	}

#endif
	//UpdateViews_(); --> already done by controller
	update_score_screen();
}

void MainWindowTeam::on_comboBox_club_guest_currentIndexChanged(const QString& s)
{
	m_pController->SetClub(Ipponboard::FighterEnum::Second, s);
#if 0
	ComboBoxDelegate* pCbx = dynamic_cast<ComboBoxDelegate*>
							 (m_pUi->tableView_tournament_list1->itemDelegateForColumn(TournamentModel::eCol_name2));

	if (pCbx)
	{
		pCbx->SetItems(m_fighterManager.GetClubFighterNames(s));
	}

#endif
	//UpdateViews_(); --> already done by controller
	update_score_screen();
}

void MainWindowTeam::on_actionPrint_triggered()
{
	WriteScoreToHtml_();

	QPrinter printer(QPrinter::HighResolution);
	printer.setOrientation(QPrinter::Landscape);
	printer.setPaperSize(QPrinter::A4);
	printer.setPageMargins(15, 10, 15, 5, QPrinter::Millimeter);
	QPrintPreviewDialog preview(&printer, this);
	connect(&preview, SIGNAL(paintRequested(QPrinter*)), SLOT(Print(QPrinter*)));
	preview.exec();
}

void MainWindowTeam::on_actionExport_triggered()
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

		if (fileName.endsWith(".html"))
		{
			QFile html(fileName);

			if (html.open(QFile::WriteOnly))
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
			QTextEdit edit(m_htmlScore, this);
			edit.document()->print(&printer);
		}

		QApplication::restoreOverrideCursor();
	}
}

void MainWindowTeam::on_button_golden_score_toggled(bool toggled)
{
	m_pController->SetGoldenScore(toggled);
}

void MainWindowTeam::on_toolButton_weights_pressed()
{
	bool ok(false);
	const QString weights = QInputDialog::getText(
								this,
								tr("Set Weights"),
								tr("Set weights (separated by ';'):"),
								QLineEdit::Normal,
								m_weights,
								&ok);

	if (ok)
	{
		if (m_pController->GetFightCount() / 2 - 1 != weights.count(';')
				&& m_pController->GetFightCount() - 1 != weights.count(';'))
		{
			QMessageBox::critical(this, "Wrong values",
								  tr("You need to specify %1 weight classes separated by ';'!")
								  .arg(QString::number(m_pController->GetFightCount())));
			on_toolButton_weights_pressed();
		}
		else
		{
			update_weights(weights);
		}
	}
}

void MainWindowTeam::on_toolButton_team_home_pressed()
{
#if 0
	MainWindowBase::on_actionManageFighters_triggered();
	const QString club = m_pUi->comboBox_club_home->currentText();

	FighterManagerDlg dlg(m_fighterManager, this);
	dlg.SetFilter(FighterManagerDlg::eColumn_club, club);
	dlg.exec();

	ComboBoxDelegate* pCbx = dynamic_cast<ComboBoxDelegate*>(
								 m_pUi->tableView_tournament_list1->
								 itemDelegateForColumn(TournamentModel::eCol_name1));

	if (pCbx)
	{
		pCbx->SetItems(m_fighterManager.GetClubFighterNames(club));
	}

#endif
}

void MainWindowTeam::on_toolButton_team_guest_pressed()
{
#if 0
	MainWindowBase::on_actionManageFighters_triggered();
	const QString club = m_pUi->comboBox_club_guest->currentText();

	FighterManagerDlg dlg(m_fighterManager, this);
	dlg.SetFilter(FighterManagerDlg::eColumn_club, club);
	dlg.exec();

	auto pCbx = dynamic_cast<ComboBoxDelegate*>(
					m_pUi->tableView_tournament_list2->
					itemDelegateForColumn(TournamentModel::eCol_name2));

	if (pCbx)
	{
		pCbx->SetItems(m_fighterManager.GetClubFighterNames(club));
	}

#endif
}

void MainWindowTeam::update_weights(QString const& weightString)
{
	m_weights = weightString;
	m_pController->SetWeights(weightString.split(';'));
}

void MainWindowTeam::on_pushButton_copySwitched_pressed()
{
	m_pController->CopyAndSwitchGuestFighters();
}

void MainWindowTeam::on_actionSet_Round_Time_triggered()
{
	bool ok(false);

	auto timeStr = QInputDialog::getText(
					   this,
					   tr("Set Value"),
					   tr("Set value to (m:ss):"),
					   QLineEdit::Normal,
					   m_pController->GetFightTimeString(),
					   &ok);

	if (ok)
	{
		m_pController->SetRoundTime(timeStr);
	}
}

void MainWindowTeam::on_actionScore_Screen_triggered()
{
	m_pUi->tabWidget->setCurrentWidget(m_pUi->tab_score_table);
}

void MainWindowTeam::on_actionScore_Control_triggered()
{
	m_pUi->tabWidget->setCurrentWidget(m_pUi->tab_view);
}

void MainWindowTeam::on_tableView_customContextMenuRequested(
	QTableView* pTableView,
	QPoint const& pos,
	const char* copySlot,
	const char* pasteSlot,
	const char* clearSlot)
{
	QMenu menu;
	QModelIndex index = pTableView->indexAt(pos);
	index = index.sibling(index.row(), 0);

	QModelIndexList selection =
		pTableView->selectionModel()->selectedIndexes();

	if (selection.empty())
	{
		Q_ASSERT(!"empty selection");
		return;
	}

	// Do not allow copy if different columns are selected
	bool copyAllowed(true);

	if (selection.size() > 1)
	{
		for (int i(0); i < selection.size() - 1; ++i)
		{
			if (selection[i].column() != selection[i + 1].column())
			{
				copyAllowed = false;
				break;
			}
		}
	}

	// Paste is only allowed for the name cells
	// and if the clipboard is not empty
	const bool pasteAllowed = (selection[0].column() == TournamentModel::eCol_name1
							   || selection[0].column() == TournamentModel::eCol_name2)
							  && !QApplication::clipboard()->text().isEmpty();

	const bool clearAllowed = copyAllowed;

	if (index.isValid())
	{
		QIcon copyIcon(":/res/icons/copy_cells.png");
		QIcon pasteIcon(":/res/icons/paste.png");
		QIcon clearIcon(":/res/icons/clear_cells.png");
		QAction* pAction = nullptr;
		pAction = menu.addAction(copyIcon, tr("Copy"), this, copySlot, QKeySequence::Copy);
		pAction->setDisabled(!copyAllowed);

		pAction = menu.addAction(pasteIcon, tr("Paste"), this, pasteSlot, QKeySequence::Paste);
		pAction->setDisabled(!pasteAllowed);

		pAction = menu.addAction(clearIcon, tr("Clear"), this, clearSlot, QKeySequence::Delete);
		pAction->setDisabled(!clearAllowed);

		menu.exec(QCursor::pos());
	}
}

void MainWindowTeam::on_tableView_tournament_list1_customContextMenuRequested(QPoint const& pos)
{
	on_tableView_customContextMenuRequested(
		m_pUi->tableView_tournament_list1,
		pos,
		SLOT(slot_copy_cell_content_list1()),
		SLOT(slot_paste_cell_content_list1()),
		SLOT(slot_clear_cell_content_list1()));
}

void MainWindowTeam::on_tableView_tournament_list2_customContextMenuRequested(QPoint const& pos)
{
	on_tableView_customContextMenuRequested(
		m_pUi->tableView_tournament_list2,
		pos,
		SLOT(slot_copy_cell_content_list2()),
		SLOT(slot_paste_cell_content_list2()),
		SLOT(slot_clear_cell_content_list2()));
}

void MainWindowTeam::copy_cell_content(QTableView* pTableView)
{
	QModelIndexList selection = pTableView->selectionModel()->selectedIndexes();
	std::sort(selection.begin(), selection.end());

	// Copy is only allowed for single column selection
	for (int i(0); i < selection.size() - 1; ++i)
	{
		if (selection[i].column() != selection[i + 1].column())
		{
			QApplication::clipboard()->clear();
			return;
		}
	}

	QString selectedText;

	for (QModelIndex index : selection)
	{
		auto data = pTableView->model()->data(index, Qt::DisplayRole);
		selectedText += data.toString() + '\n';
	}

	if (!selectedText.isEmpty())
	{
		selectedText.truncate(selectedText.lastIndexOf('\n'));  // remove last '\n'
		QApplication::clipboard()->setText(selectedText);
	}
}

void MainWindowTeam::paste_cell_content(QTableView* pTableView)
{
	if (QApplication::clipboard()->text().isEmpty())
	{
		QMessageBox::warning(this, QApplication::applicationName(),
							 tr("There is nothing to paste!"));
		return;
	}

	auto lines = QApplication::clipboard()->text().split('\n');

	QModelIndexList selection = pTableView->selectionModel()->selectedIndexes();

	if (selection.empty())
	{
		QMessageBox::critical(this, QApplication::applicationName(),
							  tr("Can not paste into an empty selection!"));
		return;
	}

	std::sort(selection.begin(), selection.end());

	if (lines.size() < selection.size())
	{
		QMessageBox::critical(this, QApplication::applicationName(),
							  tr("There is too few data for the selection in the clipboard!"));
		return;
	}

	if (lines.size() > selection.size())
	{
		// extend selection to maximum possible
		QModelIndex index = selection.back();
		const int nRows = pTableView->model()->rowCount();

		while (index.row() < nRows &&
				index.isValid() &&
				lines.size() > selection.size())
		{
			index = pTableView->model()->index(
						index.row() + 1, index.column());
			selection.push_back(index);
			pTableView->selectionModel()->select(index, QItemSelectionModel::Select);
		}

		if (lines.size() < selection.size())
		{
			QMessageBox::warning(this, QApplication::applicationName(),
								 tr("There is more data available in the clipboard as could be pasted!"));
		}
	}

	auto lineNo = 0;

	for (QModelIndex index : selection)
	{
		if (index.column() == TournamentModel::eCol_name1 ||
				index.column() == TournamentModel::eCol_name2)
		{
			pTableView->model()->setData(
				index, lines[lineNo], Qt::EditRole);
			++lineNo;
		}
	}
}

void MainWindowTeam::clear_cell_content(QTableView* pTableView)
{
	QModelIndexList selection = pTableView->selectionModel()->selectedIndexes();
	std::sort(selection.begin(), selection.end());

	// Clear is only allowed for single column selection
	for (int i(0); i < selection.size() - 1; ++i)
	{
		if (selection[i].column() != selection[i + 1].column())
		{
			QApplication::clipboard()->clear();
			return;
		}
	}

	for (QModelIndex index : selection)
	{
		pTableView->model()->setData(index, "", Qt::EditRole);
	}
}

void MainWindowTeam::slot_copy_cell_content_list1()
{
	copy_cell_content(m_pUi->tableView_tournament_list1);
}

void MainWindowTeam::slot_copy_cell_content_list2()
{
	copy_cell_content(m_pUi->tableView_tournament_list2);
}

void MainWindowTeam::slot_paste_cell_content_list1()
{
	paste_cell_content(m_pUi->tableView_tournament_list1);
}

void MainWindowTeam::slot_paste_cell_content_list2()
{
	paste_cell_content(m_pUi->tableView_tournament_list2);
}

void MainWindowTeam::slot_clear_cell_content_list1()
{
	clear_cell_content(m_pUi->tableView_tournament_list1);
}

void MainWindowTeam::slot_clear_cell_content_list2()
{
	clear_cell_content(m_pUi->tableView_tournament_list2);
}

void MainWindowTeam::Print(QPrinter* p)
{
	QTextEdit e(m_htmlScore, this);
	e.document()->print(p);
}

QString MainWindowTeam::get_template_file(QString const& modeId) const
{
	// TODO: use binary seach as the container is already sorted
	auto iter = std::find_if(begin(m_modes), end(m_modes), [&](TournamentMode const & m)
	{
		return m.id == modeId;
	});

	if (iter != end(m_modes))
	{
		return QString("%1/%2").arg(TournamentMode::TemplateDirName(), iter->listTemplate);
	}

	return QString();
}

QString MainWindowTeam::get_full_mode_title(QString const& modeId) const
{
	QString year(QString::number(QDate::currentDate().year()));

	// TODO: use binary seach as the container is already sorted
	auto iter = std::find_if(begin(m_modes), end(m_modes), [&](TournamentMode const & tm)
	{
		return tm.id == modeId;
	});

	if (iter != end(m_modes))
	{
		if (iter->subTitle.isEmpty())
		{
			return QString("%1 %2").arg(iter->title, year);
		}
		else
		{
			return QString("%1 %2 - %3").arg(iter->title, year, iter->subTitle);
		}
	}

	return tr("Ipponboard fight list %1").arg(year);
}
