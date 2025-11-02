// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef TEAM_EDITION_MAINWINDOW_H_
#define TEAM_EDITION_MAINWINDOW_H_

#include "../base/MainWindowBase.h"
#include "../core/Fight.h"
#include "../core/TournamentMode.h"

#include <memory>

class QTableView;
class QPrinter;
class QSettings;
class QString;
class QPoint;

namespace Ui
{
class MainWindowTeam;
}

class MainWindowTeam : public MainWindowBase
{
    Q_OBJECT
  public:
    explicit MainWindowTeam(QWidget* parent = nullptr);
    virtual ~MainWindowTeam();

    virtual void Init() final;

    virtual EditionType Edition() const final { return EditionType::Team; }
    virtual const char* EditionName() const final { return "Team Edition"; }
    virtual const char* EditionNameShort() const final { return "Team"; }
    static const char* ModeConfigurationFileName() { return "TournamentModes.ini"; }

    void LoadModes(Ipponboard::TournamentMode::List modes, QString selectedMode);

  protected:
    virtual void UpdateGoldenScoreView() final;
    //virtual void changeEvent(QEvent* e) override;
    virtual void closeEvent(QCloseEvent* event) override;
    virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void attach_primary_view() final;
    virtual void retranslate_Ui() final;
    virtual void ui_check_language_items() final;
    virtual void ui_check_rules_items() final
    {
        //FIXME
    }
    virtual void ui_check_show_secondary_view(bool checked) const final;

  private:
    static QStringList get_list_templates();
    void update_info_text_color(const QColor& color, const QColor& bgColor) override;
    void update_text_color_first(const QColor& color, const QColor& bgColor) override;
    void update_text_color_second(const QColor& color, const QColor& bgColor) override;
    virtual void update_fighter_name_font(const QFont&) override;
    virtual void update_views() override;

    // specific methods
    void update_club_views();
    void UpdateFightNumber_();
    void UpdateButtonText_();
    void update_score_screen();
    void WriteScoreToHtml_();
    virtual void write_specific_settings(QSettings& settings) final;
    virtual void read_specific_settings(QSettings& settings) final;
    //void update_fighter_name_completer(const QString& weight);
    //void update_fighters(const QString& s);

    /* base class slot overrides */
    virtual void on_actionManageFighters_triggered() override;

  private slots:
    void on_tableView_tournament_list1_customContextMenuRequested(QPoint const& pos);
    void on_tableView_tournament_list2_customContextMenuRequested(QPoint const& pos);
    void on_actionScore_Control_triggered();
    void on_actionScore_Screen_triggered();
    void on_button_pause_clicked();
    void on_actionSet_Round_Time_triggered();
    void on_pushButton_copySwitched_pressed();
    void on_toolButton_weights_pressed();
    void on_toolButton_team_home_pressed();
    void on_toolButton_team_guest_pressed();
    void slot_copy_cell_content_list1();
    void slot_copy_cell_content_list2();
    void slot_paste_cell_content_list1();
    void slot_paste_cell_content_list2();
    void slot_clear_cell_content_list1();
    void slot_clear_cell_content_list2();
    void Print(QPrinter* p);
    void on_tabWidget_currentChanged(int index);
    void on_actionManageModes_triggered();
    void on_actionManage_Clubs_triggered();
    void on_actionLoad_Demo_Data_triggered();
    void on_button_prev_clicked();
    void on_button_next_clicked();
    void on_comboBox_mode_currentIndexChanged(int i);
    void on_comboBox_club_host_currentTextChanged(const QString& s);
    void on_comboBox_club_home_currentTextChanged(const QString& s);
    void on_comboBox_club_guest_currentTextChanged(const QString& s);
    void on_actionPrint_triggered();
    void on_actionExport_triggered();
    void on_button_golden_score_toggled(bool);

    //void on_actionManage_Classes_triggered();
    //void on_comboBox_weight_currentTextChanged(const QString&);
    //void on_comboBox_name_second_currentTextChanged(const QString&);
    //void on_comboBox_name_first_currentTextChanged(const QString&);
    //void on_checkBox_golden_score_clicked(bool checked);
    //void on_comboBox_weight_class_currentTextChanged(const QString&);

    void on_actionReset_Scores_triggered();
    virtual bool EvaluateSpecificInput(GamepadLib::Gamepad const* pGamepad) override;

  private:
    void update_weights(QString const& weightString);
    void on_tableView_customContextMenuRequested(QTableView* pTableView, QPoint const& pos,
                                                 void (MainWindowTeam::*copySlot)(),
                                                 void (MainWindowTeam::*pasteSlot)(),
                                                 void (MainWindowTeam::*clearSlot)());
    void copy_cell_content(QTableView* pTableView);
    void paste_cell_content(QTableView* pTableView);
    void clear_cell_content(QTableView* pTableView);
    QString get_template_file(QString const& mode) const;
    QString get_full_mode_title(QString const& mode) const;

    std::unique_ptr<Ui::MainWindowTeam> m_pUi;
    std::shared_ptr<Ipponboard::ScoreScreen> m_pScoreScreen;
    std::shared_ptr<Ipponboard::ClubManager> m_pClubManager;
    QString m_htmlScore;
    QString m_currentMode;
    QString m_host;

    //std::shared_ptr<Ipponboard::FightCategoryMgr> m_pCategoryManager;
    QStringList m_FighterNamesHome;
    QStringList m_FighterNamesGuest;
    Ipponboard::TournamentMode::List m_modes;
    QString GetRoundDataAsHtml(const Ipponboard::Fight& fight, int fightNo);
};

#endif // TEAM_EDITION_MAINWINDOW_H_
