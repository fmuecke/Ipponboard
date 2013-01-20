#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include <QStringList>
#include <QDesktopWidget>
#include <QFile>
#include <QDir>
#include <QSound>
#include <QColorDialog>
#include "../gamepad/gamepad.h"
#include "../core/controlconfig.h"

using namespace Ipponboard;

SettingsDlg::SettingsDlg(QWidget* parent) :
    QDialog(parent),
    ui(new Ui::SettingsDlg)
{
    ui->setupUi(this);

#ifdef TEAM_VIEW
    ui->comboBox_next->setEnabled(true);
    ui->comboBox_prev->setEnabled(true);
    ui->comboBox_pause->setEnabled(true);

    ui->label_home->setEnabled(true);
    ui->label_guest->setEnabled(true);
    ui->lineEdit_labelHome->setEnabled(true);
    ui->lineEdit_labelGuest->setEnabled(true);
#else
    ui->label_home->setEnabled(false);
    ui->label_guest->setEnabled(false);
    ui->lineEdit_labelHome->setEnabled(false);
    ui->lineEdit_labelGuest->setEnabled(false);
#endif

    //TODO: remove comment
    //ui->tabWidget->removeTab(ui->tabWidget->count() - 1);

    ui->text_text_sample->SetText("  www.ipponboard.info   ");
    ui->text_color_first->SetText(tr("FIRST FIGHTER"));
    ui->text_color_second->SetText(tr("SECOND FIGHTER"));

    // build button text map
    m_buttonTexts[FMlib::Gamepad::eButton1] = "button 1";
    m_buttonTexts[FMlib::Gamepad::eButton2] = "button 2";
    m_buttonTexts[FMlib::Gamepad::eButton3] = "button 3";
    m_buttonTexts[FMlib::Gamepad::eButton4] = "button 4";
    m_buttonTexts[FMlib::Gamepad::eButton5] = "button 5";
    m_buttonTexts[FMlib::Gamepad::eButton6] = "button 6";
    m_buttonTexts[FMlib::Gamepad::eButton7] = "button 7";
    m_buttonTexts[FMlib::Gamepad::eButton8] = "button 8";
    m_buttonTexts[FMlib::Gamepad::eButton9] = "button 9";
    m_buttonTexts[FMlib::Gamepad::eButton10] = "button 10";
    m_buttonTexts[FMlib::Gamepad::eButton11] = "button 11";
    m_buttonTexts[FMlib::Gamepad::eButton12] = "button 12";
    // ! Assure that pov values do not overlap with regular buttons! !
    m_buttonTexts[FMlib::Gamepad::eButton_pov_fwd] = "POV fwd";
    m_buttonTexts[FMlib::Gamepad::eButton_pov_back] = "POV back";
    m_buttonTexts[FMlib::Gamepad::eButton_pov_left] = "POV left";
    m_buttonTexts[FMlib::Gamepad::eButton_pov_right] = "POV right";
    m_buttonTexts[FMlib::Gamepad::eButton_pov_right_fwd] = "POV right fwd";
    m_buttonTexts[FMlib::Gamepad::eButton_pov_right_back] = "POV right back";
    m_buttonTexts[FMlib::Gamepad::eButton_pov_left_back] = "POV left back";
    m_buttonTexts[FMlib::Gamepad::eButton_pov_left_fwd] = "POV left fwd";

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
    int screens = QApplication::desktop()->numScreens();

    for (int i(1); i <= screens; ++i)
    {
        QRect res = QApplication::desktop()->screenGeometry(i-1);

        ui->comboBox_screen->addItem(
                    QString("%1 (%2x%3)").arg(
                    QString::number(i),
                    QString::number(res.width()),
                    QString::number(res.height())
                    ));
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
    keyVals << "ENTER";			// VK_RETURN
    keyVals << "BACKSPACE";		// VK_BACK
    keyVals << "SPACE";			// VK_SPACE
    keyVals << "TAB";			// VK_TAB
    keyVals << "INS";			// VK_INSERT
    keyVals << "DEL";			// VK_DELETE
    keyVals << "HOME";			// VK_HOME
    keyVals << "END";			// VK_END
    keyVals << "PAGE UP";		// VK_PRIOR
    keyVals << "PAGE DOWN";		// VK_NEXT
    keyVals << "PAUSE";			// VK_PAUSE
    keyVals << "LEFT ARROW";	// VK_LEFT
    keyVals << "UP ARROW";		// VK_UP
    keyVals << "DOWN ARROW";	// VK_DOWN
    keyVals << "RIGHT ARROW";	// VK_RIGHT
    keyVals << "NUMPAD 0";		// VK_NUMPAD0	60	Numeric keypad 0 key
    keyVals << "NUMPAD 1";		// VK_NUMPAD1	61	Numeric keypad 1 key
    keyVals << "NUMPAD 2";		// VK_NUMPAD2	62	Numeric keypad 2 key
    keyVals << "NUMPAD 3";		// VK_NUMPAD3	63	Numeric keypad 3 key
    keyVals << "NUMPAD 4";		// VK_NUMPAD4	64	Numeric keypad 4 key
    keyVals << "NUMPAD 5";		// VK_NUMPAD5	65	Numeric keypad 5 key
    keyVals << "NUMPAD 6";		// VK_NUMPAD6	66	Numeric keypad 6 key
    keyVals << "NUMPAD 7";		// VK_NUMPAD7	67	Numeric keypad 7 key
    keyVals << "NUMPAD 8";		// VK_NUMPAD8	68	Numeric keypad 8 key
    keyVals << "NUMPAD 9";		// VK_NUMPAD9	69	Numeric keypad 9 key
    keyVals << "NUMPAD *";		// VK_MULTIPLY	6A	Multiply key
    keyVals << "NUMPAD +";		// VK_ADD	6B	Add key
    keyVals << "NUMPAD ENTER";		// VK_SEPARATOR	6C	Separator key
    keyVals << "NUMPAD -";		// VK_SUBTRACT	6D	Subtract key
    keyVals << "NUMPAD ,";		// VK_DECIMAL	6E	Decimal key
    keyVals << "NUMPAD %";		// VK_DIVIDE	6F	Divide key
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

    //FIXME: enable this when keyboard acces is customizable
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

SettingsDlg::~SettingsDlg()
{
    delete ui;
}


void SettingsDlg::SetScreensSettings(bool showAlways,
                                     int screen,
                                     bool autosize,
                                     const QSize& dimensions)
{
    ui->checkBox_open_secondary_view->setChecked(showAlways);

    Q_ASSERT(screen < ui->comboBox_screen->count());
    ui->comboBox_screen->setCurrentIndex(screen);

    ui->checkBox_autosize_secondary_view->setChecked(autosize);
    ui->lineEdit_fixedsize_width->setText(
        QString::number(dimensions.width()));
    ui->lineEdit_fixedsize_height->setText(
        QString::number(dimensions.height()));

    if (!autosize)
    {
        ui->lineEdit_fixedsize_width->setEnabled(true);
        ui->lineEdit_fixedsize_height->setEnabled(true);
    }
}

void SettingsDlg::SetInfoHeaderSettings(const QFont& font,
                                        const QColor& color,
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

void SettingsDlg::SetRules(bool autoIncrement, bool use2013RuleSet)
{
    ui->checkBox_increment_points->setChecked(autoIncrement);
    ui->checkBox_rules_2013->setChecked(use2013RuleSet);
}

bool SettingsDlg::IsShowAlways() const
{
    return ui->checkBox_open_secondary_view->isChecked();
}

int SettingsDlg::GetSelectedScreen() const
{
    return ui->comboBox_screen->currentIndex();
}

bool SettingsDlg::IsAutoSize() const
{
    return ui->checkBox_autosize_secondary_view->isChecked();
}

QSize SettingsDlg::GetSize() const
{
    QSize s;
    s.setWidth(ui->lineEdit_fixedsize_width->text().toInt());
    s.setHeight(ui->lineEdit_fixedsize_height->text().toInt());
    return s;
}

const QFont SettingsDlg::GetInfoHeaderFont() const
{
    QFont f = ui->fontComboBox_infoHeader->currentFont();
    f.setBold(ui->checkBox_text_bold->isChecked());
    f.setItalic(ui->checkBox_text_italic->isChecked());
    return f;
}

const QFont SettingsDlg::GetFighterNameFont() const
{
    QFont f = ui->fontComboBox_fighters->currentFont();
    f.setBold(ui->checkBox_fighters_bold->isChecked());
    f.setItalic(ui->checkBox_fighters_italic->isChecked());
    return f;
}

const QColor SettingsDlg::GetInfoTextColor() const
{
    return ui->text_text_sample->GetColor();
}

const QColor SettingsDlg::GetTextColorFirst() const
{
    return ui->text_color_first->GetColor();
}

const QColor SettingsDlg::GetTextColorSecond() const
{
    return ui->text_color_second->GetColor();
}

const QColor SettingsDlg::GetInfoTextBgColor() const
{
    return ui->text_text_sample->GetBgColor();
}
const QColor SettingsDlg::GetTextBgColorFirst() const
{
    return ui->text_color_first->GetBgColor();
}
const QColor SettingsDlg::GetTextBgColorSecond() const
{
    return ui->text_color_second->GetBgColor();
}

const QString SettingsDlg::GetMatLabel() const
{
    return ui->comboBox_mat->currentText();
}

const QString SettingsDlg::GetHomeLabel() const
{
    return ui->lineEdit_labelHome->text();
}

const QString SettingsDlg::GetGuestLabel() const
{
    return ui->lineEdit_labelGuest->text();
}

const QString SettingsDlg::GetGongFile() const
{
    QString path = QDir::currentPath() + "/sounds/";
    return path + ui->comboBox_sound_time_ends->currentText();
}

const bool SettingsDlg::IsAutoIncrementRule() const
{
    return ui->checkBox_increment_points->isChecked();
}

const bool SettingsDlg::IsUse2013Rules() const
{
    return ui->checkBox_rules_2013->isChecked();
}

void SettingsDlg::on_buttonBox_accepted()
{
    accept();
}

void SettingsDlg::on_buttonBox_rejected()
{
    reject();
}

void SettingsDlg::SetControlConfig(ControlConfig* pConfig)
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
    }
}

void SettingsDlg::GetControlConfig(ControlConfig* pConfig)
{
    Q_ASSERT(pConfig);

    if (pConfig)
    {
        pConfig->button_hajime_mate =
            get_button_from_text(ui->comboBox_hajime_mate->currentText());

        pConfig->button_reset =
            get_button_from_text(ui->comboBox_reset->currentText());

        pConfig->button_reset_2 =
            get_button_from_text(ui->comboBox_reset_2->currentText());

        pConfig->button_next =
            get_button_from_text(ui->comboBox_next->currentText());

        pConfig->button_prev =
            get_button_from_text(ui->comboBox_prev->currentText());

        pConfig->button_pause =
            get_button_from_text(ui->comboBox_pause->currentText());

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
    QString path = QDir::currentPath() + "/sounds/" +
                   ui->comboBox_sound_time_ends->currentText();
    QSound::play(path);
}

void Ipponboard::SettingsDlg::on_fontComboBox_infoHeader_currentFontChanged(
    QFont f)
{
    f.setBold(ui->checkBox_text_bold->isChecked());
    f.setItalic(ui->checkBox_text_italic->isChecked());
    ui->text_text_sample->SetFont(f);
}

void Ipponboard::SettingsDlg::on_fontComboBox_fighters_currentFontChanged(
    QFont f)
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

void Ipponboard::SettingsDlg::on_checkBox_autosize_secondary_view_toggled(bool checked)
{
    ui->lineEdit_fixedsize_width->setEnabled(!checked);
    ui->lineEdit_fixedsize_height->setEnabled(!checked);
    ui->label_screen_width->setEnabled(!checked);
    ui->label_screen_height->setEnabled(!checked);
}

