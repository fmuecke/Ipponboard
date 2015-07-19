#ifndef BASE__VIEW_H_
#define BASE__VIEW_H_

#include <QWidget>
#include <QMouseEvent>

#include "../core/iController.h"
#include "../core/iView.h"
#include "../core/Enums.h"
#include "../core/EditionType.h"

// forwards
namespace Ui { class ScoreViewHorizontal; }

class ScaledImage;

namespace Ipponboard
{

class View : public QWidget, public IView
{
	Q_OBJECT
public:

	enum EType { eTypePrimary, eTypeSecondary };
	enum ColorType { firstFg, firstBg, secondFg, secondBg };
	explicit View(IController* pController,
				  EditionType edition,
				  EType type,
				  QWidget* parent = 0);
	virtual ~View();

	// --- IScoreView ---
	void UpdateView();
	void Reset();
	void SetShowInfoHeader(bool show);

	// others
	void SetMessageText(Ipponboard::FighterEnum who, const QString& msg) const;
	const QFont& GetInfoHeaderFont() const { return m_InfoHeaderFont; }
	const QFont& GetFighterNameFont() const { return m_FighterNameFont; }
	const QFont& GetDigitFont() const { return m_DigitFont; }
	void SetInfoHeaderFont(const QFont& font);
	void SetFighterNameFont(const QFont& font);
	void SetDigitFont(const QFont& font);
	const QColor& GetTextColorFirst() const { return m_TextColorFirst; }
	const QColor& GetTextBgColorFirst() const { return m_TextBgColorFirst; }
	const QColor& GetTextColorSecond() const { return m_TextColorSecond; }
	const QColor& GetTextBgColorSecond() const { return m_TextBgColorSecond; }
	const QColor& GetInfoTextColor() const { return m_InfoTextColor; }
	const QColor& GetInfoTextBgColor() const { return m_InfoTextBgColor; }
	const QColor& GetMainClockColor1() const { return m_MainClockColorRunning; }
	const QColor& GetMainClockColor2() const { return m_MainClockColorStopped; }
	void SetInfoTextColor(const QColor& color, const QColor& bgColor);
	void SetTextColorFirst(const QColor& color, const QColor& bgColor);
	void SetTextColorSecond(const QColor& color, const QColor& bgColor);
	void SetMainClockColor(const QColor& fgColor, const QColor& bgColor);
	void SetMat(const QString& mat) { m_mat = mat; }
	void SetWeight(const QString& weight) { m_weight = weight; }  //TODO: move to controller !!
	void SetCategory(const QString& cat) { m_category = cat; }  //TODO: move to controller !!

protected:
	//void changeEvent( QEvent* event );
	void paintEvent(QPaintEvent* event);
	void mousePressEvent(QMouseEvent* event);

private slots:
	void setOsaekomiFirst_();
	void setOsaekomiSecond_();
	//void yoshi_();
	void resetMainTimerValue_();
	void setMainTimerValue_();
	void blink_();

private:
	void update_ippon(Ipponboard::FighterEnum) const;
	void update_wazaari(Ipponboard::FighterEnum) const;
	void update_yuko(Ipponboard::FighterEnum) const;
	void update_shido(Ipponboard::FighterEnum) const;
	void update_hansokumake(Ipponboard::FighterEnum) const;
	void update_team_score() const;
	void update_hold_clock(const FighterEnum holder, EHoldState state) const;
	Ipponboard::FighterEnum GVF_(const Ipponboard::FighterEnum f) const; // GetViewFighter
	bool is_secondary() const;
	const QColor& get_color(const ColorType t) const;
	void update_colors();
	void show_golden_score(bool show);

	EditionType m_Edition;
	EType m_Type;
	IController* m_pController;
	Ui::ScoreViewHorizontal* ui;

	QFont m_InfoHeaderFont;
	QFont m_FighterNameFont;
	QFont m_DigitFont;
	QColor m_TextColorFirst;
	QColor m_TextBgColorFirst;
	QColor m_TextColorSecond;
	QColor m_TextBgColorSecond;
	QColor m_InfoTextColor;
	QColor m_InfoTextBgColor;
	QColor m_MainClockBgColor;
	QColor m_MainClockColorRunning;
	QColor m_MainClockColorStopped;
	QString m_mat;
	QString m_weight;
	QString m_category;
	bool m_drawIppon;
	bool m_showInfoHeader;
	QTimer* m_pBlinkTimer;
};

} // namespace Ipponboard
#endif  // BASE__VIEW_H_
