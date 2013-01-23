#ifndef BASE__SETTINGSDLG_H_
#define BASE__SETTINGSDLG_H_

#include <QDialog>
#include <map>

class QComboBox;

namespace Ui
{
class SettingsDlg;
}

namespace Ipponboard
{

struct ControllerConfig;

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
	void SetFighterNameFont(const QFont& font);
	void SetTextColorsFirst(const QColor& color, const QColor& background);
	void SetTextColorsSecond(const QColor& color, const QColor& background);
    void SetLabels(QString const& mat, QString const& home, QString const& guest);
	void SetGongFile(const QString& path);

    void SetRules(bool autoIncrement, bool use2013RuleSet);

    bool IsShowAlways() const;
	int GetSelectedScreen() const;
	bool IsAutoSize() const;
	QSize GetSize() const;
	const QFont GetInfoHeaderFont() const;
	const QFont GetFighterNameFont() const;
	const QColor GetInfoTextColor() const;
	const QColor GetTextColorFirst() const;
	const QColor GetTextColorSecond() const;
	const QColor GetInfoTextBgColor() const;
	const QColor GetTextBgColorFirst() const;
	const QColor GetTextBgColorSecond() const;
	const QString GetMatLabel() const;
    const QString GetHomeLabel() const;
    const QString GetGuestLabel() const;
	const QString GetGongFile() const;
    const bool IsAutoIncrementRule() const;
    const bool IsUse2013Rules() const;

    void SetControllerConfig(const ControllerConfig* pConfig);
    void GetControllerConfig(ControllerConfig* pConfig);

protected:
	void changeEvent(QEvent* e);

private:
	int get_button_from_text(const QString& text) const;
	void set_button_value(QComboBox* pCombo, int buttonId);

private:
	Ui::SettingsDlg* ui;
	typedef std::map<int, QString> ButtonTextMap;
	ButtonTextMap m_buttonTexts;

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
	void on_toolButton_text_background_pressed();	//TODO: somehow the buttons stay pressed...
	void on_toolButton_text_color_pressed();		//TODO: somehow the buttons stay pressed...
	void on_fontComboBox_infoHeader_currentFontChanged(QFont f);
	void on_fontComboBox_fighters_currentFontChanged(QFont f);
	void on_toolButton_play_gong_pressed();
	void on_buttonBox_rejected();
	void on_buttonBox_accepted();
    void on_checkBox_autosize_secondary_view_toggled(bool checked);
};

} // namespace Ipponboard
#endif  // BASE__SETTINGSDLG_H_
