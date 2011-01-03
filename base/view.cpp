#define HORIZONTAL_VIEW

#include "view.h"
#ifdef HORIZONTAL_VIEW
# include "ui_view_horizontal.h"
#else
# ifdef TEAM_VIEW
#  include "ui_view_vertical_team.h"
# else
#  include "ui_view_vertical_single.h"
# endif
#endif
#include <QPainter>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QFontDatabase>
#include <QMenu>
#include <QInputDialog>

using namespace Ipponboard;

//=========================================================
View::View( IController* pController, EType type, QWidget *parent )
	: QWidget(parent)
	, m_Type(type)
	, m_pController(pController)
#ifdef HORIZONTAL_VIEW
	, ui(new Ui::ScoreViewHorizontal)
#else
# ifdef TEAM_VIEW
	, ui(new Ui::ScoreViewVerticalTeam)
# else
	, ui(new Ui::ScoreViewVerticalSingle)
# endif
#endif
	, m_TextFont("Calibri")
	, m_DigitFont("Calibri")
	, m_TextColorBlue(Qt::white)
	, m_TextBgColorBlue(Qt::blue)
	, m_TextColorWhite(Qt::black)
	, m_TextBgColorWhite(Qt::white)
	, m_InfoTextColor(Qt::darkBlue)
	, m_InfoTextBgColor(Qt::lightGray)
	, m_MainClockColorRunning(Qt::yellow)
	, m_MainClockColorStopped(Qt::red)
	, m_mat("")
	, m_weight("")
	, m_category("")
	, m_drawIppon(false)
	, m_showInfoHeader(true)
	, m_pBlinkTimer(0)
//=========================================================
{
	// init widgets
	ui->setupUi(this);
	ui->image_shido1_white->UpdateImage(":res/images/off.png");
	ui->image_shido2_white->UpdateImage(":res/images/off.png");
	ui->image_shido3_white->UpdateImage(":res/images/off.png");
	ui->image_shido1_blue->UpdateImage(":res/images/off.png");
	ui->image_shido2_blue->UpdateImage(":res/images/off.png");
	ui->image_shido3_blue->UpdateImage(":res/images/off.png");
	ui->image_hansokumake_blue->UpdateImage(":res/images/off.png");
	ui->image_hansokumake_white->UpdateImage(":res/images/off.png");
	ui->image_sand_clock->UpdateImage(":res/images/sand_clock.png");
	ui->dummy_blue->UpdateImage(":res/images/off_empty.png");
	ui->dummy_white->UpdateImage(":res/images/off_empty.png");

	QColor bgColor1 = GetColor_(blueBg);
	QColor fgColor1 = GetColor_(blueFg);
	QColor bgColor2 = GetColor_(whiteBg);
	QColor fgColor2 = GetColor_(whiteFg);

	// set point descriptions
	const QFont descFont("Calibri", 12, QFont::Normal, false);

	ui->text_ippon_desc1->setFont(descFont);
	ui->text_wazaari_desc1->setFont(descFont);
	ui->text_yuko_desc1->setFont(descFont);
	ui->text_ippon_desc1->SetColor(fgColor1);
	ui->text_wazaari_desc1->SetColor(fgColor1);
	ui->text_yuko_desc1->SetColor(fgColor1);
	ui->text_shido_desc->SetColor(fgColor1);
	ui->text_ippon_desc1->SetText("Ippon");
	ui->text_wazaari_desc1->SetText("Waza-ari");
	ui->text_yuko_desc1->SetText("Yuko");
	ui->text_shido_desc->SetText("");
#ifdef HORIZONTAL_VIEW
	ui->text_ippon_desc2->setFont(descFont);
	ui->text_wazaari_desc2->setFont(descFont);
	ui->text_yuko_desc2->setFont(descFont);
	ui->text_ippon_desc2->SetColor(fgColor1, bgColor1);
	ui->text_wazaari_desc2->SetColor(fgColor1, bgColor1);
	ui->text_yuko_desc2->SetColor(fgColor1, bgColor1);
	ui->text_ippon_desc2->SetText("Ippon");
	ui->text_wazaari_desc2->SetText("Waza-ari");
	ui->text_yuko_desc2->SetText("Yuko");
	ui->text_ippon_desc1->SetColor(fgColor2, bgColor2);
	ui->text_wazaari_desc1->SetColor(fgColor2, bgColor2);
	ui->text_yuko_desc1->SetColor(fgColor2, bgColor2);
	ui->dummy_blue->SetBgColor(bgColor1);
	ui->dummy_white->SetBgColor(bgColor2);
#endif

	ui->text_ippon_blue->SetDigitSize(true);
	ui->text_ippon_white->SetDigitSize(true);
	ui->text_wazaari_blue->SetDigitSize(true);
	ui->text_wazaari_white->SetDigitSize(true);
	ui->text_yuko_blue->SetDigitSize(true);
	ui->text_yuko_white->SetDigitSize(true);
	ui->text_main_clock->SetDigitSize(true);
	ui->text_hold_clock->SetDigitSize(true);
#ifdef TEAM_VIEW
	ui->text_score_team_blue->SetDigitSize(true);
	ui->text_score_team_white->SetDigitSize(true);
#endif

	ui->text_ippon_blue->SetColor(fgColor1, bgColor1);
	ui->text_wazaari_blue->SetColor(fgColor1, bgColor1);
	ui->text_yuko_blue->SetColor(fgColor1, bgColor1);
	ui->text_ippon_white->SetColor(fgColor2, bgColor2);
	ui->text_wazaari_white->SetColor(fgColor2, bgColor2);
	ui->text_yuko_white->SetColor(fgColor2, bgColor2);

	ui->text_lastname_blue->SetColor(fgColor1, bgColor1);
	ui->text_firstname_blue->SetColor(fgColor1, bgColor1);
	ui->text_lastname_white->SetColor(fgColor2, bgColor2);
	ui->text_firstname_white->SetColor(fgColor2, bgColor2);
	if( IsSecondary_() )
	{
		ui->layout_point_names->setStretchFactor(ui->text_ippon_desc1, 0);
		ui->layout_point_names->setStretchFactor(ui->text_ippon_desc2, 0);
		ui->text_ippon_desc1->SetText("");
		ui->text_ippon_desc2->SetText("");
	}
	else
	{
		ui->layout_point_names->setStretchFactor(ui->text_ippon_desc1, 1);
		ui->layout_point_names->setStretchFactor(ui->text_ippon_desc2, 1);
	}

//#ifndef HORIZONTAL_VIEW
	ui->image_shido1_blue->SetBgColor(bgColor1);
	ui->image_shido2_blue->SetBgColor(bgColor1);
	ui->image_shido3_blue->SetBgColor(bgColor1);
	ui->image_hansokumake_blue->SetBgColor(bgColor1);
	ui->image_shido1_white->SetBgColor(bgColor2);
	ui->image_shido2_white->SetBgColor(bgColor2);
	ui->image_shido3_white->SetBgColor(bgColor2);
	ui->image_hansokumake_white->SetBgColor(bgColor2);
//#endif

//	QFontDatabase fontDb;
//	QFont newFont = fontDb.font("Bonzai", "Normal", 12 );

	SetTextFont( QFont("Calibri", 12, QFont::Bold, false ) );
	SetDigitFont( QFont("Arial", 12, QFont::Bold, false ) );
	SetMainClockColor( Qt::yellow, Qt::red );

	// register view to receive updates
	m_pController->RegisterView(this);

	ui->text_mat->SetColor( m_InfoTextColor, m_InfoTextBgColor );
	ui->text_weight->SetColor( m_InfoTextColor, m_InfoTextBgColor );
	ui->text_mat->SetDigitSize(true);
	ui->text_weight->SetDigitSize(true);

	m_pBlinkTimer = new QTimer(this);
	connect( m_pBlinkTimer, SIGNAL(timeout()), this, SLOT(blink_()) );
}

//=========================================================
View::~View()
//=========================================================
{
	m_pController = 0;
	delete ui;
}

//=========================================================
void View::UpdateView()
//=========================================================
{
	Q_ASSERT(m_pController && "Controller not set!");

	UpdateColors_();

	//
	// weight class
	//
#ifdef TEAM_VIEW
	ui->text_mat->SetText( tr("Fight ") +
		QString::number(m_pController->GetRound()) );
	ui->text_weight->SetText(
			//m_pController->GetWeightClass() + " " +
			m_pController->GetWeight() + " kg");
#else
	ui->text_mat->SetText( m_mat.toUpper() );
	QString infoText(m_category);
	if (!infoText.isEmpty())
		infoText += " / ";
	infoText += m_weight.toUpper();
	ui->text_weight->SetText( infoText );
#endif

	if( m_showInfoHeader )
	{
		ui->verticalLayout_main->setStretchFactor(ui->layout_info_top, 4);
		ui->text_weight->setVisible(true);
		ui->text_mat->setVisible(true);
	}
	else
	{
		ui->verticalLayout_main->setStretchFactor(ui->layout_info_top, 0);
		ui->text_weight->setVisible(false);
		ui->text_mat->setVisible(false);
	}

	//
	// fighter names
	//
	ui->text_lastname_blue->SetText(
			m_pController->GetFighterLastName(GVF_(eFighter_Blue)).toUpper() );
	ui->text_lastname_white->SetText(
			m_pController->GetFighterLastName(GVF_(eFighter_White)).toUpper() );
	ui->text_firstname_blue->SetText(
			m_pController->GetFighterFirstName(GVF_(eFighter_Blue)).toUpper() );
	ui->text_firstname_white->SetText(
			m_pController->GetFighterFirstName(GVF_(eFighter_White)).toUpper() );

	// blue score
	UpdateIppon_(eFighter_Blue);
	UpdateWazaari_(eFighter_Blue);
	UpdateYuko_(eFighter_Blue);
	UpdateShido_(eFighter_Blue);
	UpdateHansokumake_(eFighter_Blue);

	// white score
	UpdateIppon_(eFighter_White);
	UpdateWazaari_(eFighter_White);
	UpdateYuko_(eFighter_White);
	UpdateShido_(eFighter_White);
	UpdateHansokumake_(eFighter_White);

	UpdateTeamScore_();

	//
	// timers
	//
	ui->text_main_clock->SetText( m_pController->GetTimeText( eTimer_Main ) );
	const EFighter holder( m_pController->GetLastHolder() );
	switch(m_pController->GetCurrentState())
	{
	case eState_TimerRunning:
		{
			ui->text_main_clock->SetColor( m_MainClockColorRunning );
			if ( eFighter_Blue == holder )
			{
				UpdateHoldClock_( Qt::lightGray, m_TextBgColorBlue,
								m_pController->GetTimeText( eTimer_Hold ) );
			}
			else if( eFighter_White == holder )
			{
				UpdateHoldClock_( Qt::darkGray, m_TextBgColorWhite,
								m_pController->GetTimeText( eTimer_Hold ) );
			}
			else
			{
				UpdateHoldClock_( Qt::lightGray, Qt::black,
								m_pController->GetTimeText( eTimer_Hold ) );
			}
		}
		break;

	case eState_TimerStopped:
		{
			ui->text_main_clock->SetColor( m_MainClockColorStopped );
			if ( eFighter_Blue == holder )
			{
				UpdateHoldClock_( Qt::lightGray, m_TextBgColorBlue,
								m_pController->GetTimeText( eTimer_Hold ) );
			}
			else if( eFighter_White == holder )
			{
				UpdateHoldClock_( Qt::darkGray, m_TextBgColorWhite,
								m_pController->GetTimeText( eTimer_Hold ) );
			}
			else
			{
				UpdateHoldClock_( Qt::lightGray, Qt::black,
								m_pController->GetTimeText( eTimer_Hold ) );
			}
		}
		break;

	case eState_Holding:
		{
			ui->text_main_clock->SetColor( m_MainClockColorRunning );
			if ( eFighter_Blue == holder )
			{
				UpdateHoldClock_( m_TextColorBlue, m_TextBgColorBlue,
								  m_pController->GetTimeText( eTimer_Hold ) );
			}
			else if ( eFighter_White == holder )
			{
				UpdateHoldClock_( m_TextColorWhite, m_TextBgColorWhite,
								  m_pController->GetTimeText( eTimer_Hold ) );
			}
			else
			{
				UpdateHoldClock_( Qt::gray, Qt::black,
								  m_pController->GetTimeText( eTimer_Hold ) );
			}
			#ifdef HORIZONTAL_VIEW
				ui->layout_info->setStretchFactor(ui->layout_name_blue, 3);
				ui->layout_info->setStretchFactor(ui->layout_osaekomi, 2);
				ui->layout_info->setStretchFactor(ui->layout_name_white, 3);
			#else
				ui->layout_info->setStretchFactor(ui->layout_name_blue, 2);
				ui->layout_info->setStretchFactor(ui->layout_osaekomi, 2);
				ui->layout_info->setStretchFactor(ui->layout_name_white, 2);
			#endif
		}
		break;

	default:
		break;
	}

	// time is up?
	//const EFighter lead( m_pController->GetLead() );
	//if ( eState_TimerEnded == m_pController->GetCurrentState() )
	//{
	//	// something to do here?
	//}

#ifdef _DEBUG
	QString text;
	switch(m_pController->GetCurrentState())
	{
		case eState_Holding:
			text = "HOLDING";
			break;
		case eState_TimerRunning:
			text = "RUNNING";
			break;
		case eState_TimerStopped:
			text = "STOPPED";
			break;
		default:
			text="OTHER";
	}
	ui->text_lastname_blue->SetText(text);
	update();
#endif

	// Note: with update() the area is scheduled for a redraw
	//       while repaint() does this immediately.
	repaint();
		// This is really important, or timer values are not redrawn!
}

//=========================================================
void View::SetController( IController* pController )
//=========================================================
{
	m_pController = pController;
}

//=========================================================
void View::SetTextFont( const QFont& font )
//=========================================================
{
	m_TextFont = font;

#ifdef HORIZONTAL_VIEW
	ui->text_lastname_white->setAlignment(Qt::AlignLeft);
	ui->text_firstname_white->setAlignment(Qt::AlignLeft);
	ui->text_lastname_blue->setAlignment(Qt::AlignRight);
	ui->text_firstname_blue->setAlignment(Qt::AlignRight);
#else
	//ui->text_lastname_white->setAlignment(Qt::AlignRight);
	//ui->text_firstname_white->setAlignment(Qt::AlignRight);
	ui->text_hold_clock->setAlignment(Qt::AlignLeft);
	//ui->text_weight->setAlignment(Qt::AlignLeft);
	//ui->text_weight_kg->setAlignment(Qt::AlignLeft);
#endif

	ui->text_lastname_white->SetFont(font);
	ui->text_firstname_white->SetFont(font);
	ui->text_lastname_blue->SetFont(font);
	ui->text_firstname_blue->SetFont(font);

	ui->text_weight->SetFont(font);
	ui->text_mat->SetFont(font);

}
//=========================================================
void View::SetDigitFont( const QFont& font )
//=========================================================
{
	m_DigitFont = font;

	ui->text_main_clock->SetFont(font);
	ui->text_ippon_blue->SetFont(font);
	ui->text_wazaari_blue->SetFont(font);
	ui->text_yuko_blue->SetFont(font);
	ui->text_ippon_white->SetFont(font);
	ui->text_wazaari_white->SetFont(font);
	ui->text_yuko_white->SetFont(font);
	ui->text_hold_clock->SetFont(font);
#ifdef TEAM_VIEW
	ui->text_score_team_blue->SetFont(font);
	ui->text_score_team_white->SetFont(font);
#endif
}

//=========================================================
void View::SetInfoTextColor( const QColor& color, const QColor& bgColor )
//=========================================================
{
	m_InfoTextColor = color;
	m_InfoTextBgColor = bgColor;

	ui->text_mat->SetColor(color, bgColor);
	ui->text_weight->SetColor(color, bgColor);
}


//=========================================================
void View::SetTextColorBlue( const QColor& color, const QColor& bgColor )
//=========================================================
{
	m_TextColorBlue = color;
	m_TextBgColorBlue = bgColor;

	UpdateColors_();
}

//=========================================================
void View::SetTextColorWhite( const QColor& color, const QColor& bgColor )
//=========================================================
{
	m_TextColorWhite = color;
	m_TextBgColorWhite = bgColor;

	UpdateColors_();
}


//=========================================================
void View::SetMainClockColor( const QColor& running, const QColor& stopped )
//=========================================================
{
	m_MainClockColorRunning = running;
	m_MainClockColorStopped = stopped;

	// colors are set automatically depending on current state
}

//=========================================================
void View::Reset()
//=========================================================
{
	ui->text_hold_clock->SetColor(Qt::gray, Qt::black);
	ui->image_sand_clock->SetBgColor(Qt::black);
}

//=========================================================
void View::SetShowInfoHeader(bool show)
//=========================================================
{
	m_showInfoHeader = show;

	UpdateView();
}

//=========================================================
void View::paintEvent(QPaintEvent * /*event*/)
//=========================================================
{
	Q_ASSERT(m_pController && "IBController not set!");
}

//=========================================================
void View::mousePressEvent(QMouseEvent* event)
//=========================================================
{
	Q_ASSERT(m_pController && "IBController not set!");

	EFighter whos(eFighter_Nobody);
	EAction action(eAction_NONE);
	const bool doRevoke = event->button() & Qt::RightButton;

	ScaledImage* child = dynamic_cast<ScaledImage*>(childAt(event->pos()));
	if (!child)
	{
		ScaledText* child = dynamic_cast<ScaledText*>(childAt(event->pos()));
		if (!child)
			return;
		if( child == ui->text_main_clock )
		{
			if( doRevoke )  // right click!
			{
				QMenu menu;
				QAction* item(0);

				item = menu.addAction(tr("Set Value"));
				if( m_pController->GetCurrentState() != eState_SonoMama &&
					m_pController->GetCurrentState() != eState_TimerStopped )
					item->setEnabled(false);
				else
					connect( item, SIGNAL(triggered()), this, SLOT(setMainTimerValue_()) );

				item = menu.addAction(tr("Reset"));
				connect( item, SIGNAL(triggered()), this, SLOT(resetMainTimerValue_()) );

				menu.exec(QCursor::pos());

				return;
			}
			action = eAction_Hajime_Matte;
		}
		else if( child == ui->text_hold_clock )
		{
			if( doRevoke )  // right click!
			{
				QMenu menu;
				QAction* item(0);
				item = menu.addAction(QIcon(":/res/images/choose_blue_16.png"), tr("Blue is holding"));
				connect( item, SIGNAL(triggered()), this, SLOT(setOsaekomiBlue_()) );
				item = menu.addAction(QIcon(":/res/images/choose_white_16.png"),tr("White is holding"));
				connect( item, SIGNAL(triggered()), this, SLOT(setOsaekomiWhite_()) );
				menu.addSeparator();
				//item = menu.addAction( tr("Continue holding") );
				//connect( item, SIGNAL(triggered()), this, SLOT(yoshi_()) );

				item = menu.addAction(tr("Set Value"));
//				if( m_pController->GetCurrentState() != eState_SonoMama &&
//					m_pController->GetCurrentState() != eState_TimerStopped )
//					item->setEnabled(false);
//				else
					connect( item, SIGNAL(triggered()), this, SLOT(setOsaekomiTimerValue_()) );

				item = menu.addAction(tr("Reset"));
				connect( item, SIGNAL(triggered()), this, SLOT(resetOsaekomiTimerValue_()) );

				menu.exec(QCursor::pos());

				return;
			}
			//if( eState_TimerRunning_Osaekomi == m_pController->GetCurrentState() )
			whos = eFighter_Blue; // initially set osaekomi for blue
			action = eAction_OsaeKomi_Toketa;
		}
		else if ( child == ui->text_ippon_white )
		{
			whos = eFighter_White;
			action = eAction_Ippon;
		}
		else if ( child == ui->text_ippon_blue )
		{
			whos = eFighter_Blue;
			action = eAction_Ippon;
		}
		else if( child == ui->text_wazaari_white )
		{
			whos = eFighter_White;
			action = eAction_Wazaari;
		}
		else if( child == ui->text_wazaari_blue )
		{
			whos = eFighter_Blue;
			action = eAction_Wazaari;
		}
		else if( child == ui->text_yuko_white )
		{
			whos = eFighter_White;
			action = eAction_Yuko;
		}
		else if( child == ui->text_yuko_blue )
		{
			whos = eFighter_Blue;
			action = eAction_Yuko;
		}
		else
		{
			Q_ASSERT(!"action not defined for pointer!");
		}
	}
	else
	{
		if( child == ui->image_shido1_white )
		{
			whos = eFighter_White;
			action = eAction_Shido;
		}
		else if( child == ui->image_shido3_white ||
				 child == ui->image_shido2_white ||
				 child == ui->image_shido1_white )
		{
			whos = eFighter_White;
			action = eAction_Shido;
		}
		else if( child == ui->image_shido3_blue ||
				 child == ui->image_shido2_blue ||
				 child == ui->image_shido1_blue )
		{
			whos = eFighter_Blue;
			action = eAction_Shido;
		}
		else if( child == ui->image_hansokumake_white )
		{
			whos = eFighter_White;
			action = eAction_Hansokumake;
		}
		else if( child == ui->image_hansokumake_blue )
		{
			whos = eFighter_Blue;
			action = eAction_Hansokumake;
		}
		else
		{
			Q_ASSERT(!"action not defined for pointer!");
		}
	}

	whos = GVF_(whos); // get correct fighter for display
	m_pController->DoAction( action, whos, doRevoke );
}

//=========================================================
void View::setOsaekomiBlue_()
//=========================================================
{
	m_pController->DoAction( eAction_SetOsaekomi, eFighter_Blue, false/*doRevoke*/ );
}

//=========================================================
void View::setOsaekomiWhite_()
//=========================================================
{
	m_pController->DoAction( eAction_SetOsaekomi, eFighter_White, false/*doRevoke*/ );
}

//=========================================================
void View::setOsaekomiTimerValue_()
//=========================================================
{
	bool ok(false);
	const int seconds = QInputDialog::getInt(
		0,
		tr("Set Value"),
		tr("Set value to (ss):"),
		0,	// value
		0,	// min
		59,	// max
		1,	// step
		&ok);
	if( ok )
		m_pController->SetTimerValue(eTimer_Hold, QString::number(seconds));
}

//=========================================================
void View::resetOsaekomiTimerValue_()
//=========================================================
{
	m_pController->ResetTimerValue(eTimer_Hold);
}

//=========================================================
void View::setMainTimerValue_()
//=========================================================
{
	bool ok(false);
	const QString time = QInputDialog::getText(
		0,
		tr("Set Value"),
		tr("Set value to (m:ss):"),
		QLineEdit::Normal,
		m_pController->GetTimeText(eTimer_Main),
		&ok);
	if( ok )
		m_pController->SetTimerValue(eTimer_Main, time);
}

//=========================================================
void View::resetMainTimerValue_()
//=========================================================
{
	m_pController->ResetTimerValue(eTimer_Main);
}

//=========================================================
void View::blink_()
//=========================================================
{
	m_drawIppon = !m_drawIppon;
	if( IsSecondary_() )
	{
		UpdateIppon_(eFighter_Blue);
		UpdateIppon_(eFighter_White);
	}
}

//=========================================================
void View::UpdateIppon_( Ipponboard::EFighter who ) const
//=========================================================
{
	ScaledText* digit_ippon(ui->text_ippon_blue);
#ifdef HORIZONTAL_VIEW
	QVBoxLayout* digit_wazaari(ui->layout_wazaari_blue);
	QVBoxLayout* digit_yuko(ui->layout_yuko_blue);
	EFighter uke(eFighter_White);
#endif
	if( eFighter_White == who )
	{
		digit_ippon = ui->text_ippon_white;
#ifdef HORIZONTAL_VIEW
		digit_wazaari = ui->layout_wazaari_white;
		digit_yuko = ui->layout_yuko_white;
		uke = eFighter_Blue;
#endif
	}

	const int score = m_pController->GetScore( GVF_(who), ePoint_Ippon );
	if( score != 0 )
	{
#ifdef HORIZONTAL_VIEW
		Q_ASSERT(m_pBlinkTimer);
		if( !m_pBlinkTimer->isActive() )
			m_pBlinkTimer->start(750);

		if( IsSecondary_() )
		{
			if( m_drawIppon )
			{
				ui->layout_score->setStretchFactor(digit_ippon,6);
				ui->layout_score->setStretchFactor(digit_wazaari,0);
				ui->layout_score->setStretchFactor(digit_yuko,0);
			}
			else
			{
				ui->layout_score->setStretchFactor(digit_ippon,0);
				ui->layout_score->setStretchFactor(digit_wazaari,3);
				ui->layout_score->setStretchFactor(digit_yuko,3);
			}
		}
		else
		{
			ui->layout_score->setStretchFactor(digit_ippon,2);
			ui->layout_score->setStretchFactor(digit_wazaari,2);
			ui->layout_score->setStretchFactor(digit_yuko,2);
		}
#else
		ui->verticalLayout_main->setStretchFactor(digit_ippon, 8);
		digit_ippon->SetBlinking(true);

#endif
		digit_ippon->SetText( "IPPON", !IsSecondary_() );
	}
	else
	{
		const int score_uke = m_pController->GetScore(GVF_(uke), ePoint_Ippon);
		if( m_pBlinkTimer->isActive() &&  0 == score_uke )
			m_pBlinkTimer->stop();

		digit_ippon->SetBlinking(false);
		if( !IsSecondary_() )
		{
#ifdef HORIZONTAL_VIEW
			ui->layout_score->setStretchFactor(digit_ippon, 2);
			ui->layout_score->setStretchFactor(digit_wazaari,2);
			ui->layout_score->setStretchFactor(digit_yuko,2);
#else
			ui->verticalLayout_main->setStretchFactor(digit_ippon, 3);
#endif
			digit_ippon->SetText( "-" );
		}
		else
		{
#ifdef HORIZONTAL_VIEW
			ui->layout_score->setStretchFactor(digit_ippon, 0);
			ui->layout_score->setStretchFactor(digit_wazaari,3);
			ui->layout_score->setStretchFactor(digit_yuko,3);
#else
			ui->verticalLayout_main->setStretchFactor(digit_ippon, 0);
#endif
			digit_ippon->SetText( "" );
		}
	}
}

//=========================================================
void View::UpdateWazaari_( Ipponboard::EFighter who ) const
//=========================================================
{
	ScaledText* digit(ui->text_wazaari_blue);
	if( eFighter_White == who )
		digit = ui->text_wazaari_white;

	const int score = m_pController->GetScore( GVF_(who), ePoint_Wazaari );
	//digit->setDigitCount( score > 9 ? 2 : 1 );
	digit->SetText( QString::number(score) );
}

//=========================================================
void View::UpdateYuko_( Ipponboard::EFighter who ) const
//=========================================================
{
	ScaledText* digit(ui->text_yuko_blue);
	if( eFighter_White == who )
		digit = ui->text_yuko_white;

	const int score = m_pController->GetScore( GVF_(who), ePoint_Yuko );
	digit->SetText( QString::number(score) );
}

//=========================================================
void View::UpdateShido_( Ipponboard::EFighter who ) const
//=========================================================
{
	ScaledImage* pImage1(0);
	ScaledImage* pImage2(0);
	ScaledImage* pImage3(0);

	if( eFighter_Blue == who )
	{
		pImage1 = ui->image_shido1_blue;
		pImage2 = ui->image_shido2_blue;
		pImage3 = ui->image_shido3_blue;
	}
	else
	{
		pImage1 = ui->image_shido1_white;
		pImage2 = ui->image_shido2_white;
		pImage3 = ui->image_shido3_white;
	}

	const int score = m_pController->GetScore( GVF_(who), ePoint_Shido );
	if( score >= 3 )
	{
		pImage3->UpdateImage(":res/images/on.png");
	}
	else
	{
		if( eTypePrimary == m_Type )
			pImage3->UpdateImage(":res/images/off.png");
		else
			pImage3->UpdateImage(":res/images/off_empty.png");
	}
	if( score >= 2 )
	{
		pImage2->UpdateImage(":res/images/on.png");
	}
	else
	{
		if( eTypePrimary == m_Type )
			pImage2->UpdateImage(":res/images/off.png");
		else
			pImage2->UpdateImage(":res/images/off_empty.png");
	}
	if( score >= 1 )
	{
		pImage1->UpdateImage(":res/images/on.png");
	}
	else
	{
		if( eTypePrimary == m_Type )
			pImage1->UpdateImage(":res/images/off.png");
		else
			pImage1->UpdateImage(":res/images/off_empty.png");
	}
}

//=========================================================
void View::UpdateHansokumake_( Ipponboard::EFighter who ) const
//=========================================================
{
	ScaledImage* pImage(ui->image_hansokumake_blue);
//	QHBoxLayout* pLayout(ui->horizontalLayout_score_blue);
//	QVBoxLayout* pShidoLayout(ui->verticalLayout_shido_blue);
	if( eFighter_White == who )
	{
		pImage = ui->image_hansokumake_white;
//		pLayout = ui->horizontalLayout_score_white;
//		pShidoLayout = ui->verticalLayout_shido_white;
	}

	const int score_hansokumake = m_pController->GetScore( GVF_(who), ePoint_Hansokumake );
	const int score_shido = m_pController->GetScore( GVF_(who), ePoint_Shido );
	if( score_hansokumake > 0 || score_shido == 4 )
	{
		pImage->UpdateImage(":res/images/on_hansokumake.png");
		if( IsSecondary_() )
		{
//			pLayout->setStretchFactor(pImage, 3);
//			pLayout->setStretchFactor(pShidoLayout, 3);
		}
	}
	else
	{
		if( eTypePrimary == m_Type )
		{
			pImage->UpdateImage(":res/images/off_hansokumake.png");
		}
		else
		{
			pImage->UpdateImage(":res/images/off_empty.png");
//			pLayout->setStretchFactor(pImage, 0);
//			pLayout->setStretchFactor(pShidoLayout, 6);
		}
	}
}

//=========================================================
void View::UpdateTeamScore_() const
//=========================================================
{
	EFighter tori = eFighter_Blue;
	EFighter uke = eFighter_White;

	if( m_Type == eTypePrimary )
	{
		uke = eFighter_Blue;
		tori = eFighter_White;
	}

#ifdef TEAM_VIEW
	ui->text_score_team_blue->SetText(
			QString::number(m_pController->GetTeamScore(tori))
	);
	ui->text_score_team_white->SetText(
			QString::number(m_pController->GetTeamScore(uke))
	);
#else
	ui->text_score_team_blue->SetText("");
	ui->text_score_team_white->SetText("");
#endif
}

//=========================================================
void View::UpdateHoldClock_( const QColor& text,
							 const QColor& bg,
							 const QString& value ) const
//=========================================================
{
	ui->text_hold_clock->SetColor( text, bg );
	ui->image_sand_clock->SetBgColor( bg );

	if( value == "00" &&
		eState_Holding != m_pController->GetCurrentState() )
	{
		// hide score on secondary view
		ui->text_hold_clock->SetColor( Qt::gray, Qt::black );
		ui->image_sand_clock->SetBgColor( Qt::black );
		if( IsSecondary_() )
		{
			ui->text_hold_clock->SetText( "" );
			ui->image_sand_clock->UpdateImage(":res/images/off_empty.png");
		#ifdef HORIZONTAL_VIEW
			ui->layout_info->setStretchFactor(ui->layout_osaekomi, 0);
		#else
			ui->layout_info->setStretchFactor(ui->layout_name_blue, 4);
			ui->layout_info->setStretchFactor(ui->layout_osaekomi, 1);
			ui->layout_info->setStretchFactor(ui->layout_name_white, 4);
		#endif
		}
		else
		{
			ui->text_hold_clock->SetText( value );
		}
	}
	else
	{
		ui->text_hold_clock->SetText( value );
		ui->image_sand_clock->UpdateImage(":res/images/sand_clock.png");
	#ifdef HORIZONTAL_VIEW
		//ui->layout_info->setStretchFactor(ui->layout_name_blue, 4);
		//ui->layout_info->setStretchFactor(ui->layout_name_white, 4);
	#else
		//ui->layout_info->setStretchFactor(ui->layout_name_blue, 4);
		//ui->layout_info->setStretchFactor(ui->layout_osaekomi, 1);
		//ui->layout_info->setStretchFactor(ui->layout_name_white, 4);
	#endif
	}
}

//=========================================================
Ipponboard::EFighter View::GVF_(const Ipponboard::EFighter f) const
//=========================================================
{
#ifdef HORIZONTAL_VIEW
	if( !IsSecondary_() )
	{
		return (f==eFighter_Blue)? eFighter_White : eFighter_Blue;
	}
#endif
	return f;
}

//=========================================================
bool View::IsSecondary_() const
//=========================================================
{
	return eTypeSecondary == m_Type;
}

//=========================================================
const QColor& View::GetColor_(const ColorType t) const
//=========================================================
{
#ifdef HORIZONTAL_VIEW
	const bool doSwap(eTypePrimary == m_Type);
#else
	const bool doSwap(false);
#endif

	switch(t)
	{
	case blueFg:
		return doSwap ? m_TextColorWhite : m_TextColorBlue;
	case blueBg:
		return doSwap ? m_TextBgColorWhite : m_TextBgColorBlue;
	case whiteFg:
		return doSwap ? m_TextColorBlue : m_TextColorWhite;
	case whiteBg:
		return doSwap ? m_TextBgColorBlue : m_TextBgColorWhite;
	default:
		assert(!"unhandled switch case!");
	}

	return m_TextBgColorBlue; // just for debug purpose...
}

//=========================================================
void View::UpdateColors_()
//=========================================================
{
	ui->text_score_team_blue->SetColor(GetColor_(blueFg), GetColor_(blueBg));
	ui->dummy_blue->SetBgColor(GetColor_(blueBg));
	ui->image_hansokumake_blue->SetBgColor(GetColor_(blueBg));
	ui->image_shido3_blue->SetBgColor(GetColor_(blueBg));
	ui->image_shido2_blue->SetBgColor(GetColor_(blueBg));
	ui->image_shido1_blue->SetBgColor(GetColor_(blueBg));
	ui->text_yuko_blue->SetColor(GetColor_(blueFg), GetColor_(blueBg));
	ui->text_wazaari_blue->SetColor(GetColor_(blueFg), GetColor_(blueBg));
	ui->text_ippon_blue->SetColor(GetColor_(blueFg), GetColor_(blueBg));
	ui->text_ippon_desc2->SetColor(GetColor_(blueFg), GetColor_(blueBg));
	ui->text_wazaari_desc2->SetColor(GetColor_(blueFg), GetColor_(blueBg));
	ui->text_yuko_desc2->SetColor(GetColor_(blueFg), GetColor_(blueBg));
	ui->text_lastname_blue->SetColor(GetColor_(blueFg), GetColor_(blueBg));
	ui->text_firstname_blue->SetColor(GetColor_(blueFg), GetColor_(blueBg));

	ui->text_score_team_white->SetColor(GetColor_(whiteFg), GetColor_(whiteBg));
	ui->dummy_white->SetBgColor(GetColor_(whiteBg));
	ui->image_hansokumake_white->SetBgColor(GetColor_(whiteBg));
	ui->image_shido3_white->SetBgColor(GetColor_(whiteBg));
	ui->image_shido2_white->SetBgColor(GetColor_(whiteBg));
	ui->image_shido1_white->SetBgColor(GetColor_(whiteBg));
	ui->text_yuko_white->SetColor(GetColor_(whiteFg), GetColor_(whiteBg));
	ui->text_wazaari_white->SetColor(GetColor_(whiteFg), GetColor_(whiteBg));
	ui->text_ippon_white->SetColor(GetColor_(whiteFg), GetColor_(whiteBg));
	ui->text_ippon_desc1->SetColor(GetColor_(whiteFg), GetColor_(whiteBg));
	ui->text_wazaari_desc1->SetColor(GetColor_(whiteFg), GetColor_(whiteBg));
	ui->text_yuko_desc1->SetColor(GetColor_(whiteFg), GetColor_(whiteBg));
	ui->text_lastname_white->SetColor(GetColor_(whiteFg), GetColor_(whiteBg));
	ui->text_firstname_white->SetColor(GetColor_(whiteFg), GetColor_(whiteBg));
}
