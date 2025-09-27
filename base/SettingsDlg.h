// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__SETTINGSDLG_H_
#define BASE__SETTINGSDLG_H_

#include "../core/EditionType.h"
#include <QDialog>
#include <QSoundEffect>
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
	SettingsDlg(EditionType edition, QWidget* parent = 0);
	~SettingsDlg();

	void SetScreensSettings(int screen, const QSize& dimensions); // zero dimensions means auto size
	void SetInfoHeaderSettings(const QFont& font, const QColor& color,
							   const QColor& background);
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

protected:
	void changeEvent(QEvent* e);

private:
	int get_button_from_text(const QString& text) const;
	void set_button_value(QComboBox* pCombo, int buttonId);

private:
	EditionType m_edition;
	Ui::SettingsDlg* ui;
	typedef std::map<int, QString> ButtonTextMap;
	ButtonTextMap m_buttonTexts;
    QSoundEffect m_previewEffect;

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
	void on_checkBox_secondary_view_custom_size_toggled(bool checked);
};

} // namespace Ipponboard
#endif  // BASE__SETTINGSDLG_H_
