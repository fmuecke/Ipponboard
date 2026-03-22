// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__MAINWINDOW_BASE_H_
#define BASE__MAINWINDOW_BASE_H_

#include "../core/ControllerConfig.h"
#include "../core/EditionType.h"
#include "../core/iGoldenScoreView.h"
#include "../core/iView.h"
#include "../util/helpers.hpp"
#include "FighterManager.h"
#include "OnlineVersionChecker.h"

#include <QMainWindow>
#include <QPoint>
#include <QStyleHints>
#include <memory>
#include <vector>

// forwards
namespace Ui
{
//class MainWindow;
//class Frame_Clubs;
} // namespace Ui
namespace Ipponboard
{
class Fighter;
class View;
class Controller;
class ClubManager;
class FightCategoryMgr;
class ScoreScreen;
} // namespace Ipponboard
class QApplication;
class QLabel;
class QSettings;

namespace GamepadLib
{
class Gamepad;
}

constexpr auto str_golden_score = "Golden Score";
constexpr auto str_normal_round_time = "Normal";

namespace settings
{
constexpr auto str_Main = "Main";
constexpr auto str_SecondScreen = "SecondScreen";
constexpr auto str_SecondScreenSize = "SecondScreenSize";
constexpr auto str_SecondScreenOffset = "SecondScreenOffset";
constexpr auto str_MatLabel = "MatLabel";
constexpr auto str_LabelHome = "LabelHome";
constexpr auto str_LabelGuest = "LabelGuest";
constexpr auto str_Language = "Language";
constexpr auto str_Theme = "Theme";
constexpr auto str_Version = "Version";
constexpr auto str_Fonts = "Fonts";
constexpr auto str_TextFont1 = "TextFont1";
constexpr auto str_FighterNameFont = "FighterNameFont";
constexpr auto str_DigitFont = "DigitFont";
constexpr auto str_Colors = "Colors";
constexpr auto str_InfoTextColor = "InfoTextColor";
constexpr auto str_InfoTextBgColor = "InfoTextBgColor";
constexpr auto str_TextColorFirst = "TextColorFirst";
constexpr auto str_TextBgColorFirst = "TextBgColorFirst";
constexpr auto str_TextColorSecond = "TextColorSecond";
constexpr auto str_TextBgColorSecond = "TextBgColorSecond";
constexpr auto str_MainClockColorRunning = "MainClockColorRunning";
constexpr auto str_MainClockColorStopped = "MainClockColorStopped";
constexpr auto str_Input = "Input";
constexpr auto str_buttonHajimeMate = "ButtonHajimeMate";
constexpr auto str_buttonOsaekomiToketa = "ButtonOsaekomiToketa";
constexpr auto str_buttonNext = "ButtonNext";
constexpr auto str_buttonPrev = "ButtonPrev";
constexpr auto str_buttonPause = "ButtonPause";
constexpr auto str_buttonReset = "ButtonReset";
constexpr auto str_buttonReset2 = "ButtonReset2";
constexpr auto str_buttonResetHoldFirst = "ButtonResetHoldFirst";
constexpr auto str_buttonResetHoldSecond = "ButtonResetHoldSecond";
constexpr auto str_buttonFirstHolding = "ButtonFirstHolding";
constexpr auto str_buttonSecondHolding = "ButtonSecondHolding";
constexpr auto str_buttonHansokumakeFirst = "ButtonHansokumakeFirst";
constexpr auto str_buttonHansokumakeSecond = "ButtonHansokumakeSecond";
constexpr auto str_invertX = "InvertX";
constexpr auto str_invertY = "InvertY";
constexpr auto str_invertR = "InvertR";
constexpr auto str_invertZ = "InvertZ";
constexpr auto str_InputRaw = "InputRaw";
constexpr auto str_raw_buttonHajimeMate = "ButtonHajimeMateRaw";
constexpr auto str_raw_buttonNext = "ButtonNextRaw";
constexpr auto str_raw_buttonPrev = "ButtonPrevRaw";
constexpr auto str_raw_buttonPause = "ButtonPauseRaw";
constexpr auto str_raw_buttonReset = "ButtonResetRaw";
constexpr auto str_raw_buttonReset2 = "ButtonReset2Raw";
constexpr auto str_raw_buttonResetHoldFirst = "ButtonResetHoldFirstRaw";
constexpr auto str_raw_buttonResetHoldSecond = "ButtonResetHoldSecondRaw";
constexpr auto str_raw_buttonFirstHolding = "ButtonFirstHoldingRaw";
constexpr auto str_raw_buttonSecondHolding = "ButtonSecondHoldingRaw";
constexpr auto str_raw_buttonHansokumakeFirst = "ButtonHansokumakeFirstRaw";
constexpr auto str_raw_buttonHansokumakeSecond = "ButtonHansokumakeSecondRaw";
constexpr auto str_axisLeftX = "AxisLeftX";
constexpr auto str_axisLeftY = "AxisLeftY";
constexpr auto str_axisRightX = "AxisRightX";
constexpr auto str_axisRightY = "AxisRightY";
constexpr auto str_axisLeftInvertX = "AxisLeftInvertX";
constexpr auto str_axisLeftInvertY = "AxisLeftInvertY";
constexpr auto str_axisRightInvertX = "AxisRightInvertX";
constexpr auto str_axisRightInvertY = "AxisRightInvertY";
constexpr auto str_Sounds = "Sounds";
constexpr auto str_Options = "Options";
constexpr auto str_MatSignal = "MatSignal";
constexpr auto str_rules = "Rules";
constexpr auto str_autoAdjustPoints = "AutoAdjustPoints";
constexpr auto str_checkVersionOnStartup = "CheckVersionOnStartup";
} // namespace settings

class MainWindowBase : public QMainWindow,
                       public Ipponboard::IView,
                       public Ipponboard::IGoldenScoreView
{
    Q_OBJECT

  public:
    explicit MainWindowBase(QWidget* parent = nullptr);
    ~MainWindowBase() override;

    virtual void Init();

    static QString GetConfigFileName();
    QString GetFighterFileName() const;
    virtual EditionType Edition() const = 0;
    virtual const char* EditionName() const = 0;
    virtual const char* EditionNameShort() const = 0;

    /* IView */
    void UpdateView() override;
    void Reset() override {}
    void SetShowInfoHeader(bool /*show*/) override {}

    /* IGoldenScoreView */
    void UpdateGoldenScoreView() override{};

  protected:
    virtual void changeEvent(QEvent*) override;
    virtual void closeEvent(QCloseEvent*) override;
    virtual void keyPressEvent(QKeyEvent*) override;

    virtual void write_settings() const;
    virtual void read_settings();
    void load_fighters();
    void save_fighters();
    virtual void update_views();
    virtual void update_screen_visibility(QWidget* pView) const;
    virtual void update_statebar();
    virtual void update_info_text_color(const QColor& color, const QColor& bgColor);
    virtual void update_text_color_first(const QColor& color, const QColor& bgColor);
    virtual void update_text_color_second(const QColor& color, const QColor& bgColor);
    virtual void update_fighter_name_font(const QFont&);
    virtual void attach_primary_view() = 0;
    virtual void retranslate_Ui() = 0;
    virtual void ui_check_language_items() = 0;
    virtual void ui_check_theme_items() = 0;
    virtual void ui_check_rules_items() = 0;
    virtual void ui_check_show_secondary_view(bool checked) const = 0;

  private:
    void change_lang(bool beQuiet = false);
    void change_theme();
    void show_hide_view() const;
    void ensureVersionStatusLabel();
    void startVersionCheck();
    void applyVersionStatus(const OnlineVersionChecker::OnlineVersion& onlineVersion);
    void showVersionDialog();
    void updateVersionStatusText();

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
    void on_actionThemeAutomatic_triggered(bool);
    void on_actionThemeDark_triggered(bool);
    void on_actionThemeLight_triggered(bool);
    void on_actionRulesClassic_triggered(bool);
    void on_actionRules2013_triggered(bool);
    void on_actionRules2017_triggered(bool);
    void on_actionRules2017U15_triggered(bool);
    void on_actionRules2018_triggered(bool);
    void on_actionRules2025_triggered(bool);
    void on_actionOnline_Feedback_triggered();
    void on_actionVisit_Project_Homepage_triggered();
    void on_actionAbout_Ipponboard_triggered();
    void on_actionUser_Manual_triggered();
    void on_actionView_Logfile_triggered();
    virtual void on_actionManageFighters_triggered() {}
    virtual void on_actionAutoAdjustPoints_toggled(bool checked);

  protected:
    virtual bool EvaluateSpecificInput(GamepadLib::Gamepad const* /*pGamepad*/) { return false; }
    void set_input_suppressed(bool value) noexcept { m_isInputSuppressed = value; }
    [[nodiscard]] bool is_input_suppressed() const noexcept { return m_isInputSuppressed; }

    std::shared_ptr<Ipponboard::View> m_pPrimaryView;
    std::shared_ptr<Ipponboard::View> m_pSecondaryView;
    std::shared_ptr<Ipponboard::Controller> m_pController;
    Ipponboard::FighterManager m_fighterManager;
    QString m_Language;
    Qt::ColorScheme m_Theme;
    QString m_MatLabel;
    QString m_weights;
    QFont m_FighterNameFont;
    int m_secondScreenNo;
    QSize m_secondScreenSize;
    QPoint m_secondScreenOffset;
    Ipponboard::ControllerConfig m_controllerCfg;

  private:
    std::unique_ptr<GamepadLib::Gamepad> m_pGamepad;
    QLabel* m_pVersionStatusLabel{ nullptr };
    OnlineVersionChecker::OnlineVersion m_latestOnlineVersion;
    bool m_isInputSuppressed{ false };
    bool m_checkVersionOnStartup{ true };
    bool m_isVersionCheckInProgress{ false };
    OnlineVersionChecker::State m_versionStatusState{ OnlineVersionChecker::State::Empty };
};

#endif // BASE__MAINWINDOW_BASE_H_
