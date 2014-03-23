#ifndef BASE__MAINWINDOWBASE_H_
#define BASE__MAINWINDOWBASE_H_

#include <QMainWindow>
#include <QTableWidgetItem>
#include <vector>
#include <boost/shared_ptr.hpp>
#include "../core/ControllerConfig.h"

// forwards
namespace Ui
{
class MainWindow;
class Frame_Clubs;
}
namespace Ipponboard
{
class View;
class Controller;
class ClubManager;
class FightCategoryMgr;
class ScoreScreen;
}

namespace FMlib
{
class Gamepad;
}
typedef boost::shared_ptr<FMlib::Gamepad> PGamePad;

#ifdef TEAM_VIEW
static const char* const str_ini_name = "IpponboardT.ini";
static const char* const str_mode_1te_bundesliga_nord_m = "1. Bundesliga Nord (Männer)";
static const char* const str_mode_1te_bundesliga_sued_m = "1. Bundesliga Süd (Männer)";
static const char* const str_mode_2te_bundesliga_nord_m = "2. Bundesliga Nord (Männer)";
static const char* const str_mode_2te_bundesliga_sued_m = "2. Bundesliga Süd (Männer)";
static const char* const str_mode_1te_bundesliga_nord_f = "1. Bundesliga Nord (Frauen)";
static const char* const str_mode_1te_bundesliga_sued_f = "1. Bundesliga Süd (Frauen)";
static const char* const str_mode_2te_bundesliga_nord_f = "2. Bundesliga Nord (Frauen)";
static const char* const str_mode_2te_bundesliga_sued_f = "2. Bundesliga Süd (Frauen)";
static const char* const str_mode_bayernliga_nord_m = "Bayernliga Nord (M)";
static const char* const str_mode_bayernliga_sued_m = "Bayernliga Süd (M)";
static const char* const str_mode_bayernliga_nord_f = "Bayernliga Nord (F)";
static const char* const str_mode_bayernliga_sued_f = "Bayernliga Süd (F)";
static const char* const str_mode_landesliga_nord_m = "Landesliga Nord (M)";
static const char* const str_mode_landesliga_sued_m = "Landesliga Süd (M)";
static const char* const str_mode_landesliga_nord_f = "Landesliga Nord (F)";
static const char* const str_mode_landesliga_sued_f = "Landesliga Süd (F)";
static const char* const str_mode_mm_u17_m = "Deutsche VMM MU17";
static const char* const str_mode_mm_u17_f = "Deutsche VMM FU17";
#else
static const char* const str_ini_name = "Ipponboard.ini";
#endif

static const char* const str_golden_score = "Golden Score";
static const char* const str_normal_round_time = "Normal";
static const char* const str_tag_Main = "Main";
static const char* const str_tag_size = "size";
static const char* const str_tag_pos = "pos";
static const char* const str_tag_SecondScreen = "SecondScreen";
static const char* const str_tag_SecondScreenSize = "SecondScreenSize";
static const char* const str_tag_AutoSize = "AutoSize";
static const char* const str_tag_AlwaysShow = "AlwaysShow";
static const char* const str_tag_MatLabel = "MatLabel";
static const char* const str_tag_LabelHome = "LabelHome";
static const char* const str_tag_LabelGuest = "LabelGuest";
static const char* const str_tag_Language = "Language";
static const char* const str_tag_Version = "Version";
static const char* const str_tag_AutoIncrementPoints = "AutoIncrementPoints";
static const char* const str_tag_Use2013Rules = "Use2013Rules";
#ifdef TEAM_VIEW
static const char* const str_tag_Mode = "Mode";
static const char* const str_tag_Host = "Host";
#endif
static const char* const str_tag_Fonts = "Fonts";
static const char* const str_tag_TextFont1 = "TextFont1";
static const char* const str_tag_FighterNameFont = "FighterNameFont";
static const char* const str_tag_DigitFont = "DigitFont";
static const char* const str_tag_Colors = "Colors";
static const char* const str_tag_InfoTextColor = "InfoTextColor";
static const char* const str_tag_InfoTextBgColor = "InfoTextBgColor";
static const char* const str_tag_TextColorFirst = "TextColorFirst";
static const char* const str_tag_TextBgColorFirst = "TextBgColorFirst";
static const char* const str_tag_TextColorSecond = "TextColorSecond";
static const char* const str_tag_TextBgColorSecond = "TextBgColorSecond";
static const char* const str_tag_MainClockColorRunning = "MainClockColorRunning";
static const char* const str_tag_MainClockColorStopped = "MainClockColorStopped";
static const char* const str_tag_Styles = "Styles";
static const char* const str_tag_BgStyle = "BackgroundStyle";
static const char* const str_tag_Input = "Input";
static const char* const str_tag_buttonHajimeMate = "ButtonHajimeMate";
static const char* const str_tag_buttonOsaekomiToketa = "ButtonOsaekomiToketa";
static const char* const str_tag_buttonNext = "ButtonNext";
static const char* const str_tag_buttonPrev = "ButtonPrev";
static const char* const str_tag_buttonPause = "ButtonPause";
static const char* const str_tag_buttonReset = "ButtonReset";
static const char* const str_tag_buttonReset2 = "ButtonReset2";
static const char* const str_tag_buttonResetHoldFirst = "ButtonResetHoldFirst";
static const char* const str_tag_buttonResetHoldSecond = "ButtonResetHoldSecond";
static const char* const str_tag_buttonFirstHolding = "ButtonFirstHolding";
static const char* const str_tag_buttonSecondHolding = "ButtonSecondHolding";
static const char* const str_tag_buttonHansokumakeFirst = "ButtonHansokumakeFirst";
static const char* const str_tag_buttonHansokumakeSecond = "ButtonHansokumakeSecond";
static const char* const str_tag_invertX = "InvertX";
static const char* const str_tag_invertY = "InvertY";
static const char* const str_tag_invertR = "InvertR";
static const char* const str_tag_invertZ = "InvertZ";
static const char* const str_tag_Sounds = "Sounds";
static const char* const str_tag_sound_time_ends = "TimeEnds";

class MainWindowBase : public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindowBase(QWidget* parent = 0);
	virtual ~MainWindowBase();

protected:
	void changeEvent(QEvent* e);
	void closeEvent(QCloseEvent* event);
	void keyPressEvent(QKeyEvent* event);

private:
	void write_settings();
	void read_settings();
	void update_info_text_color(const QColor& color, const QColor& bgColor);
	void update_text_color_first(const QColor& color, const QColor& bgColor);
	void update_text_color_second(const QColor& color, const QColor& bgColor);
	void update_fighter_name_font(const QFont&);
	void show_hide_view() const;
	void update_views();
	void change_lang(bool beQuiet = false);
#ifdef TEAM_VIEW
	void update_club_views();
	void UpdateFightNumber_();
	void UpdateScoreScreen_();
	void WriteScoreToHtml_();
#endif

private slots:
#ifdef TEAM_VIEW
	void on_tableView_tournament_list1_customContextMenuRequested(QPoint const& pos);
	void on_tableView_tournament_list2_customContextMenuRequested(QPoint const& pos);
	void on_actionScore_Control_triggered();
	void on_actionScore_Screen_triggered();
	void on_button_pause_clicked();
	void on_button_current_round_clicked(bool checked);
	void on_actionSet_Round_Time_triggered();
	void on_pushButton_copySwitched_pressed();
	void on_toolButton_weights_pressed();
	void slot_copy_cell_content_list1();
	void slot_copy_cell_content_list2();
	void slot_paste_cell_content_list1();
	void slot_paste_cell_content_list2();
	void slot_clear_cell_content_list1();
	void slot_clear_cell_content_list2();
	void Print(QPrinter* p);
	void on_tabWidget_currentChanged(int index);
	void on_actionManage_Clubs_triggered();
	void on_actionLoad_Demo_Data_triggered();
	void on_button_prev_clicked();
	void on_button_next_clicked();
	void on_comboBox_mode_currentIndexChanged(const QString& s);
	void on_comboBox_club_host_currentIndexChanged(const QString& s);
	void on_comboBox_club_home_currentIndexChanged(const QString& s);
	void on_comboBox_club_guest_currentIndexChanged(const QString& s);
	void on_actionPrint_triggered();
	void on_actionExport_triggered();
#else
	void on_actionManage_Classes_triggered();
	void on_comboBox_weight_currentIndexChanged(const QString&);
	void on_lineEdit_name_second_textChanged(const QString&);
	void on_lineEdit_name_first_textChanged(const QString&);
	void on_checkBox_golden_score_clicked(bool checked);
	void on_comboBox_weight_class_currentIndexChanged(const QString&);
#endif
	void on_actionSet_Main_Timer_triggered();
	void on_actionSet_Hold_Timer_triggered();
	void on_action_Info_Header_triggered(bool checked);
	void on_actionLang_English_triggered(bool);
	void on_actionLang_Deutsch_triggered(bool);
	void on_actionContact_Author_triggered();
	void on_actionOnline_Feedback_triggered();
	void on_actionVisit_Project_Homepage_triggered();
	void on_actionAbout_Ipponboard_triggered();
	void on_actionTest_Gong_triggered();
	void on_actionShow_SecondaryView_triggered();
	void on_actionReset_Scores_triggered();
	void on_actionPreferences_triggered();
	void on_button_reset_clicked();
	void EvaluateInput();

	void on_checkBox_use2013rules_toggled(bool checked);

	void on_checkBox_autoIncrement_toggled(bool checked);

private:
	void update_statebar();
	Ui::MainWindow* m_pUi;
	Ipponboard::View* m_pPrimaryView;
	Ipponboard::View* m_pSecondaryView;
	Ipponboard::Controller* m_pController;
#ifdef TEAM_VIEW
	void update_weights(QString weightString);
	void on_tableView_customContextMenuRequested(QTableView* pTableView,
			QPoint const& pos,
			const char* copySlot,
			const char* pasteSlot,
			const char* clearSlot);
	void copy_cell_content(QTableView* pTableView);
	void paste_cell_content(QTableView* pTableView);
	void clear_cell_content(QTableView* pTableView);
	static QString get_template_file(QString const& mode);
	static QString get_full_mode_title(QString const& mode);

	Ipponboard::ScoreScreen* m_pScoreScreen;
	Ipponboard::ClubManager* m_pClubManager;
	std::vector<QTableWidgetItem> fighters_home;
	std::vector<QTableWidgetItem> fighters_guest;
	QString m_htmlScore;
	QString m_mode;
	QString m_host;
#else
	Ipponboard::FightCategoryMgr* m_pCategoryManager;
#endif
	QString m_MatLabel;
	PGamePad m_pGamePad;

	QFont m_FighterNameFont;
	int m_secondScreenNo;
	bool m_bAlwaysShow;
	bool m_bAutoSize;
	QSize m_secondScreenSize;
	QString m_Language;
	QString m_weights;

	Ipponboard::ControllerConfig m_controllerCfg;
};

#endif  // BASE__MAINWINDOW_H_
