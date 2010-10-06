#ifndef SETTINGSDLG_HEADER_INCLUDED
#define SETTINGSDLG_HEADER_INCLUDED

#include <QDialog>
#include <map>

namespace Ui {
	class SettingsDlg;
}

namespace Ipponboard
{

class SettingsDlg : public QDialog
 {
	Q_OBJECT
public:
	SettingsDlg(QWidget* parent = 0);
	~SettingsDlg();

	void SetScreensSettings(bool showAlways, int screen, bool autosize,
							const QSize& dimensions);
	void SetTextSettings(const QFont& font, const QColor& color,
						 const QColor& background);
	void SetTextColorsBlue(const QColor& color, const QColor& background);
	void SetTextColorsWhite(const QColor& color, const QColor& background);
	void SetDigitSettings(const QFont& font, const QColor& color,
						  const QColor& background);
	void SetGongFile(const QString& path);

	bool IsShowAlways() const;
	int GetSelectedScreen() const;
	bool IsAutoSize() const;
	QSize GetSize() const;
	const QFont GetTextFont() const;
	const QFont GetDigitFont() const;
	const QColor GetInfoTextColor() const;
	const QColor GetTextColorBlue() const;
	const QColor GetTextColorWhite() const;
	const QColor GetDigitColor() const;
	const QColor GetInfoTextBgColor() const;
	const QColor GetTextBgColorBlue() const;
	const QColor GetTextBgColorWhite() const;
	const QColor GetDigitBgColor() const;
	const QString GetGongFile() const;

	void SetButtonHajimeMatte(int b);
	void SetButtonOsaekomiToketa(int b);
	void SetButtonNext(int b);
	void SetButtonPrev(int b);
	void SetButtonPause(int b);
	void SetButtonReset(int b);
	void SetButtonReset2(int b);
	void SetButtonResetHold(int b);
	void SetButtonResetHold2(int b);
	void SetButtonBlueHolding(int b);
	void SetButtonWhiteHolding(int b);
	void SetButtonHansokumakeBlue(int b);
	void SetButtonHansokumakeWhite(int b);

	int GetButtonHajimeMatte() const;
	int GetButtonOsaekomiToketa() const;
	int GetButtonNext() const;
	int GetButtonPrev() const;
	int GetButtonPause() const;
	int GetButtonReset() const;
	int GetButtonReset2() const;
	int GetButtonResetHold() const;
	int GetButtonResetHold2() const;
	int GetButtonBlueHolding() const;
	int GetButtonWhiteHolding() const;
	int GetButtonHansokumakeBlue() const;
	int GetButtonHansokumakeWhite() const;

	void SetInvertedX(bool);
	void SetInvertedY(bool);
	void SetInvertedR(bool);
	void SetInvertedZ(bool);

	bool IsInvertedX() const;
	bool IsInvertedY() const;
	bool IsInvertedR() const;
	bool IsInvertedZ() const;

protected:
	void changeEvent(QEvent *e);

private:
	int GetButtonFromText_(const QString& text) const;

private:
	Ui::SettingsDlg *ui;
	typedef std::map<int, QString> ButtonTextMap;
	ButtonTextMap m_buttonTexts;

private slots:


private slots:
	void on_toolButton_text_background_white_pressed();
	void on_toolButton_text_color_white_pressed();
	void on_toolButton_text_background_blue_pressed();
	void on_toolButton_text_color_blue_pressed();
	void on_toolButton_text_background_pressed();	//TODO: somehow the buttons stay pressed...
	void on_toolButton_text_color_pressed();		//TODO: somehow the buttons stay pressed...
	void on_fontComboBox_text_currentFontChanged(QFont f);
	void on_toolButton_play_gong_pressed();
	void on_buttonBox_rejected();
	void on_buttonBox_accepted();
};

} // namespace Ipponboard
#endif // SETTINGSDLG_HEADER_INCLUDED
