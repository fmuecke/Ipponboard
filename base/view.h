#ifndef VIEW_H
#define VIEW_H

#include <QWidget>
#include <QMouseEvent>

#include "icontroller.h"
#include "iview.h"

// forwards
#ifdef HORIZONTAL_VIEW
namespace Ui { class ScoreViewHorizontal; }
#else
# ifdef TEAM_VIEW
namespace Ui { class ScoreViewVerticalTeam; }
# else
namespace Ui { class ScoreViewVerticalSingle; }
# endif
#endif

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

	// others
	void SetMessageText(Ipponboard::EFighter who, const QString& msg) const;
	const QFont& GetTextFont() const { return m_TextFont; }
	const QFont& GetDigitFont() const { return m_DigitFont; }
	void SetTextFont( const QFont& font );
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
	void setOsaekomiTimerValue_();
	void resetOsaekomiTimerValue_();
	//void yoshi_();
	void setMainTimerValue_();
	void resetMainTimerValue_();
	void blink_();

private:
	void UpdateIppon_( Ipponboard::EFighter ) const;
	void UpdateWazaari_( Ipponboard::EFighter ) const;
	void UpdateYuko_( Ipponboard::EFighter ) const;
	void UpdateShido_( Ipponboard::EFighter ) const;
	void UpdateHansokumake_( Ipponboard::EFighter ) const;
	void UpdateTeamScore_() const;
	void UpdateHoldClock_(const QColor& text,
						  const QColor& bg,
						  const QString& value) const;
	Ipponboard::EFighter GVF_(const Ipponboard::EFighter f) const; // GetViewFighter
	bool IsSecondary_() const;
	const QColor& GetColor_(const ColorType t) const;
	void View::UpdateColors_();

	EType m_Type;
	IController* m_pController;
#ifdef HORIZONTAL_VIEW
	Ui::ScoreViewHorizontal* ui;
#else
# ifdef TEAM_VIEW
	Ui::ScoreViewVerticalTeam* ui;
# else
	Ui::ScoreViewVerticalSingle* ui;
# endif
#endif
	QFont m_TextFont;
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
	QTimer* m_pBlinkTimer;
};

} // namespace Ipponboard
#endif // VIEW_H
