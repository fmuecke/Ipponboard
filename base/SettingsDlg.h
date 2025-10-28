// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__SETTINGSDLG_H_
#define BASE__SETTINGSDLG_H_

#include "../core/EditionType.h"
#include "GamepadSectionMapper.h"
#include "RawInputCapture.h"

#include <QDialog>
#include <QSoundEffect>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <memory>
#include <optional>
#include <vector>

class QComboBox;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

namespace Ui
{
class SettingsDlg;
}

namespace GamepadLib
{
class Gamepad;
}

namespace Ipponboard
{

using RawInputCapture = BasicRawInputCapture<GamepadLib::Gamepad>;

struct ControllerConfig;

class SettingsDlg : public QDialog
{
    Q_OBJECT
  public:
    SettingsDlg(EditionType edition, QWidget* parent = 0);
    ~SettingsDlg();

    void SetScreensSettings(int screen, const QSize& dimensions); // zero dimensions means auto size
    void SetInfoHeaderSettings(const QFont& font, const QColor& color, const QColor& background);
    void SetFighterNameFont(const QFont& font);
    void SetTextColorsFirst(const QColor& color, const QColor& background);
    void SetTextColorsSecond(const QColor& color, const QColor& background);
    void SetLabels(QString const& mat, QString const& home, QString const& guest);
    void SetGongFile(const QString& path);

    int GetSelectedScreen() const;
    QSize GetSize() const;
    QFont GetInfoHeaderFont() const;
    QFont GetFighterNameFont() const;
    QColor GetInfoTextColor() const;
    QColor GetTextColorFirst() const;
    QColor GetTextColorSecond() const;
    QColor GetInfoTextBgColor() const;
    QColor GetTextBgColorFirst() const;
    QColor GetTextBgColorSecond() const;
    QString GetMatLabel() const;
    QString GetHomeLabel() const;
    QString GetGuestLabel() const;
    QString GetGongFile() const;

    void SetControllerConfig(const ControllerConfig* pConfig);
    void GetControllerConfig(ControllerConfig* pConfig);
    void SetGamepad(GamepadLib::Gamepad* gamepad) noexcept;

  protected:
    void changeEvent(QEvent* e);

  private:
    struct RawButtonBinding
    {
        QLineEdit* lineEdit;
        QPushButton* captureButton;
        int ControllerConfig::*configMember;
        QString description;
    };

    struct RawAxisBinding
    {
        QLineEdit* lineEdit;
        QPushButton* captureButton;
        QCheckBox* invertCheckBox;
        int ControllerConfig::*codeMember;
        bool ControllerConfig::*invertMember;
        bool ControllerConfig::*legacyInvertMember;
    };

    enum class CaptureMode
    {
        None,
        Button,
        Axis
    };

    void initialize_raw_bindings();
    void start_button_capture(RawButtonBinding* binding);
    void finish_button_capture(std::uint16_t code);
    void start_axis_capture(RawAxisBinding* binding);
    void finish_axis_capture(int code);
    void cancel_raw_capture();
    void update_raw_status(const QString& message);
    [[nodiscard]] QString format_raw_value(int value) const;
    void prime_raw_capture();
    [[nodiscard]] QString describe_pressed_button(std::uint16_t code) const;
    void stop_raw_diagnostics(bool quiet = false);
    void update_raw_diagnostics();
    [[nodiscard]] QStringList describe_axes_state() const;
    [[nodiscard]] QString describe_section_action(
        const GamepadSectionMapper::SectionAction& action) const;

  private:
    EditionType m_edition;
    Ui::SettingsDlg* ui;
    QSoundEffect m_previewEffect;
    QTimer m_rawCaptureTimer;
    QTimer m_rawDiagnosticsTimer;
    GamepadLib::Gamepad* m_gamepad;
    std::unique_ptr<RawInputCapture> m_rawButtonCapture;
    std::unique_ptr<RawAxisCapture> m_rawAxisCapture;

    std::vector<RawButtonBinding> m_rawButtonBindings;
    std::vector<RawAxisBinding> m_rawAxisBindings;
    CaptureMode m_captureMode;
    RawButtonBinding* m_activeButtonBinding;
    RawAxisBinding* m_activeAxisBinding;

  private slots:
    void on_comboBox_mat_editTextChanged(QString);
    void on_checkBox_fighters_italic_toggled(bool checked);
    void on_checkBox_fighters_bold_toggled(bool checked);
    void on_checkBox_text_italic_toggled(bool checked);
    void on_checkBox_text_bold_toggled(bool checked);
    void on_toolButton_text_background_second_pressed();
    void on_toolButton_text_color_second_pressed();
    void on_toolButton_text_background_first_pressed();
    void on_toolButton_text_color_first_pressed();
    void on_toolButton_text_background_pressed(); //TODO: somehow the buttons stay pressed...
    void on_toolButton_text_color_pressed();      //TODO: somehow the buttons stay pressed...
    void on_fontComboBox_infoHeader_currentFontChanged(QFont f);
    void on_fontComboBox_fighters_currentFontChanged(QFont f);
    void on_toolButton_play_gong_pressed();
    void on_buttonBox_rejected();
    void on_buttonBox_accepted();
    void on_checkBox_secondary_view_custom_size_toggled(bool checked);
    void on_raw_capture_timeout();
    void on_pushButton_test_raw_mapping_toggled(bool checked);
};

} // namespace Ipponboard
#endif // BASE__SETTINGSDLG_H_
