#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include <vector>
#include <boost/shared_ptr.hpp>

// forwards
namespace Ui {
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

namespace FMlib {
	class Gamepad;
}
typedef boost::shared_ptr<FMlib::Gamepad> PGamePad;

#ifdef TEAM_VIEW
	static const char* const str_ini_name = "IpponBoardT.ini";
#else
	static const char* const str_ini_name = "IpponBoard.ini";
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
static const char* const str_tag_Language = "Language";
static const char* const str_tag_Fonts = "Fonts";
static const char* const str_tag_TextFont1 = "TextFont1";
static const char* const str_tag_DigitFont = "DigitFont";
static const char* const str_tag_Colors = "Colors";
static const char* const str_tag_InfoTextColor = "InfoTextColor";
static const char* const str_tag_InfoTextBgColor = "InfoTextBgColor";
static const char* const str_tag_TextColorBlue = "TextColorBlue";
static const char* const str_tag_TextBgColorBlue = "TextBgColorBlue";
static const char* const str_tag_TextColorWhite = "TextColorWhite";
static const char* const str_tag_TextBgColorWhite = "TextBgColorWhite";
static const char* const str_tag_MainClockColorRunning = "MainClockColorRunning";
static const char* const str_tag_MainClockColorStopped = "MainClockColorStopped";
static const char* const str_tag_Styles = "Styles";
static const char* const str_tag_BgStyle = "BackgroundStyle";
static const char* const str_tag_Input = "Input";
static const char* const str_tag_buttonHajimeMatte = "ButtonHajimeMatte";
static const char* const str_tag_buttonOsaekomiToketa = "ButtonOsaekomiToketa";
static const char* const str_tag_buttonNext = "ButtonNext";
static const char* const str_tag_buttonPrev = "ButtonPrev";
static const char* const str_tag_buttonPause = "ButtonPause";
static const char* const str_tag_buttonReset = "ButtonReset";
static const char* const str_tag_buttonReset2 = "ButtonReset2";
static const char* const str_tag_buttonResetHoldBlue = "ButtonResetHoldBlue";
static const char* const str_tag_buttonResetHoldWhite = "ButtonResetHoldWhite";
static const char* const str_tag_buttonBlueHolding = "ButtonBlueHolding";
static const char* const str_tag_buttonWhiteHolding = "ButtonWhiteHolding";
static const char* const str_tag_buttonHansokumakeBlue = "ButtonHansokumakeBlue";
static const char* const str_tag_buttonHansokumakeWhite = "ButtonHansokumakeWhite";
static const char* const str_tag_invertX = "InvertX";
static const char* const str_tag_invertY = "InvertY";
static const char* const str_tag_invertR = "InvertR";
static const char* const str_tag_invertZ = "InvertZ";
static const char* const str_tag_Sounds = "Sounds";
static const char* const str_tag_sound_time_ends = "TimeEnds";

class MainWindow : public QMainWindow {
	Q_OBJECT
public:
	explicit MainWindow( QWidget *parent = 0 );
	virtual ~MainWindow();

protected:
	void changeEvent( QEvent *e );
	void closeEvent( QCloseEvent *event );

private:
	void WriteSettings_();
	void ReadSettings_();
	void UpdateInfoTextColor_(const QColor& color, const QColor& bgColor);
	void UpdateTextColorBlue_(const QColor& color, const QColor& bgColor);
	void UpdateTextColorWhite_(const QColor& color, const QColor& bgColor);
	void UpdateTextFont_(const QFont&);
	void ShowHideView_() const;
	void UpdateViews_();
	void change_lang(bool beQuiet = false);
#ifdef TEAM_VIEW
	void UpdateFightNumber_();
	void UpdateScoreScreen_();
	void WriteScoreToHtml_();
#endif

private slots:
	void on_action_Info_Header_triggered(bool checked);
	void on_actionLang_English_triggered(bool );
	void on_actionLang_Deutsch_triggered(bool );
	void on_actionContact_Author_triggered();
	void on_actionOnline_Feedback_triggered();
	void on_actionVisit_Project_Homepage_triggered();
	void on_actionAbout_Ipponboard_triggered();
	void on_actionTest_Gong_triggered();
	void on_actionShow_SecondaryView_triggered();
//	void on_actionSelect_Color_triggered();
//	void on_actionChange_Background_triggered();
	void on_actionReset_Scores_triggered();
	void on_actionPreferences_triggered();
	void on_button_reset_clicked();
	void EvaluateInput();

#ifdef TEAM_VIEW
	void Print(QPrinter *p)
	{
		QTextEdit e(m_htmlScore, this);
		e.document()->print(p);
	}
	void on_tabWidget_currentChanged(int index);
	void on_actionManage_Clubs_triggered();
	void on_actionLoad_Demo_Data_triggered();
	void on_button_pause_clicked();
	void on_button_prev_clicked();
	void on_button_next_clicked();
	void on_comboBox_club_home_currentIndexChanged(const QString& s);
	void on_comboBox_club_guest_currentIndexChanged(const QString& s);
	void on_comboBox_round_currentIndexChanged(const QString& s);
	void on_actionPrint_triggered();
	void on_actionExport_triggered();
#else
	void on_actionManage_Classes_triggered();
	void on_comboBox_mat_currentIndexChanged(const QString& );
	void on_comboBox_weight_currentIndexChanged(const QString& );
	void on_lineEdit_name_white_textChanged(const QString& );
	void on_lineEdit_name_blue_textChanged(const QString& );
	void on_comboBox_time_currentIndexChanged(const QString& );
	void on_comboBox_weight_class_currentIndexChanged(const QString& );
#endif

private:
	Ui::MainWindow* m_pUi;
	Ipponboard::View* m_pPrimaryView;
	Ipponboard::View* m_pSecondaryView;
	Ipponboard::Controller* m_pController;
#ifdef TEAM_VIEW
	Ipponboard::ScoreScreen* m_pScoreScreen;
	Ipponboard::ClubManager* m_pClubManager;
	std::vector<QTableWidgetItem> fighters_home;
	std::vector<QTableWidgetItem> fighters_guest;
	QString m_htmlScore;
#else
	Ipponboard::FightCategoryMgr* m_pCategoryManager;
#endif
	PGamePad m_pGamePad;

	QFont m_textFont;
	int m_secondScreenNo;
	bool m_bAlwaysShow;
	bool m_bAutoSize;
	QSize m_secondScreenSize;
	QString m_Language;

	int m_buttonHajimeMatte;
	int m_buttonOsaekomiToketa;
	int m_buttonNext;
	int m_buttonPrev;
	int m_buttonPause;
	int m_buttonReset;
	int m_buttonReset2;
	int m_buttonResetHoldBlue;
	int m_buttonResetHoldWhite;
	int m_buttonBlueHolding;
	int m_buttonWhiteHolding;
	int m_buttonHansokumakeBlue;
	int m_buttonHansokumakeWhite;
};

#endif // MAINWINDOW_H
