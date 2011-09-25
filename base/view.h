#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QMouseEvent>

#include "icontroller.h"
#include "iview.h"
#include "enums.h"

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
	enum ColorType { blueFg, blueBg, whiteFg, whiteBg };
	explicit View( IController* pController,
				   EType type,
				   QWidget *parent = 0 );
	virtual ~View();

	// --- IScoreView ---
	void UpdateView();
	void SetController( IController* pController );
	void Reset();
	void SetShowInfoHeader(bool show);

	// others
	void SetMessageText(Ipponboard::EFighter who, const QString& msg) const;
	const QFont& GetInfoHeaderFont() const { return m_InfoHeaderFont; }
	const QFont& GetFighterNameFont() const { return m_FighterNameFont; }
	const QFont& GetDigitFont() const { return m_DigitFont; }
	void SetInfoHeaderFont( const QFont& font );
	void SetFighterNameFont( const QFont& font );
	void SetDigitFont( const QFont& font );
	const QColor& GetTextColorBlue() const { return m_TextColorBlue; }
	const QColor& GetTextBgColorBlue() const { return m_TextBgColorBlue; }
	const QColor& GetTextColorWhite() const { return m_TextColorWhite; }
	const QColor& GetTextBgColorWhite() const { return m_TextBgColorWhite; }
	const QColor& GetInfoTextColor() const { return m_InfoTextColor; }
	const QColor& GetInfoTextBgColor() const { return m_InfoTextBgColor; }
	const QColor& GetMainClockColor1() const { return m_MainClockColorRunning; }
	const QColor& GetMainClockColor2() const { return m_MainClockColorStopped; }
	void SetInfoTextColor( const QColor& color, const QColor& bgColor );
	void SetTextColorBlue( const QColor& color, const QColor& bgColor );
	void SetTextColorWhite( const QColor& color, const QColor& bgColor );
	void SetMainClockColor( const QColor& fgColor, const QColor& bgColor );
	void SetMat( const QString& mat ) { m_mat = mat; }
	void SetWeight( const QString& weight) { m_weight = weight; } //TODO: move to controller !!
	void SetCategory( const QString& cat) { m_category = cat; } //TODO: move to controller !!

protected:
	//void changeEvent( QEvent* event );
	void paintEvent( QPaintEvent* event );
	void mousePressEvent( QMouseEvent* event );

private slots:
	void setOsaekomiBlue_();
	void setOsaekomiWhite_();
	//void yoshi_();
	void resetMainTimerValue_();
	void blink_();

private:
	void update_ippon( Ipponboard::EFighter ) const;
	void update_wazaari( Ipponboard::EFighter ) const;
	void update_yuko( Ipponboard::EFighter ) const;
	void update_shido( Ipponboard::EFighter ) const;
	void update_hansokumake( Ipponboard::EFighter ) const;
	void update_team_score() const;
	void update_hold_clock(const EFighter holder, EHoldState state) const;
	Ipponboard::EFighter GVF_(const Ipponboard::EFighter f) const; // GetViewFighter
	bool is_secondary() const;
	const QColor& get_color(const ColorType t) const;
	void update_colors();
	void show_golden_score(bool show);

	EType m_Type;
	IController* m_pController;
	Ui::ScoreViewHorizontal* ui;

	QFont m_InfoHeaderFont;
	QFont m_FighterNameFont;
	QFont m_DigitFont;
	QColor m_TextColorBlue;
	QColor m_TextBgColorBlue;
	QColor m_TextColorWhite;
	QColor m_TextBgColorWhite;
	QColor m_InfoTextColor;
	QColor m_InfoTextBgColor;
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
#endif // VIEW_H
