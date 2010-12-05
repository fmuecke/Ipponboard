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
	const char* const str_ini_name = "IpponBoardT.ini";
#else
	const char* const str_ini_name = "IpponBoard.ini";
#endif

const char* const str_golden_score = "Golden Score";
const char* const str_normal_round_time = "Normal";
const char* const str_tag_Main = "Main";
const char* const str_tag_size = "size";
const char* const str_tag_pos = "pos";
const char* const str_tag_SecondScreen = "SecondScreen";
const char* const str_tag_SecondScreenSize = "SecondScreenSize";
const char* const str_tag_AutoSize = "AutoSize";
const char* const str_tag_AlwaysShow = "AlwaysShow";
const char* const str_tag_Fonts = "Fonts";
const char* const str_tag_TextFont1 = "TextFont1";
const char* const str_tag_DigitFont = "DigitFont";
const char* const str_tag_Colors = "Colors";
const char* const str_tag_InfoTextColor = "InfoTextColor";
const char* const str_tag_InfoTextBgColor = "InfoTextBgColor";
const char* const str_tag_TextColorBlue = "TextColorBlue";
const char* const str_tag_TextBgColorBlue = "TextBgColorBlue";
const char* const str_tag_TextColorWhite = "TextColorWhite";
const char* const str_tag_TextBgColorWhite = "TextBgColorWhite";
const char* const str_tag_MainClockColorRunning = "MainClockColorRunning";
const char* const str_tag_MainClockColorStopped = "MainClockColorStopped";
const char* const str_tag_Styles = "Styles";
const char* const str_tag_BgStyle = "BackgroundStyle";
const char* const str_tag_Input = "Input";
const char* const str_tag_buttonHajimeMatte = "ButtonHajimeMatte";
const char* const str_tag_buttonOsaekomiToketa = "ButtonOsaekomiToketa";
const char* const str_tag_buttonNext = "ButtonNext";
const char* const str_tag_buttonPrev = "ButtonPrev";
const char* const str_tag_buttonPause = "ButtonPause";
const char* const str_tag_buttonReset = "ButtonReset";
const char* const str_tag_buttonReset2 = "ButtonReset2";
const char* const str_tag_buttonResetHoldBlue = "ButtonResetHoldBlue";
const char* const str_tag_buttonResetHoldWhite = "ButtonResetHoldWhite";
const char* const str_tag_buttonBlueHolding = "ButtonBlueHolding";
const char* const str_tag_buttonWhiteHolding = "ButtonWhiteHolding";
const char* const str_tag_buttonHansokumakeBlue = "ButtonHansokumakeBlue";
const char* const str_tag_buttonHansokumakeWhite = "ButtonHansokumakeWhite";
const char* const str_tag_invertX = "InvertX";
const char* const str_tag_invertY = "InvertY";
const char* const str_tag_invertR = "InvertR";
const char* const str_tag_invertZ = "InvertZ";
const char* const str_tag_Sounds = "Sounds";
const char* const str_tag_sound_time_ends = "TimeEnds";


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
	void changeLang_(QString const& langStr);
	void updateLangCheckStates_() const;

#ifdef TEAM_VIEW
	void UpdateMatchNumber_();
	void UpdateScoreScreen_();
	void WriteScoreToHtml_();
#endif

private slots:
	void on_actionEnglish_triggered();
 void on_actionDeutsch_triggered();
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
