#include "settingsdlg.h"
#include "ui_settingsdlg.h"
#include <QStringList>
#include <QDesktopWidget>
#include <QFile>
#include <QDir>
#include <QSound>
#include <QColorDialog>
#include "../gamepad/gamepad.h"

using namespace Ipponboard;

SettingsDlg::SettingsDlg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::SettingsDlg)
{
	ui->setupUi(this);

	ui->text_text_sample->SetText("IPPONBOARD");
	ui->text_color_blue->SetText(tr("Blue"));
	ui->text_color_white->SetText(tr("White"));
	ui->text_digit_sample->SetText("3:24");

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
	//m_buttonTexts[FMlib::Gamepad::ePov_up] = "POV U";
	//m_buttonTexts[FMlib::Gamepad::ePov_right] = "POV R";
	//m_buttonTexts[FMlib::Gamepad::ePov_down] = "POV D";
	//m_buttonTexts[FMlib::Gamepad::ePov_left] = "POV L";
	//m_buttonTexts[FMlib::Gamepad::ePov_upper_right] = "POV UR";
	//m_buttonTexts[FMlib::Gamepad::ePov_lower_right] = "POV DR";
	//m_buttonTexts[FMlib::Gamepad::ePov_lower_left] = "POV DL";
	//m_buttonTexts[FMlib::Gamepad::ePov_upper_left] = "POV UL";

	QStringList buttons;
	ButtonTextMap::const_iterator iter = m_buttonTexts.begin();
	while( iter != m_buttonTexts.end() )
	{
		buttons.push_back(iter->second);
		++iter;
	}

	ui->comboBox_next->addItems(buttons);
	ui->comboBox_prev->addItems(buttons);
	ui->comboBox_pause->addItems(buttons);
	ui->comboBox_reset->addItems(buttons);
	ui->comboBox_reset_2->addItems(buttons);
	ui->comboBox_reset_hold->addItems(buttons);
	ui->comboBox_reset_hold_2->addItems(buttons);
	ui->comboBox_hajime_matte->addItem("POV down");
	ui->comboBox_blue_holding->addItems(buttons);
	ui->comboBox_white_holding->addItems(buttons);
	ui->comboBox_hansokumake_blue->addItems(buttons);
	ui->comboBox_hansokumake_white->addItems(buttons);

	// num screens
	int screens = QApplication::desktop()->numScreens();
	for(int i(1); i<=screens; ++i)
	{
		ui->comboBox_screen->addItem(QString::number(i));
	}

	QDir dir( QDir::currentPath() + "/sounds" );
	QStringList nameFilters;
	nameFilters << "*.wav";
	dir.setNameFilters(nameFilters);
	QStringList sounds(	dir.entryList(QDir::Files) );
	ui->comboBox_sound_time_ends->addItems(sounds);
}

SettingsDlg::~SettingsDlg()
{
	delete ui;
}


void SettingsDlg::SetScreensSettings( bool showAlways,
									  int screen,
									  bool autosize,
									  const QSize& dimensions )
{
	ui->checkBox_open_secondary_view->setChecked(showAlways);

	Q_ASSERT(screen < ui->comboBox_screen->count());
	ui->comboBox_screen->setCurrentIndex(screen);

	ui->checkBox_autosize_secondary_view->setChecked(autosize);
	ui->lineEdit_fixedsize_width->setText(
			QString::number(dimensions.width()));
	ui->lineEdit_fixedsize_height->setText(
			QString::number(dimensions.height()));
	if( !autosize )
	{
		ui->lineEdit_fixedsize_width->setEnabled(true);
		ui->lineEdit_fixedsize_height->setEnabled(true);
	}
}

void SettingsDlg::SetTextSettings( const QFont& font,
								   const QColor& color,
								   const QColor& background )
{
	ui->fontComboBox_text->setCurrentFont(font);
	ui->text_text_sample->SetFontAndColor(font, color, background);
	ui->checkBox_text_bold->setChecked(font.bold());
	ui->checkBox_text_italic->setChecked(font.italic());
}

void SettingsDlg::SetTextColorsBlue( const QColor& color, const QColor& background )
{
	ui->text_color_blue->SetColor(color, background);
}

void SettingsDlg::SetTextColorsWhite( const QColor& color, const QColor& background )
{
	ui->text_color_white->SetColor(color, background);
}

void SettingsDlg::SetDigitSettings( const QFont& font,
									const QColor& color,
									const QColor& background )
{
	ui->fontComboBox_digit->setCurrentFont(font);
	ui->text_digit_sample->SetFontAndColor(font, color, background);
	ui->checkBox_digits_bold->setChecked(font.bold());
	ui->checkBox_digits_italic->setChecked(font.italic());
}

void SettingsDlg::SetGongFile( const QString& path )
{
	int pos = path.lastIndexOf('/',-1);
	QString filename;
	if( -1 != pos )
		filename = path.right( path.length() - pos -1 );
	else
		filename = path;

	const int index = ui->comboBox_sound_time_ends->findText(filename);
	if( -1 != index )
		ui->comboBox_sound_time_ends->setCurrentIndex(index);
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

const QFont SettingsDlg::GetTextFont() const
{
	QFont f = ui->fontComboBox_text->currentFont();
	f.setBold(ui->checkBox_text_bold->isChecked());
	f.setItalic(ui->checkBox_text_italic->isChecked());
	return f;
}
const QFont SettingsDlg::GetDigitFont() const
{
	return ui->fontComboBox_digit->currentFont();
}

const QColor SettingsDlg::GetInfoTextColor() const
{
	return ui->text_text_sample->GetColor();
}

const QColor SettingsDlg::GetTextColorBlue() const
{
	return ui->text_color_blue->GetColor();
}

const QColor SettingsDlg::GetTextColorWhite() const
{
	return ui->text_color_white->GetColor();
}

const QColor SettingsDlg::GetDigitColor() const
{
	return ui->text_digit_sample->GetColor();
}

const QColor SettingsDlg::GetInfoTextBgColor() const
{
	return ui->text_text_sample->GetBgColor();
}
const QColor SettingsDlg::GetTextBgColorBlue() const
{
	return ui->text_color_blue->GetBgColor();
}
const QColor SettingsDlg::GetTextBgColorWhite() const
{
	return ui->text_color_white->GetBgColor();
}
const QColor SettingsDlg::GetDigitBgColor() const
{
	return ui->text_digit_sample->GetBgColor();
}

const QString SettingsDlg::GetGongFile() const
{
	QString path = QDir::currentPath() + "/sounds/";
	return path + ui->comboBox_sound_time_ends->currentText();
}

void SettingsDlg::on_buttonBox_accepted()
{
	accept();
}

void SettingsDlg::on_buttonBox_rejected()
{
	reject();
}

void SettingsDlg::SetButtonHajimeMatte(int /*b*/)
{
	//int index = ui->comboBox_hajime_matte->findText(m_buttonTexts[b]);
	//ui->comboBox_hajime_matte->setCurrentIndex(index);
}

void SettingsDlg::SetButtonOsaekomiToketa(int /*b*/)
{
	//int index = ui->comboBox_osaekomi_toketa->findText(m_buttonTexts[b]);
	//ui->comboBox_osaekomi_toketa->setCurrentIndex(index);
}

void SettingsDlg::SetButtonNext(int b)
{
	int index = ui->comboBox_next->findText(m_buttonTexts[b]);
	ui->comboBox_next->setCurrentIndex(index);
}

void SettingsDlg::SetButtonPrev(int b)
{
	int index = ui->comboBox_prev->findText(m_buttonTexts[b]);
	ui->comboBox_prev->setCurrentIndex(index);
}

void SettingsDlg::SetButtonPause(int b)
{
	int index = ui->comboBox_pause->findText(m_buttonTexts[b]);
	ui->comboBox_pause->setCurrentIndex(index);
}

void SettingsDlg::SetButtonReset(int b)
{
	int index = ui->comboBox_reset->findText(m_buttonTexts[b]);
	ui->comboBox_reset->setCurrentIndex(index);
}

void SettingsDlg::SetButtonReset2(int b)
{
	int index = ui->comboBox_reset_2->findText(m_buttonTexts[b]);
	ui->comboBox_reset_2->setCurrentIndex(index);
}

void SettingsDlg::SetButtonResetHoldBlue(int b)
{
	int index = ui->comboBox_reset_hold->findText(m_buttonTexts[b]);
	ui->comboBox_reset_hold->setCurrentIndex(index);
}

void SettingsDlg::SetButtonResetHoldWhite(int b)
{
	int index = ui->comboBox_reset_hold_2->findText(m_buttonTexts[b]);
	ui->comboBox_reset_hold_2->setCurrentIndex(index);
}

void SettingsDlg::SetButtonBlueHolding(int b)
{
	int index = ui->comboBox_blue_holding->findText(m_buttonTexts[b]);
	ui->comboBox_blue_holding->setCurrentIndex(index);
}

void SettingsDlg::SetButtonWhiteHolding(int b)
{
	int index = ui->comboBox_white_holding->findText(m_buttonTexts[b]);
	ui->comboBox_white_holding->setCurrentIndex(index);
}

void SettingsDlg::SetButtonHansokumakeBlue(int b)
{
	int index = ui->comboBox_hansokumake_blue->findText(m_buttonTexts[b]);
	ui->comboBox_hansokumake_blue->setCurrentIndex(index);
}

void SettingsDlg::SetButtonHansokumakeWhite(int b)
{
	int index = ui->comboBox_hansokumake_white->findText(m_buttonTexts[b]);
	ui->comboBox_hansokumake_white->setCurrentIndex(index);
}

int SettingsDlg::GetButtonHajimeMatte() const
{
	return FMlib::Gamepad::ePov_down;//GetButtonFromText_(ui->comboBox_hajime_matte->currentText());
}
int SettingsDlg::GetButtonOsaekomiToketa() const
{
	return FMlib::Gamepad::ePov_up;//GetButtonFromText_(ui->comboBox_osaekomi_toketa->currentText());
}

int SettingsDlg::GetButtonNext() const
{
	return GetButtonFromText_(ui->comboBox_next->currentText());
}

int SettingsDlg::GetButtonPrev() const
{
	return GetButtonFromText_(ui->comboBox_prev->currentText());
}

int SettingsDlg::GetButtonPause() const
{
	return GetButtonFromText_(ui->comboBox_pause->currentText());
}

int SettingsDlg::GetButtonReset() const
{
	return GetButtonFromText_(ui->comboBox_reset->currentText());
}

int SettingsDlg::GetButtonReset2() const
{
	return GetButtonFromText_(ui->comboBox_reset_2->currentText());
}

int SettingsDlg::GetButtonResetHold() const
{
	return GetButtonFromText_(ui->comboBox_reset_hold->currentText());
}

int SettingsDlg::GetButtonResetHold2() const
{
	return GetButtonFromText_(ui->comboBox_reset_hold_2->currentText());
}

int SettingsDlg::GetButtonBlueHolding() const
{
	return GetButtonFromText_(ui->comboBox_blue_holding->currentText());
}

int SettingsDlg::GetButtonWhiteHolding() const
{
	return GetButtonFromText_(ui->comboBox_white_holding->currentText());
}

int SettingsDlg::GetButtonHansokumakeBlue() const
{
	return GetButtonFromText_(ui->comboBox_hansokumake_blue->currentText());
}

int SettingsDlg::GetButtonHansokumakeWhite() const
{
	return GetButtonFromText_(ui->comboBox_hansokumake_white->currentText());
}

void SettingsDlg::SetInvertedX(bool inverted)
{
	ui->checkBox_invert_x_axis->setChecked(inverted);
}
void SettingsDlg::SetInvertedY(bool inverted)
{
	ui->checkBox_invert_y_axis->setChecked(inverted);
}
void SettingsDlg::SetInvertedR(bool inverted)
{
	ui->checkBox_invert_r_axis->setChecked(inverted);
}
void SettingsDlg::SetInvertedZ(bool inverted)
{
	ui->checkBox_invert_z_axis->setChecked(inverted);
}

bool SettingsDlg::IsInvertedX() const
{
	return ui->checkBox_invert_x_axis->isChecked();
}
bool SettingsDlg::IsInvertedY() const
{
	return ui->checkBox_invert_y_axis->isChecked();
}
bool SettingsDlg::IsInvertedR() const
{
	return ui->checkBox_invert_r_axis->isChecked();
}
bool SettingsDlg::IsInvertedZ() const
{
	return ui->checkBox_invert_z_axis->isChecked();
}



//---------------------------------------------------------
void SettingsDlg::changeEvent(QEvent *e)
//---------------------------------------------------------
{
	QDialog::changeEvent(e);
	switch (e->type()) {
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int SettingsDlg::GetButtonFromText_(const QString& text) const
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{
	ButtonTextMap::const_iterator iter = m_buttonTexts.begin();
	while( iter != m_buttonTexts.end() )
	{
		if( iter->second == text )
			return iter->first;
		++iter;
	}
	Q_ASSERT(!"element should be in list");
	return -1;
}

void Ipponboard::SettingsDlg::on_toolButton_play_gong_pressed()
{
	QString path = QDir::currentPath() + "/sounds/" +
		ui->comboBox_sound_time_ends->currentText();
	QSound::play( path );
}

void Ipponboard::SettingsDlg::on_fontComboBox_text_currentFontChanged(
		QFont f)
{
	f.setBold(ui->checkBox_text_bold->isChecked());
	f.setItalic(ui->checkBox_text_italic->isChecked());
	ui->text_text_sample->SetFont(f);
}

void Ipponboard::SettingsDlg::on_toolButton_text_color_pressed()
{
	QColor col(ui->text_text_sample->GetColor());
	col = QColorDialog::getColor( col, this );
	if( col.isValid() )
		ui->text_text_sample->SetColor(col);
}

void Ipponboard::SettingsDlg::on_toolButton_text_background_pressed()
{
	QColor col(ui->text_text_sample->GetBgColor());
	col = QColorDialog::getColor( col, this );
	if( col.isValid() )
		ui->text_text_sample->SetColor(ui->text_text_sample->GetColor(), col);
}

void Ipponboard::SettingsDlg::on_toolButton_text_color_blue_pressed()
{
	QColor col(ui->text_color_blue->GetColor());
	col = QColorDialog::getColor( col, this );
	if( col.isValid() )
		ui->text_color_blue->SetColor(col);
}

void Ipponboard::SettingsDlg::on_toolButton_text_background_blue_pressed()
{
	QColor col(ui->text_color_blue->GetBgColor());
	col = QColorDialog::getColor( col, this );
	if( col.isValid() )
		ui->text_color_blue->SetColor(ui->text_color_blue->GetColor(),col);
}

void Ipponboard::SettingsDlg::on_toolButton_text_color_white_pressed()
{
	QColor col(ui->text_color_white->GetColor());
	col = QColorDialog::getColor( col, this );
	if( col.isValid() )
		ui->text_color_white->SetColor(col);
}

void Ipponboard::SettingsDlg::on_toolButton_text_background_white_pressed()
{
	QColor col(ui->text_color_white->GetBgColor());
	col = QColorDialog::getColor( col, this );
	if( col.isValid() )
		ui->text_color_white->SetColor(ui->text_color_white->GetColor(),col);
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
