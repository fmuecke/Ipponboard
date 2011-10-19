#ifndef BASE__SETTINGSDLG_H_
#define BASE__SETTINGSDLG_H_

#include <QDialog>
#include <map>

class QComboBox;

namespace Ui {
	class SettingsDlg;
}

namespace Ipponboard
{

struct ControlConfig;

class SettingsDlg : public QDialog
 {
	Q_OBJECT
public:
	SettingsDlg(QWidget* parent = 0);
	~SettingsDlg();

	void SetScreensSettings(bool showAlways, int screen, bool autosize,
							const QSize& dimensions);
	void SetInfoHeaderSettings(const QFont& font, const QColor& color,
						 const QColor& background);
	void SetFighterNameFont( const QFont& font );
	void SetTextColorsBlue(const QColor& color, const QColor& background);
	void SetTextColorsWhite(const QColor& color, const QColor& background);
	void SetDigitSettings(const QFont& font, const QColor& color,
						  const QColor& background);
	void SetMatLabel(QString const& text);
	void SetGongFile(const QString& path);

	bool IsShowAlways() const;
	int GetSelectedScreen() const;
	bool IsAutoSize() const;
	QSize GetSize() const;
	const QFont GetInfoHeaderFont() const;
	const QFont GetFighterNameFont() const;
	const QFont GetDigitFont() const;
	const QColor GetInfoTextColor() const;
	const QColor GetTextColorBlue() const;
	const QColor GetTextColorWhite() const;
	const QColor GetDigitColor() const;
	const QColor GetInfoTextBgColor() const;
	const QColor GetTextBgColorBlue() const;
	const QColor GetTextBgColorWhite() const;
	const QColor GetDigitBgColor() const;
	const QString GetMatLabel() const;
	const QString GetGongFile() const;

	void SetControlConfig(ControlConfig* pConfig);
	void GetControlConfig(ControlConfig* pConfig);

protected:
	void changeEvent(QEvent *e);

private:
	int get_button_from_text(const QString& text) const;
	void set_button_value(QComboBox* pCombo, int buttonId);

private:
	Ui::SettingsDlg *ui;
	typedef std::map<int, QString> ButtonTextMap;
	ButtonTextMap m_buttonTexts;

private slots:


private slots:
	void on_checkBox_text_italic_toggled(bool checked);
	void on_checkBox_text_bold_toggled(bool checked);
	void on_toolButton_text_background_white_pressed();
	void on_toolButton_text_color_white_pressed();
	void on_toolButton_text_background_blue_pressed();
	void on_toolButton_text_color_blue_pressed();
	void on_toolButton_text_background_pressed();	//TODO: somehow the buttons stay pressed...
	void on_toolButton_text_color_pressed();		//TODO: somehow the buttons stay pressed...
	void on_fontComboBox_infoHeader_currentFontChanged(QFont f);
	void on_fontComboBox_fighters_currentFontChanged(QFont f);
	void on_toolButton_play_gong_pressed();
	void on_buttonBox_rejected();
	void on_buttonBox_accepted();
};

} // namespace Ipponboard
#endif  // BASE__SETTINGSDLG_H_
