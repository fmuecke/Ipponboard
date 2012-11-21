#ifndef BASE__MAINWINDOW_H_
#define BASE__MAINWINDOW_H_

#include "mainwindowbase.h"

#ifndef TEAM_VIEW
#include "../core/fighter.h"
#endif

#include <memory>

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

#ifdef TEAM_VIEW
static const char* const str_tag_Mode = "Mode";
static const char* const str_tag_Host = "Host";
#endif

class MainWindow : public MainWindowBase
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

protected:
    virtual void changeEvent(QEvent* e) override;
    virtual void closeEvent(QCloseEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;

private:
    void write_settings();
    void read_settings();
    void update_info_text_color(const QColor& color, const QColor& bgColor);
    void update_text_color_blue(const QColor& color, const QColor& bgColor);
    void update_text_color_white(const QColor& color, const QColor& bgColor);
    void update_fighter_name_font(const QFont&);
    void show_hide_view() const;
    void update_views();
    void change_lang(bool beQuiet = false);
#ifdef TEAM_VIEW
    void update_club_views();
    void UpdateFightNumber_();
    void UpdateScoreScreen_();
    void WriteScoreToHtml_();
#else
    void update_fighter_name_completer(const QString& weight);
    void update_fighters(const QString& s);
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
    void Print(QPrinter* p)
    {
        QTextEdit e(m_htmlScore, this);
        e.document()->print(p);
    }
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
    void on_comboBox_name_white_currentIndexChanged(const QString&);
    void on_comboBox_name_blue_currentIndexChanged(const QString&);
    void on_checkBox_golden_score_clicked(bool checked);
    void on_comboBox_weight_class_currentIndexChanged(const QString&);
    void on_actionImportList_triggered();
    void on_actionExportList_triggered();
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

private:
    std::unique_ptr<Ui::MainWindow> m_pUi;
    std::unique_ptr<Ipponboard::View> m_pPrimaryView;
    std::unique_ptr<Ipponboard::View> m_pSecondaryView;
    std::unique_ptr<Ipponboard::Controller> m_pController;
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

    std::unique_ptr<Ipponboard::ScoreScreen> m_pScoreScreen;
    std::tr1::shared_ptr<Ipponboard::ClubManager> m_pClubManager;
//    std::vector<QTableWidgetItem> fighters_home;
//    std::vector<QTableWidgetItem> fighters_guest;
    QString m_htmlScore;
    QString m_mode;
    QString m_host;
#else
    std::tr1::shared_ptr<Ipponboard::FightCategoryMgr> m_pCategoryManager;
    QStringList m_CurrentFighterNames;
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

    Ipponboard::ControlConfig m_controlCfg;
};

#endif  // BASE__MAINWINDOW_H_
