// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "SettingsDlg.h"

#include "../core/ControllerConfig.h"
#include "../gamepad/Gamepad.h"
#include "ui_SettingsDlg.h"

#include <QColorDialog>
#include <QDebug>
#include <QDesktopWidget>
#include <QDir>
#include <QFile>
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
      m_buttonTexts(),
      m_previewEffect(),
      m_rawCaptureTimer(this),
      m_gamepad(nullptr),
      m_rawButtonCapture(),
      m_rawAxisCapture(),
      m_rawButtonBindings{},
      m_rawAxisBindings{},
      m_captureMode(CaptureMode::None),
      m_activeButtonBinding(nullptr),
      m_activeAxisBinding(nullptr),
      m_rawStatusLabel(nullptr)
{
    ui->setupUi(this);
    m_previewEffect.setParent(this);
    m_previewEffect.setLoopCount(1);
    initialize_raw_bindings();

    if (m_edition == EditionType::Team)
    {
        ui->comboBox_next->setEnabled(true);
        ui->comboBox_prev->setEnabled(true);
        ui->comboBox_pause->setEnabled(true);

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

    //TODO: remove comment
    //ui->tabWidget->removeTab(ui->tabWidget->count() - 1);

    ui->text_text_sample->SetText("  Ipponboard   ");
    ui->text_color_first->SetText(tr("FIRST FIGHTER"));
    ui->text_color_second->SetText(tr("SECOND FIGHTER"));

    // build button text map
    m_buttonTexts[GamepadLib::EButton::button1] = "button 1";
    m_buttonTexts[GamepadLib::EButton::button2] = "button 2";
    m_buttonTexts[GamepadLib::EButton::button3] = "button 3";
    m_buttonTexts[GamepadLib::EButton::button4] = "button 4";
    m_buttonTexts[GamepadLib::EButton::button5] = "button 5";
    m_buttonTexts[GamepadLib::EButton::button6] = "button 6";
    m_buttonTexts[GamepadLib::EButton::button7] = "button 7";
    m_buttonTexts[GamepadLib::EButton::button8] = "button 8";
    m_buttonTexts[GamepadLib::EButton::button9] = "button 9";
    m_buttonTexts[GamepadLib::EButton::button10] = "button 10";
    m_buttonTexts[GamepadLib::EButton::button11] = "button 11";
    m_buttonTexts[GamepadLib::EButton::button12] = "button 12";
    // ! Assure that pov values do not overlap with regular buttons! !
    m_buttonTexts[GamepadLib::EButton::button_pov_fwd] = "POV fwd";
    m_buttonTexts[GamepadLib::EButton::button_pov_back] = "POV back";
    m_buttonTexts[GamepadLib::EButton::button_pov_left] = "POV left";
    m_buttonTexts[GamepadLib::EButton::button_pov_right] = "POV right";
    m_buttonTexts[GamepadLib::EButton::button_pov_right_fwd] = "POV right fwd";
    m_buttonTexts[GamepadLib::EButton::button_pov_right_back] = "POV right back";
    m_buttonTexts[GamepadLib::EButton::button_pov_left_back] = "POV left back";
    m_buttonTexts[GamepadLib::EButton::button_pov_left_fwd] = "POV left fwd";

    QStringList buttons;
    ButtonTextMap::const_iterator iter = m_buttonTexts.begin();

    while (iter != m_buttonTexts.end())
    {
        buttons.push_back(iter->second);
        ++iter;
    }

    ui->comboBox_next->addItems(buttons);
    ui->comboBox_prev->addItems(buttons);
    ui->comboBox_pause->addItems(buttons);
    ui->comboBox_reset->addItems(buttons);
    ui->comboBox_reset_2->addItems(buttons);
    ui->comboBox_reset_hold_first->addItems(buttons);
    ui->comboBox_reset_hold_second->addItems(buttons);
    ui->comboBox_hajime_mate->addItems(buttons);
    ui->comboBox_first_holding->addItems(buttons);
    ui->comboBox_second_holding->addItems(buttons);
    ui->comboBox_hansokumake_first->addItems(buttons);
    ui->comboBox_hansokumake_second->addItems(buttons);

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

    auto* validator = new QIntValidator(-1, std::numeric_limits<std::uint16_t>::max(), this);

    m_rawButtonBindings = {
        RawButtonBinding{ ui->lineEdit_raw_hajime_mate,
                          ui->pushButton_capture_raw_hajime_mate,
                          &ControllerConfig::button_hajime_mate_raw },
        RawButtonBinding{ ui->lineEdit_raw_next,
                          ui->pushButton_capture_raw_next,
                          &ControllerConfig::button_next_raw },
        RawButtonBinding{ ui->lineEdit_raw_prev,
                          ui->pushButton_capture_raw_prev,
                          &ControllerConfig::button_prev_raw },
        RawButtonBinding{ ui->lineEdit_raw_pause,
                          ui->pushButton_capture_raw_pause,
                          &ControllerConfig::button_pause_raw },
        RawButtonBinding{ ui->lineEdit_raw_reset_first,
                          ui->pushButton_capture_raw_reset_first,
                          &ControllerConfig::button_reset_raw },
        RawButtonBinding{ ui->lineEdit_raw_reset_second,
                          ui->pushButton_capture_raw_reset_second,
                          &ControllerConfig::button_reset2_raw },
        RawButtonBinding{ ui->lineEdit_raw_osaekomi_first,
                          ui->pushButton_capture_raw_osaekomi_first,
                          &ControllerConfig::button_osaekomi_toketa_first_raw },
        RawButtonBinding{ ui->lineEdit_raw_osaekomi_second,
                          ui->pushButton_capture_raw_osaekomi_second,
                          &ControllerConfig::button_osaekomi_toketa_second_raw },
        RawButtonBinding{ ui->lineEdit_raw_reset_hold_first,
                          ui->pushButton_capture_raw_reset_hold_first,
                          &ControllerConfig::button_reset_hold_first_raw },
        RawButtonBinding{ ui->lineEdit_raw_reset_hold_second,
                          ui->pushButton_capture_raw_reset_hold_second,
                          &ControllerConfig::button_reset_hold_second_raw },
        RawButtonBinding{ ui->lineEdit_raw_hansokumake_first,
                          ui->pushButton_capture_raw_hansokumake_first,
                          &ControllerConfig::button_hansokumake_first_raw },
        RawButtonBinding{ ui->lineEdit_raw_hansokumake_second,
                          ui->pushButton_capture_raw_hansokumake_second,
                          &ControllerConfig::button_hansokumake_second_raw },
    };

    for (auto& binding : m_rawButtonBindings)
    {
        if (binding.lineEdit)
        {
            binding.lineEdit->setValidator(validator);
            binding.lineEdit->setPlaceholderText(tr("-1"));
        }

        if (binding.captureButton)
        {
            auto* bindingPtr = &binding;
            connect(binding.captureButton,
                    &QPushButton::clicked,
                    this,
                    [this, bindingPtr]() { start_button_capture(bindingPtr); });
        }
    }

    m_rawAxisBindings = {
        RawAxisBinding{ ui->lineEdit_raw_axis_left_x,
                        ui->pushButton_capture_raw_axis_left_x,
                        ui->checkBox_raw_axis_left_x_invert,
                        &ControllerConfig::axis_left_x,
                        &ControllerConfig::axis_left_invert_x },
        RawAxisBinding{ ui->lineEdit_raw_axis_left_y,
                        ui->pushButton_capture_raw_axis_left_y,
                        ui->checkBox_raw_axis_left_y_invert,
                        &ControllerConfig::axis_left_y,
                        &ControllerConfig::axis_left_invert_y },
        RawAxisBinding{ ui->lineEdit_raw_axis_right_x,
                        ui->pushButton_capture_raw_axis_right_x,
                        ui->checkBox_raw_axis_right_x_invert,
                        &ControllerConfig::axis_right_x,
                        &ControllerConfig::axis_right_invert_x },
        RawAxisBinding{ ui->lineEdit_raw_axis_right_y,
                        ui->pushButton_capture_raw_axis_right_y,
                        ui->checkBox_raw_axis_right_y_invert,
                        &ControllerConfig::axis_right_y,
                        &ControllerConfig::axis_right_invert_y },
    };

    for (auto& binding : m_rawAxisBindings)
    {
        if (binding.lineEdit)
        {
            binding.lineEdit->setValidator(validator);
            binding.lineEdit->setPlaceholderText(tr("-1"));
        }

        if (binding.captureButton)
        {
            auto* bindingPtr = &binding;
            connect(binding.captureButton,
                    &QPushButton::clicked,
                    this,
                    [this, bindingPtr]() { start_axis_capture(bindingPtr); });
        }
    }

    const auto connectMirror = [this](QCheckBox* source, QCheckBox* mirror)
    {
        if (!source || !mirror)
        {
            return;
        }

        connect(source,
                &QCheckBox::toggled,
                this,
                [mirror](bool checked)
                {
                    QSignalBlocker blocker(mirror);
                    mirror->setChecked(checked);
                });

        connect(mirror,
                &QCheckBox::toggled,
                this,
                [source](bool checked)
                {
                    QSignalBlocker blocker(source);
                    source->setChecked(checked);
                });
    };

    connectMirror(ui->checkBox_raw_axis_left_x_invert, ui->checkBox_invert_x_axis);
    connectMirror(ui->checkBox_raw_axis_left_y_invert, ui->checkBox_invert_y_axis);
    connectMirror(ui->checkBox_raw_axis_right_x_invert, ui->checkBox_invert_r_axis);
    connectMirror(ui->checkBox_raw_axis_right_y_invert, ui->checkBox_invert_z_axis);

    m_rawStatusLabel = ui->label_raw_status;
    update_raw_status(tr("Press Capture to record a button or axis code."));
}

void SettingsDlg::start_button_capture(RawButtonBinding* binding)
{
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
    if (m_rawStatusLabel)
    {
        m_rawStatusLabel->setText(message);
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

SettingsDlg::~SettingsDlg()
{
    m_previewEffect.stop();
    m_previewEffect.setSource(QUrl());
    cancel_raw_capture();
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
        set_button_value(ui->comboBox_hajime_mate, pConfig->button_hajime_mate);
        set_button_value(ui->comboBox_reset, pConfig->button_reset);
        set_button_value(ui->comboBox_reset_2, pConfig->button_reset_2);
        set_button_value(ui->comboBox_next, pConfig->button_next);
        set_button_value(ui->comboBox_prev, pConfig->button_prev);
        set_button_value(ui->comboBox_pause, pConfig->button_pause);
        set_button_value(ui->comboBox_first_holding, pConfig->button_osaekomi_toketa_first);
        set_button_value(ui->comboBox_second_holding, pConfig->button_osaekomi_toketa_second);
        set_button_value(ui->comboBox_reset_hold_first, pConfig->button_reset_hold_first);
        set_button_value(ui->comboBox_reset_hold_second, pConfig->button_reset_hold_second);
        set_button_value(ui->comboBox_hansokumake_first, pConfig->button_hansokumake_first);
        set_button_value(ui->comboBox_hansokumake_second, pConfig->button_hansokumake_second);

        ui->checkBox_invert_x_axis->setChecked(pConfig->axis_inverted_X);
        ui->checkBox_invert_y_axis->setChecked(pConfig->axis_inverted_Y);
        ui->checkBox_invert_r_axis->setChecked(pConfig->axis_inverted_R);
        ui->checkBox_invert_z_axis->setChecked(pConfig->axis_inverted_Z);

        for (const auto& binding : m_rawButtonBindings)
        {
            if (!binding.lineEdit)
            {
                continue;
            }

            const int value = pConfig->*(binding.configMember);
            binding.lineEdit->setText(format_raw_value(value));
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
                binding.invertCheckBox->setChecked(pConfig->*(binding.invertMember));
            }
        }
    }
}

void SettingsDlg::GetControllerConfig(ControllerConfig* pConfig)
{
    Q_ASSERT(pConfig);

    if (pConfig)
    {
        pConfig->button_hajime_mate = get_button_from_text(ui->comboBox_hajime_mate->currentText());

        pConfig->button_reset = get_button_from_text(ui->comboBox_reset->currentText());

        pConfig->button_reset_2 = get_button_from_text(ui->comboBox_reset_2->currentText());

        pConfig->button_next = get_button_from_text(ui->comboBox_next->currentText());

        pConfig->button_prev = get_button_from_text(ui->comboBox_prev->currentText());

        pConfig->button_pause = get_button_from_text(ui->comboBox_pause->currentText());

        pConfig->button_osaekomi_toketa_first =
            get_button_from_text(ui->comboBox_first_holding->currentText());

        pConfig->button_osaekomi_toketa_second =
            get_button_from_text(ui->comboBox_second_holding->currentText());

        pConfig->button_reset_hold_first =
            get_button_from_text(ui->comboBox_reset_hold_first->currentText());

        pConfig->button_reset_hold_second =
            get_button_from_text(ui->comboBox_reset_hold_second->currentText());

        pConfig->button_hansokumake_first =
            get_button_from_text(ui->comboBox_hansokumake_first->currentText());

        pConfig->button_hansokumake_second =
            get_button_from_text(ui->comboBox_hansokumake_second->currentText());

        pConfig->axis_inverted_X = ui->checkBox_invert_x_axis->isChecked();
        pConfig->axis_inverted_Y = ui->checkBox_invert_y_axis->isChecked();
        pConfig->axis_inverted_R = ui->checkBox_invert_r_axis->isChecked();
        pConfig->axis_inverted_Z = ui->checkBox_invert_z_axis->isChecked();

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
                pConfig->*(binding.invertMember) = binding.invertCheckBox->isChecked();
            }
        }
    }
}

void SettingsDlg::SetGamepad(GamepadLib::Gamepad* gamepad) noexcept
{
    m_gamepad = gamepad;

    if (m_gamepad)
    {
        const auto* namePtr = m_gamepad->GetProductName();
        const QString name = (namePtr && *namePtr != L'\0') ? QString::fromWCharArray(namePtr)
                                                            : tr("Unknown controller");
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
int SettingsDlg::get_button_from_text(const QString& text) const
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{
    ButtonTextMap::const_iterator iter = m_buttonTexts.begin();

    while (iter != m_buttonTexts.end())
    {
        if (iter->second == text)
            return iter->first;

        ++iter;
    }

    Q_ASSERT(!"element should be in list");
    return -1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Ipponboard::SettingsDlg::set_button_value(QComboBox* pCombo, int buttonId)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{
    int index = pCombo->findText(m_buttonTexts[buttonId]);
    pCombo->setCurrentIndex(index);
}

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
