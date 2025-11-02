// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASIC_EDITION_MAINWINDOW_H_
#define BASIC_EDITION_MAINWINDOW_H_

#include "../base/MainWindowBase.h"
#include "../util/helpers.hpp"

#include <memory>

namespace Ui
{
class MainWindow;
}

class MainWindow : public MainWindowBase
{
    Q_OBJECT
  public:
    explicit MainWindow(QWidget* parent = nullptr);
    virtual ~MainWindow();

    virtual void Init() final;

    virtual EditionType Edition() const final { return EditionType::Single; }
    virtual const char* EditionName() const final { return "Single Edition"; }
    virtual const char* EditionNameShort() const final { return "Single"; }

  protected:
    virtual void UpdateGoldenScoreView() final;
    //virtual void changeEvent(QEvent* e) override;
    //virtual void closeEvent(QCloseEvent* event) override;
    //virtual void keyPressEvent(QKeyEvent* event) override;
    virtual void write_specific_settings(QSettings& settings) final;
    virtual void read_specific_settings(QSettings& settings) final;
    virtual void update_statebar() override;
    virtual void attach_primary_view() final;
    virtual void retranslate_Ui() final;
    virtual void ui_check_language_items() final;
    virtual void ui_check_rules_items() final;
    virtual void ui_check_show_secondary_view(bool checked) const final;
    virtual void ui_update_used_options();

  private:
    //void update_info_text_color(const QColor& color, const QColor& bgColor) override;
    //void update_text_color_first(const QColor& color, const QColor& bgColor) override;
    //void update_text_color_second(const QColor& color, const QColor& bgColor) override;
    //virtual void update_fighter_name_font(const QFont&) override;
    //virtual void update_views() override;

    /* specific private methods */
    void update_fighter_name_completer(const QString& weight);
    void update_fighters(const QString& s);

    /* base class slot overrides */
    virtual void on_actionManageFighters_triggered() override;

  protected slots:
    //virtual bool EvaluateSpecificInput(GamepadLib::Gamepad const* pGamepad) override;

    /* specific private slots */
    void on_actionManageCategories_triggered();
    void on_comboBox_weight_currentTextChanged(const QString&);
    void on_comboBox_name_second_currentIndexChanged(int index);
    void on_comboBox_name_first_currentIndexChanged(int index);
    void on_checkBox_golden_score_clicked(bool checked);
    void on_comboBox_weight_class_currentTextChanged(const QString&);

  private slots:
    void on_actionAutoAdjustPoints_toggled(bool checked) override;
    void on_actionViewInfoBar_toggled(bool checked);
    void on_toolButton_viewSecondaryScreen_toggled();

  private:
    /* member */
    std::unique_ptr<Ui::MainWindow> m_pUi;
    std::shared_ptr<Ipponboard::FightCategoryMgr> m_pCategoryManager;
    QStringList m_CurrentFighterNames;
};

#endif // BASIC_EDITION_MAINWINDOW_H_
