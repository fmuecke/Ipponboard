#ifndef BASE__MAINWINDOW_BASE_H_
#define BASE__MAINWINDOW_BASE_H_

#include "../core/iView.h"
#include "../core/EditionType.h"
#include "FighterManager.h"
#include "../core/ControllerConfig.h"
#include "../util/helpers.hpp"

#include <QMainWindow>
#include <vector>
#include <memory>

// forwards
namespace Ui
{
//class MainWindow;
//class Frame_Clubs;
}
namespace Ipponboard
{
class Fighter;
class View;
class Controller;
class ClubManager;
class FightCategoryMgr;
class ScoreScreen;
}
class QSettings;

namespace FMlib
{
class Gamepad;
}
typedef std::shared_ptr<FMlib::Gamepad> PGamepad;

static const char* const str_golden_score = "Golden Score";
static const char* const str_normal_round_time = "Normal";
static const char* const str_tag_Main = "Main";
static const char* const str_tag_size = "size";
static const char* const str_tag_pos = "pos";
static const char* const str_tag_SecondScreen = "SecondScreen";
static const char* const str_tag_SecondScreenSize = "SecondScreenSize";
static const char* const str_tag_MatLabel = "MatLabel";
static const char* const str_tag_LabelHome = "LabelHome";
static const char* const str_tag_LabelGuest = "LabelGuest";
static const char* const str_tag_Language = "Language";
static const char* const str_tag_Version = "Version";
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
static const char* const str_tag_rules = "Rules";

class MainWindowBase : public QMainWindow, public Ipponboard::IView
{
	Q_OBJECT
public:
	explicit MainWindowBase(QWidget* parent = nullptr);
	virtual ~MainWindowBase();

	virtual void Init();

	QString GetConfigFileName() const;
	QString GetFighterFileName() const;
	virtual EditionType Edition() const = 0;
	virtual const char* EditionName() const = 0;
	virtual const char* EditionNameShort() const = 0;

	/* IView */
	virtual void UpdateView();
	virtual void Reset() {}
	virtual void SetShowInfoHeader(bool /*show*/) {}

protected:
	virtual void changeEvent(QEvent*) override;
	virtual void closeEvent(QCloseEvent*) override;
	virtual void keyPressEvent(QKeyEvent*) override;

	void write_settings();
	void read_settings();
	void load_fighters();
	void save_fighters();
	virtual void update_views();
	virtual void update_statebar();
	virtual void update_info_text_color(const QColor& color, const QColor& bgColor);
	virtual void update_text_color_first(const QColor& color, const QColor& bgColor);
	virtual void update_text_color_second(const QColor& color, const QColor& bgColor);
	virtual void update_fighter_name_font(const QFont&);
	virtual void attach_primary_view() = 0;
	virtual void retranslate_Ui() = 0;
	virtual void ui_check_language_items() = 0;
    virtual void ui_check_rules_items() = 0;
    virtual void ui_check_show_secondary_view(bool checked) const = 0;

private:
	virtual void write_specific_settings(QSettings& settings);
	virtual void read_specific_settings(QSettings& settings);
	void change_lang(bool beQuiet = false);
	void show_hide_view() const;

protected slots:
	void on_actionSet_Main_Timer_triggered();
	void on_actionSet_Hold_Timer_triggered();
	void on_action_Info_Header_triggered(bool checked);
	void on_actionTest_Gong_triggered();
	void on_actionShow_SecondaryView_triggered();
	//void on_actionReset_Scores_triggered();
	void on_actionPreferences_triggered();
	void on_button_reset_clicked();
	void EvaluateInput();
	void on_actionLang_English_triggered(bool);
	void on_actionLang_Deutsch_triggered(bool);
    void on_actionLang_Dutch_triggered(bool);
    void on_actionRulesClassic_triggered(bool);
    void on_actionRules2013_triggered(bool);
    void on_actionRules2017_triggered(bool);
	void on_actionContact_Author_triggered();
	void on_actionOnline_Feedback_triggered();
	void on_actionVisit_Project_Homepage_triggered();
	void on_actionAbout_Ipponboard_triggered();
	virtual void on_actionManageFighters_triggered() {}

protected:
	virtual bool EvaluateSpecificInput(FMlib::Gamepad const* /*pGamepad*/) { return false; }

	std::shared_ptr<Ipponboard::View> m_pPrimaryView;
	std::shared_ptr<Ipponboard::View> m_pSecondaryView;
	std::shared_ptr<Ipponboard::Controller> m_pController;
	Ipponboard::FighterManager m_fighterManager;
	QString m_Language;
	QString m_MatLabel;
	QString m_weights;
	QFont m_FighterNameFont;
	int m_secondScreenNo;
	QSize m_secondScreenSize;
	Ipponboard::ControllerConfig m_controllerCfg;

private:
	PGamepad m_pGamepad;
};

#endif  // BASE__MAINWINDOW_BASE_H_
