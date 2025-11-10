// Copyright 2018-2025 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "SettingsDlg.h"

#include "../core/ControllerConfig.h"
#include "../gamepad/Gamepad.h"
#include "ui_SettingsDlg.h"

#include <QColorDialog>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QGuiApplication>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScreen>
#include <QSignalBlocker>
#include <QSoundEffect>
#include <QStringList>
#include <QUrl>
#include <limits>
#include <memory>
#include <vector>


using namespace Ipponboard;

SettingsDlg::SettingsDlg(EditionType edition, QWidget* parent)
    : QDialog(parent),
      m_edition(edition),
      ui(new Ui::SettingsDlg),
      m_previewEffect(),
      m_rawCaptureTimer(this),
      m_rawDiagnosticsTimer(this),
      m_gamepad(nullptr),
      m_rawButtonCapture(),
      m_rawAxisCapture(),
      m_rawButtonBindings(),
      m_rawAxisBindings(),
      m_captureMode(CaptureMode::None),
      m_activeButtonBinding(nullptr),
      m_activeAxisBinding(nullptr)
{
    ui->setupUi(this);
    m_previewEffect.setParent(this);
    m_previewEffect.setLoopCount(1);
    initialize_raw_bindings();

    ui->toolButton_test_raw_mapping->setCheckable(true);
    ui->toolButton_test_raw_mapping->setText(tr("Show pressed buttons"));

    if (m_edition == EditionType::Team)
    {
        ui->comboBox_mat->setEnabled(false);
        ui->label_home->setEnabled(true);
        ui->label_guest->setEnabled(true);
        ui->lineEdit_labelHome->setEnabled(true);
        ui->lineEdit_labelGuest->setEnabled(true);
    }
    else
    {
        ui->comboBox_mat->setEnabled(true);
        ui->label_home->setEnabled(false);
        ui->label_guest->setEnabled(false);
        ui->lineEdit_labelHome->setEnabled(false);
        ui->lineEdit_labelGuest->setEnabled(false);
    }

    ui->text_text_sample->SetText("  Ipponboard   ");
    ui->text_color_first->SetText(tr("FIRST FIGHTER"));
    ui->text_color_second->SetText(tr("SECOND FIGHTER"));

    // num screens
    int numScreens = QGuiApplication::screens().count();

    for (int i(1); i <= numScreens; ++i)
    {
        QRect res = QGuiApplication::screens().at(i - 1)->availableGeometry();

        ui->comboBox_screen->addItem(QString("%1 (%2x%3)")
                                         .arg(QString::number(i),
                                              QString::number(res.width()),
                                              QString::number(res.height())));
    }

    QDir dir(QDir::currentPath() + "/sounds");
    QStringList nameFilters;
    nameFilters << "*.wav";
    dir.setNameFilters(nameFilters);
    QStringList sounds(dir.entryList(QDir::Files));
    ui->comboBox_sound_time_ends->addItems(sounds);

    // keyboard page
    QStringList keyVals;
    keyVals << "<none>";
    keyVals << "ENTER";        // VK_RETURN
    keyVals << "BACKSPACE";    // VK_BACK
    keyVals << "SPACE";        // VK_SPACE
    keyVals << "TAB";          // VK_TAB
    keyVals << "INS";          // VK_INSERT
    keyVals << "DEL";          // VK_DELETE
    keyVals << "HOME";         // VK_HOME
    keyVals << "END";          // VK_END
    keyVals << "PAGE UP";      // VK_PRIOR
    keyVals << "PAGE DOWN";    // VK_NEXT
    keyVals << "PAUSE";        // VK_PAUSE
    keyVals << "LEFT ARROW";   // VK_LEFT
    keyVals << "UP ARROW";     // VK_UP
    keyVals << "DOWN ARROW";   // VK_DOWN
    keyVals << "RIGHT ARROW";  // VK_RIGHT
    keyVals << "NUMPAD 0";     // VK_NUMPAD0	60	Numeric keypad 0 key
    keyVals << "NUMPAD 1";     // VK_NUMPAD1	61	Numeric keypad 1 key
    keyVals << "NUMPAD 2";     // VK_NUMPAD2	62	Numeric keypad 2 key
    keyVals << "NUMPAD 3";     // VK_NUMPAD3	63	Numeric keypad 3 key
    keyVals << "NUMPAD 4";     // VK_NUMPAD4	64	Numeric keypad 4 key
    keyVals << "NUMPAD 5";     // VK_NUMPAD5	65	Numeric keypad 5 key
    keyVals << "NUMPAD 6";     // VK_NUMPAD6	66	Numeric keypad 6 key
    keyVals << "NUMPAD 7";     // VK_NUMPAD7	67	Numeric keypad 7 key
    keyVals << "NUMPAD 8";     // VK_NUMPAD8	68	Numeric keypad 8 key
    keyVals << "NUMPAD 9";     // VK_NUMPAD9	69	Numeric keypad 9 key
    keyVals << "NUMPAD *";     // VK_MULTIPLY	6A	Multiply key
    keyVals << "NUMPAD +";     // VK_ADD	6B	Add key
    keyVals << "NUMPAD ENTER"; // VK_SEPARATOR	6C	Separator key
    keyVals << "NUMPAD -";     // VK_SUBTRACT	6D	Subtract key
    keyVals << "NUMPAD ,";     // VK_DECIMAL	6E	Decimal key
    keyVals << "NUMPAD %";     // VK_DIVIDE	6F	Divide key
    keyVals << "F1";
    keyVals << "F2";
    keyVals << "F3";
    keyVals << "F4";
    keyVals << "F5";
    keyVals << "F6";
    keyVals << "F7";
    keyVals << "F8";
    keyVals << "F9";
    keyVals << "F10";
    keyVals << "F11";
    keyVals << "F12";
    // (http://msdn.microsoft.com/en-us/library/ms927178.aspx)

    QStringList modifierVals;
    modifierVals << "<none>";
    modifierVals << "Ctrl";
    modifierVals << "Alt";
    modifierVals << "Shift";
    modifierVals << "Ctrl+Alt";
    modifierVals << "Ctrl+Shift";
    modifierVals << "Alt+Shift";
    modifierVals << "Ctrl+Alt+Shift";

    //FIXME: enable this when keyboard access is customizable
    //for (int i = 0; i < ui->tableWidget_keys->rowCount(); ++i)
    //{
    //	QComboBox* pCombo = new QComboBox();
    //	pCombo->addItems(keyVals);
    //	pCombo->setEditable(true);
    //	ui->tableWidget_keys->setCellWidget(i, 1, pCombo);
    //
    //	pCombo = new QComboBox();
    //	pCombo->addItems(modifierVals);
    //	ui->tableWidget_keys->setCellWidget(i, 0, pCombo);
    //}

    ui->tableWidget_keys->verticalHeader()->setVisible(true);

    // NOTE: This is nasty workaround for the standard buttons not
    // beeing translated (separate translator would be required)
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText(tr("Ok"));
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText(tr("Cancel"));
}

void SettingsDlg::initialize_raw_bindings()
{
    m_rawCaptureTimer.setInterval(50);
    m_rawCaptureTimer.setSingleShot(false);
    connect(&m_rawCaptureTimer, &QTimer::timeout, this, &SettingsDlg::on_raw_capture_timeout);

    m_rawDiagnosticsTimer.setInterval(150);
    m_rawDiagnosticsTimer.setSingleShot(false);
    connect(&m_rawDiagnosticsTimer, &QTimer::timeout, this, &SettingsDlg::update_raw_diagnostics);

    auto* validator = new QIntValidator(-1, std::numeric_limits<std::uint16_t>::max(), this);

    m_rawButtonBindings = {
        { ui->lineEdit_raw_hajime_mate,
          ui->pushButton_capture_raw_hajime_mate,
          &ControllerConfig::button_hajime_mate_raw,
          tr("Hajime / Mate") },
        { ui->lineEdit_raw_next,
          ui->pushButton_capture_raw_next,
          &ControllerConfig::button_next_raw,
          tr("Next fight") },
        { ui->lineEdit_raw_prev,
          ui->pushButton_capture_raw_prev,
          &ControllerConfig::button_prev_raw,
          tr("Previous fight") },
        { ui->lineEdit_raw_pause,
          ui->pushButton_capture_raw_pause,
          &ControllerConfig::button_pause_raw,
          tr("Pause") },
        { ui->lineEdit_raw_reset_first,
          ui->pushButton_capture_raw_reset_first,
          &ControllerConfig::button_reset_raw,
          tr("Reset button (left)") },
        { ui->lineEdit_raw_reset_second,
          ui->pushButton_capture_raw_reset_second,
          &ControllerConfig::button_reset2_raw,
          tr("Reset button (right)") },
        { ui->lineEdit_raw_osaekomi_first,
          ui->pushButton_capture_raw_osaekomi_first,
          &ControllerConfig::button_osaekomi_toketa_first_raw,
          tr("Osaekomi / Toketa (left)") },
        { ui->lineEdit_raw_osaekomi_second,
          ui->pushButton_capture_raw_osaekomi_second,
          &ControllerConfig::button_osaekomi_toketa_second_raw,
          tr("Osaekomi / Toketa (right)") },
        { ui->lineEdit_raw_reset_hold_first,
          ui->pushButton_capture_raw_reset_hold_first,
          &ControllerConfig::button_reset_hold_first_raw,
          tr("Reset hold (left)") },
        { ui->lineEdit_raw_reset_hold_second,
          ui->pushButton_capture_raw_reset_hold_second,
          &ControllerConfig::button_reset_hold_second_raw,
          tr("Reset hold (right)") },
        { ui->lineEdit_raw_hansokumake_first,
          ui->pushButton_capture_raw_hansokumake_first,
          &ControllerConfig::button_hansokumake_first_raw,
          tr("Hansokumake (left)") },
        { ui->lineEdit_raw_hansokumake_second,
          ui->pushButton_capture_raw_hansokumake_second,
          &ControllerConfig::button_hansokumake_second_raw,
          tr("Hansokumake (right)") },
    };

    for (std::size_t idx = 0; idx < m_rawButtonBindings.size(); ++idx)
    {
        auto& binding = m_rawButtonBindings[idx];
        if (binding.lineEdit)
        {
            binding.lineEdit->setValidator(validator);
            binding.lineEdit->setPlaceholderText(tr("-1"));
        }

        if (binding.captureButton)
        {
            // Note: capture the index, not the binding reference, so we avoid dangling pointers (no range-based for!).
            connect(binding.captureButton,
                    &QPushButton::clicked,
                    this,
                    [this, idx]() { start_button_capture(&m_rawButtonBindings[idx]); });
        }
    }

    m_rawAxisBindings = {
        { ui->lineEdit_raw_axis_left_x,
          ui->pushButton_capture_raw_axis_left_x,
          ui->checkBox_raw_axis_left_x_invert,
          &ControllerConfig::axis_left_x,
          &ControllerConfig::axis_left_invert_x,
          &ControllerConfig::axis_inverted_X }, //TODO: consolidate legacy member
        { ui->lineEdit_raw_axis_left_y,
          ui->pushButton_capture_raw_axis_left_y,
          ui->checkBox_raw_axis_left_y_invert,
          &ControllerConfig::axis_left_y,
          &ControllerConfig::axis_left_invert_y,
          &ControllerConfig::axis_inverted_Y }, //TODO: consolidate legacy member
        { ui->lineEdit_raw_axis_right_x,
          ui->pushButton_capture_raw_axis_right_x,
          ui->checkBox_raw_axis_right_x_invert,
          &ControllerConfig::axis_right_x,
          &ControllerConfig::axis_right_invert_x,
          &ControllerConfig::axis_inverted_R }, //TODO: consolidate legacy member
        { ui->lineEdit_raw_axis_right_y,
          ui->pushButton_capture_raw_axis_right_y,
          ui->checkBox_raw_axis_right_y_invert,
          &ControllerConfig::axis_right_y,
          &ControllerConfig::axis_right_invert_y,
          &ControllerConfig::axis_inverted_Z }, //TODO: consolidate legacy member
    };

    for (std::size_t idx = 0; idx < m_rawAxisBindings.size(); ++idx)
    {
        auto& binding = m_rawAxisBindings[idx];
        if (binding.lineEdit)
        {
            binding.lineEdit->setValidator(validator);
            binding.lineEdit->setPlaceholderText(tr("-1"));
        }

        if (binding.captureButton)
        {
            // Same rationale as the buttons: capture-by-index keeps the binding pointer valid.
            connect(binding.captureButton,
                    &QPushButton::clicked,
                    this,
                    [this, idx]() { start_axis_capture(&m_rawAxisBindings[idx]); });
        }
    }

    update_raw_status(tr("Press Capture to record a button or axis code."));
}

void SettingsDlg::start_button_capture(RawButtonBinding* binding)
{
    stop_raw_diagnostics(true);

    if (!binding)
    {
        return;
    }

    if (!m_gamepad)
    {
        update_raw_status(tr("No gamepad detected for capture."));
        return;
    }

    if (m_captureMode == CaptureMode::Button && m_activeButtonBinding == binding)
    {
        cancel_raw_capture();
        return;
    }

    cancel_raw_capture();

    m_rawButtonCapture = std::make_unique<RawInputCapture>(*m_gamepad);
    m_captureMode = CaptureMode::Button;
    m_activeButtonBinding = binding;
    if (binding->captureButton)
    {
        binding->captureButton->setText(tr("Cancel"));
    }

    update_raw_status(tr("Press the desired button on the controller."));
    prime_raw_capture();
    m_rawCaptureTimer.start();
}

void SettingsDlg::finish_button_capture(std::uint16_t code)
{
    if (!m_activeButtonBinding)
    {
        return;
    }

    if (m_activeButtonBinding->lineEdit)
    {
        m_activeButtonBinding->lineEdit->setText(QString::number(code));
    }

    if (m_activeButtonBinding->captureButton)
    {
        m_activeButtonBinding->captureButton->setText(tr("Capture"));
    }

    update_raw_status(tr("Captured raw code %1.").arg(code));

    m_activeButtonBinding = nullptr;
    m_captureMode = CaptureMode::None;
    m_rawCaptureTimer.stop();
    m_rawButtonCapture.reset();
}

void SettingsDlg::start_axis_capture(RawAxisBinding* binding)
{
    stop_raw_diagnostics(true);

    if (!binding)
    {
        return;
    }

    if (!m_gamepad)
    {
        update_raw_status(tr("No gamepad detected for capture."));
        return;
    }

    if (m_captureMode == CaptureMode::Axis && m_activeAxisBinding == binding)
    {
        cancel_raw_capture();
        return;
    }

    cancel_raw_capture();

    m_rawAxisCapture =
        std::make_unique<RawAxisCapture>(*m_gamepad,
                                         std::vector<GamepadLib::EAxis>{ GamepadLib::EAxis::X,
                                                                         GamepadLib::EAxis::Y,
                                                                         GamepadLib::EAxis::R,
                                                                         GamepadLib::EAxis::Z });

    m_captureMode = CaptureMode::Axis;
    m_activeAxisBinding = binding;
    if (binding->captureButton)
    {
        binding->captureButton->setText(tr("Cancel"));
    }

    update_raw_status(tr("Move the desired axis on the controller."));
    prime_raw_capture();
    m_rawCaptureTimer.start();
}

void SettingsDlg::finish_axis_capture(int code)
{
    if (!m_activeAxisBinding)
    {
        return;
    }

    if (m_activeAxisBinding->lineEdit)
    {
        m_activeAxisBinding->lineEdit->setText(QString::number(code));
    }

    if (m_activeAxisBinding->captureButton)
    {
        m_activeAxisBinding->captureButton->setText(tr("Capture"));
    }

    update_raw_status(tr("Captured axis code %1.").arg(code));

    m_activeAxisBinding = nullptr;
    m_captureMode = CaptureMode::None;
    m_rawCaptureTimer.stop();
    m_rawAxisCapture.reset();
}

void SettingsDlg::cancel_raw_capture()
{
    const bool hadActiveCapture = m_captureMode != CaptureMode::None;

    if (m_activeButtonBinding && m_activeButtonBinding->captureButton)
    {
        m_activeButtonBinding->captureButton->setText(tr("Capture"));
    }

    if (m_activeAxisBinding && m_activeAxisBinding->captureButton)
    {
        m_activeAxisBinding->captureButton->setText(tr("Capture"));
    }

    m_activeButtonBinding = nullptr;
    m_activeAxisBinding = nullptr;
    m_captureMode = CaptureMode::None;

    if (m_rawCaptureTimer.isActive())
    {
        m_rawCaptureTimer.stop();
    }

    m_rawButtonCapture.reset();
    m_rawAxisCapture.reset();

    if (hadActiveCapture)
    {
        update_raw_status(tr("Capture cancelled."));
    }
}

void SettingsDlg::update_raw_status(const QString& message)
{
    if (ui && ui->label_raw_status)
    {
        ui->label_raw_status->setText(message);
    }
}

QString SettingsDlg::format_raw_value(int value) const
{
    if (value < 0)
    {
        return {};
    }

    return QString::number(value);
}

QString SettingsDlg::describe_pressed_button(std::uint16_t code) const
{
    for (const auto& binding : m_rawButtonBindings)
    {
        if (!binding.lineEdit)
        {
            continue;
        }

        bool ok = false;
        const int value = binding.lineEdit->text().toInt(&ok);
        if (ok && value == static_cast<int>(code))
        {
            if (!binding.description.isEmpty())
            {
                return tr("%1 (code %2)").arg(binding.description, QString::number(code));
            }

            return tr("Configured binding (code %1)").arg(code);
        }
    }

    if (code >= GamepadLib::Constants::PovCodeBase &&
        code < GamepadLib::Constants::PovCodeBase + GamepadLib::Constants::PovCodeCount)
    {
        const int index = static_cast<int>(code - GamepadLib::Constants::PovCodeBase);
        QStringList povNames;
        povNames << tr("POV forward") << tr("POV right") << tr("POV back") << tr("POV left")
                 << tr("POV right-forward") << tr("POV right-back") << tr("POV left-back")
                 << tr("POV left-forward");
        return tr("%1 (code %2)").arg(povNames.value(index), QString::number(code));
    }

    if (code < GamepadLib::Constants::MaxButtons)
    {
        return tr("Button %1 (code %2)")
            .arg(QString::number(static_cast<int>(code) + 1), QString::number(code));
    }

    return tr("Raw code %1").arg(code);
}

SettingsDlg::~SettingsDlg()
{
    m_previewEffect.stop();
    m_previewEffect.setSource(QUrl());
    cancel_raw_capture();
    stop_raw_diagnostics(true);
    delete ui;
}

void SettingsDlg::SetScreensSettings(int screen, const QSize& dimensions)
{
    Q_ASSERT(screen < ui->comboBox_screen->count());
    ui->comboBox_screen->setCurrentIndex(screen);

    if (dimensions.isNull())
    {
        ui->checkBox_secondary_view_custom_size->setChecked(false);
    }
    else
    {
        ui->checkBox_secondary_view_custom_size->setChecked(true);
        ui->lineEdit_fixedsize_width->setText(QString::number(dimensions.width()));
        ui->lineEdit_fixedsize_height->setText(QString::number(dimensions.height()));
        ui->lineEdit_fixedsize_width->setEnabled(true);
        ui->lineEdit_fixedsize_height->setEnabled(true);
    }
}

void SettingsDlg::SetInfoHeaderSettings(const QFont& font, const QColor& color,
                                        const QColor& background)
{
    ui->fontComboBox_infoHeader->setCurrentFont(font);
    ui->text_text_sample->SetFontAndColor(font, color, background);
    ui->checkBox_text_bold->setChecked(font.bold());
    ui->checkBox_text_italic->setChecked(font.italic());
}

void SettingsDlg::SetFighterNameFont(const QFont& font)
{
    ui->fontComboBox_fighters->setCurrentFont(font);
    ui->checkBox_fighters_bold->setChecked(font.bold());
    ui->checkBox_fighters_italic->setChecked(font.italic());
    ui->text_color_first->SetFont(font);
    ui->text_color_second->SetFont(font);
}

void SettingsDlg::SetTextColorsFirst(const QColor& color, const QColor& background)
{
    ui->text_color_first->SetColor(color, background);
}

void SettingsDlg::SetTextColorsSecond(const QColor& color, const QColor& background)
{
    ui->text_color_second->SetColor(color, background);
}

void SettingsDlg::SetLabels(QString const& mat, QString const& home, QString const& guest)
{
    const int index = ui->comboBox_mat->findText(mat);

    if (-1 != index)
    {
        ui->comboBox_mat->setCurrentIndex(index);
    }
    else
    {
        ui->comboBox_mat->setEditText(mat);
    }

    ui->text_text_sample->SetText(mat);

    ui->lineEdit_labelHome->setText(home);
    ui->lineEdit_labelGuest->setText(guest);
}

void SettingsDlg::SetGongFile(const QString& path)
{
    int pos = path.lastIndexOf('/', -1);
    QString filename;

    if (-1 != pos)
        filename = path.right(path.length() - pos - 1);
    else
        filename = path;

    const int index = ui->comboBox_sound_time_ends->findText(filename);

    if (-1 != index)
        ui->comboBox_sound_time_ends->setCurrentIndex(index);
}

int SettingsDlg::GetSelectedScreen() const { return ui->comboBox_screen->currentIndex(); }

QSize SettingsDlg::GetSize() const
{
    QSize s;
    s.setWidth(ui->lineEdit_fixedsize_width->text().toInt());
    s.setHeight(ui->lineEdit_fixedsize_height->text().toInt());
    return s;
}

QFont SettingsDlg::GetInfoHeaderFont() const
{
    QFont f = ui->fontComboBox_infoHeader->currentFont();
    f.setBold(ui->checkBox_text_bold->isChecked());
    f.setItalic(ui->checkBox_text_italic->isChecked());
    return f;
}

QFont SettingsDlg::GetFighterNameFont() const
{
    QFont f = ui->fontComboBox_fighters->currentFont();
    f.setBold(ui->checkBox_fighters_bold->isChecked());
    f.setItalic(ui->checkBox_fighters_italic->isChecked());
    return f;
}

QColor SettingsDlg::GetInfoTextColor() const { return ui->text_text_sample->GetColor(); }

QColor SettingsDlg::GetTextColorFirst() const { return ui->text_color_first->GetColor(); }

QColor SettingsDlg::GetTextColorSecond() const { return ui->text_color_second->GetColor(); }

QColor SettingsDlg::GetInfoTextBgColor() const { return ui->text_text_sample->GetBgColor(); }
QColor SettingsDlg::GetTextBgColorFirst() const { return ui->text_color_first->GetBgColor(); }
QColor SettingsDlg::GetTextBgColorSecond() const { return ui->text_color_second->GetBgColor(); }

QString SettingsDlg::GetMatLabel() const { return ui->comboBox_mat->currentText(); }

QString SettingsDlg::GetHomeLabel() const { return ui->lineEdit_labelHome->text(); }

QString SettingsDlg::GetGuestLabel() const { return ui->lineEdit_labelGuest->text(); }

QString SettingsDlg::GetGongFile() const
{
    QString path = QDir::currentPath() + "/sounds/";
    return path + ui->comboBox_sound_time_ends->currentText();
}

void SettingsDlg::on_buttonBox_accepted() { accept(); }

void SettingsDlg::on_buttonBox_rejected() { reject(); }

void SettingsDlg::SetControllerConfig(const ControllerConfig* pConfig)
{
    Q_ASSERT(pConfig);

    if (pConfig)
    {
        for (const auto& binding : m_rawButtonBindings)
        {
            if (binding.lineEdit)
            {
                const int value = pConfig->*(binding.configMember);
                binding.lineEdit->setText(format_raw_value(value));
            }
        }

        for (const auto& binding : m_rawAxisBindings)
        {
            if (binding.lineEdit)
            {
                const int value = pConfig->*(binding.codeMember);
                binding.lineEdit->setText(format_raw_value(value));
            }

            if (binding.invertCheckBox)
            {
                bool invert = binding.invertMember ? pConfig->*(binding.invertMember) : false;
                if (!invert && binding.legacyInvertMember)
                {
                    invert = pConfig->*(binding.legacyInvertMember);
                }
                binding.invertCheckBox->setChecked(invert);
            }
        }
    }
}

void SettingsDlg::GetControllerConfig(ControllerConfig* pConfig)
{
    Q_ASSERT(pConfig);

    if (pConfig)
    {
        for (const auto& binding : m_rawButtonBindings)
        {
            if (!binding.lineEdit)
            {
                continue;
            }

            bool ok = false;
            const int value = binding.lineEdit->text().toInt(&ok);
            pConfig->*(binding.configMember) = ok ? value : -1;
        }

        for (const auto& binding : m_rawAxisBindings)
        {
            if (binding.lineEdit)
            {
                bool ok = false;
                const int value = binding.lineEdit->text().toInt(&ok);
                pConfig->*(binding.codeMember) = ok ? value : -1;
            }

            if (binding.invertCheckBox)
            {
                const bool isChecked = binding.invertCheckBox->isChecked();
                if (binding.invertMember)
                {
                    pConfig->*(binding.invertMember) = isChecked;
                }
                if (binding.legacyInvertMember)
                {
                    pConfig->*(binding.legacyInvertMember) = isChecked;
                }
            }
        }
    }
}

void SettingsDlg::SetGamepad(GamepadLib::Gamepad* gamepad) noexcept
{
    m_gamepad = gamepad;

    if (m_gamepad)
    {
        if (m_gamepad->GetState() != GamepadLib::EGamepadState::ok)
        {
            update_raw_status(tr("No gamepad detected for capture."));
            qInfo() << "No gamepad detected for raw capture.";
            return;
        }

        const auto* namePtr = m_gamepad->GetProductName();
        QString name =
            (namePtr && *namePtr != L'\0') ? QString::fromWCharArray(namePtr) : QString();

        if (name.trimmed().isEmpty())
        {
            name = tr("Unnamed controller");
        }

        const QString status = tr("Gamepad ready for capture: %1").arg(name);
        qInfo() << "Gamepad prepared for raw capture:" << name;
        update_raw_status(status);
    }
    else
    {
        update_raw_status(tr("No gamepad detected for capture."));
    }
}

//---------------------------------------------------------
void SettingsDlg::changeEvent(QEvent* e)
//---------------------------------------------------------
{
    QDialog::changeEvent(e);

    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;

    default:
        break;
    }
}

void Ipponboard::SettingsDlg::on_toolButton_play_gong_pressed()
{
    QString path = QDir::currentPath() + "/sounds/" + ui->comboBox_sound_time_ends->currentText();
    qDebug() << "Playing sound from file:" << path;
    m_previewEffect.stop();
    m_previewEffect.setSource(QUrl::fromLocalFile(path));
    m_previewEffect.play();
}

void Ipponboard::SettingsDlg::on_fontComboBox_infoHeader_currentFontChanged(QFont f)
{
    f.setBold(ui->checkBox_text_bold->isChecked());
    f.setItalic(ui->checkBox_text_italic->isChecked());
    ui->text_text_sample->SetFont(f);
}

void Ipponboard::SettingsDlg::on_fontComboBox_fighters_currentFontChanged(QFont f)
{
    f.setBold(ui->checkBox_fighters_bold->isChecked());
    f.setItalic(ui->checkBox_fighters_italic->isChecked());
    ui->text_color_first->SetFont(f);
    ui->text_color_second->SetFont(f);
}

void Ipponboard::SettingsDlg::on_toolButton_text_color_pressed()
{
    QColor col(ui->text_text_sample->GetColor());
    col = QColorDialog::getColor(col, this);

    if (col.isValid())
        ui->text_text_sample->SetColor(col);
}

void Ipponboard::SettingsDlg::on_toolButton_text_background_pressed()
{
    QColor col(ui->text_text_sample->GetBgColor());
    col = QColorDialog::getColor(col, this);

    if (col.isValid())
        ui->text_text_sample->SetColor(ui->text_text_sample->GetColor(), col);
}

void Ipponboard::SettingsDlg::on_toolButton_text_color_first_pressed()
{
    QColor col(ui->text_color_first->GetColor());
    col = QColorDialog::getColor(col, this);

    if (col.isValid())
        ui->text_color_first->SetColor(col);
}

void Ipponboard::SettingsDlg::on_toolButton_text_background_first_pressed()
{
    QColor col(ui->text_color_first->GetBgColor());
    col = QColorDialog::getColor(col, this);

    if (col.isValid())
        ui->text_color_first->SetColor(ui->text_color_first->GetColor(), col);
}

void Ipponboard::SettingsDlg::on_toolButton_text_color_second_pressed()
{
    QColor col(ui->text_color_second->GetColor());
    col = QColorDialog::getColor(col, this);

    if (col.isValid())
        ui->text_color_second->SetColor(col);
}

void Ipponboard::SettingsDlg::on_toolButton_text_background_second_pressed()
{
    QColor col(ui->text_color_second->GetBgColor());
    col = QColorDialog::getColor(col, this);

    if (col.isValid())
        ui->text_color_second->SetColor(ui->text_color_second->GetColor(), col);
}

void Ipponboard::SettingsDlg::on_checkBox_text_bold_toggled(bool checked)
{
    QFont f = ui->text_text_sample->font();
    f.setBold(checked);
    ui->text_text_sample->SetFont(f);
}

void Ipponboard::SettingsDlg::on_checkBox_text_italic_toggled(bool checked)
{
    QFont f = ui->text_text_sample->font();
    f.setItalic(checked);
    ui->text_text_sample->SetFont(f);
}

void Ipponboard::SettingsDlg::on_checkBox_fighters_bold_toggled(bool checked)
{
    QFont f = ui->text_color_first->font();
    f.setBold(checked);
    ui->text_color_first->SetFont(f);
    ui->text_color_second->SetFont(f);
}

void Ipponboard::SettingsDlg::on_checkBox_fighters_italic_toggled(bool checked)
{
    QFont f = ui->text_color_first->font();
    f.setItalic(checked);
    ui->text_color_first->SetFont(f);
    ui->text_color_second->SetFont(f);
}

void Ipponboard::SettingsDlg::on_comboBox_mat_editTextChanged(QString text)
{
    ui->text_text_sample->SetText(text);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Ipponboard::SettingsDlg::on_checkBox_secondary_view_custom_size_toggled(bool checked)
{
    ui->lineEdit_fixedsize_width->setEnabled(checked);
    ui->lineEdit_fixedsize_height->setEnabled(checked);
    ui->label_screen_width->setEnabled(checked);
    ui->label_screen_height->setEnabled(checked);

    if (!checked)
    {
        ui->lineEdit_fixedsize_width->setText("0");
        ui->lineEdit_fixedsize_height->setText("0");
    }
}
void SettingsDlg::prime_raw_capture()
{
    switch (m_captureMode)
    {
    case CaptureMode::Button:
        if (m_rawButtonCapture)
        {
            m_rawButtonCapture->Prime();
        }
        break;
    case CaptureMode::Axis:
        if (m_rawAxisCapture)
        {
            m_rawAxisCapture->Prime();
        }
        break;
    case CaptureMode::None:
    default:
        break;
    }
}

void SettingsDlg::stop_raw_diagnostics(bool quiet)
{
    if (m_rawDiagnosticsTimer.isActive())
    {
        m_rawDiagnosticsTimer.stop();
    }

    if (ui && ui->toolButton_test_raw_mapping)
    {
        QSignalBlocker blocker(ui->toolButton_test_raw_mapping);
        ui->toolButton_test_raw_mapping->setChecked(false);
        ui->toolButton_test_raw_mapping->setText(tr("Show pressed buttons"));
    }

    if (!quiet)
    {
        update_raw_status(tr("Diagnostics stopped."));
    }
}

void SettingsDlg::on_raw_capture_timeout()
{
    switch (m_captureMode)
    {
    case CaptureMode::Button:
        if (m_rawButtonCapture)
        {
            if (const auto code = m_rawButtonCapture->PollButton())
            {
                finish_button_capture(*code);
            }
        }
        break;
    case CaptureMode::Axis:
        if (m_rawAxisCapture)
        {
            if (const auto code = m_rawAxisCapture->PollAxis())
            {
                finish_axis_capture(*code);
            }
        }
        break;
    case CaptureMode::None:
    default:
        if (m_rawCaptureTimer.isActive())
        {
            m_rawCaptureTimer.stop();
        }
        break;
    }
}

void SettingsDlg::update_raw_diagnostics()
{
    if (!m_gamepad || m_gamepad->GetState() != GamepadLib::EGamepadState::ok)
    {
        stop_raw_diagnostics();
        return;
    }

    m_gamepad->ReadData();
    const auto& pressed = m_gamepad->CurrentButtons();

    QStringList buttonEntries;
    buttonEntries.reserve(static_cast<int>(pressed.size()));

    for (const auto code : pressed)
    {
        if (code == GamepadLib::Constants::PovCenteredVal)
        {
            continue;
        }

        const auto description = describe_pressed_button(code);
        if (!description.isEmpty())
        {
            buttonEntries.push_back(description);
        }
    }

    buttonEntries.sort(Qt::CaseInsensitive);

    QStringList lines;
    if (buttonEntries.isEmpty())
    {
        lines << tr("Buttons: none");
    }
    else
    {
        lines << tr("Buttons: %1").arg(buttonEntries.join(QStringLiteral(", ")));
    }

    lines.append(describe_axes_state());
    update_raw_status(lines.join(QStringLiteral("\n")));
}

void SettingsDlg::on_toolButton_test_raw_mapping_toggled(bool checked)
{
    if (checked)
    {
        if (!m_gamepad)
        {
            update_raw_status(tr("No gamepad detected for diagnostics."));
            stop_raw_diagnostics(true);
            return;
        }

        if (m_gamepad->GetState() != GamepadLib::EGamepadState::ok)
        {
            update_raw_status(tr("Gamepad not ready for diagnostics."));
            stop_raw_diagnostics(true);
            return;
        }

        if (m_captureMode != CaptureMode::None)
        {
            update_raw_status(tr("Stop capture before starting diagnostics."));
            stop_raw_diagnostics(true);
            return;
        }

        if (ui && ui->toolButton_test_raw_mapping)
        {
            ui->toolButton_test_raw_mapping->setText(tr("Stop diagnostics"));
        }

        update_raw_diagnostics();
        m_rawDiagnosticsTimer.start();
        return;
    }

    stop_raw_diagnostics();
}

QStringList SettingsDlg::describe_axes_state() const
{
    QStringList result;

    if (!m_gamepad)
    {
        return result;
    }

    const auto& actions = GamepadSectionMapper::Actions();

    using EAxis = GamepadLib::EAxis;

    const auto currSectionXY = m_gamepad->GetSection(EAxis::X, EAxis::Y);
    QString mapping =
        currSectionXY == 0 ? tr("Neutral") : describe_section_action(actions[currSectionXY]);
    result << tr("%1: %2 (X=%4, Y=%6)")
                  .arg(tr("Left stick"),
                       mapping,
                       QString::number(m_gamepad->GetPos(EAxis::X)),
                       QString::number(m_gamepad->GetPos(EAxis::Y)));

    const auto currSectionRZ = m_gamepad->GetSection(EAxis::R, EAxis::Z);
    mapping = currSectionRZ == 0 ? tr("Neutral") : describe_section_action(actions[currSectionRZ]);
    result << tr("%1: %2 (R=%4, Z=%6)")
                  .arg(tr("Right stick"),
                       mapping,
                       QString::number(m_gamepad->GetPos(EAxis::R)),
                       QString::number(m_gamepad->GetPos(EAxis::Z)));

    return result;
}

QString SettingsDlg::describe_section_action(
    const GamepadSectionMapper::SectionAction& action) const
{
    QString actionName;

    switch (action.action)
    {
    case eAction_Ippon:
        actionName = tr("Ippon");
        break;
    case eAction_Wazaari:
        actionName = tr("Wazaari");
        break;
    case eAction_Yuko:
        actionName = tr("Yuko");
        break;
    case eAction_Shido:
        actionName = tr("Shido");
        break;
    default:
        actionName = tr("Unknown");
        break;
    }

    const QString mode = action.revoke ? tr("revoke") : tr("award");
    return tr("(%1) %2").arg(mode, actionName);
}
