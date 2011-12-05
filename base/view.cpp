
#include "view.h"
#include "ui_view_horizontal.h"

#include <QPainter>
#include <QMessageBox>
#include <QTimer>
#include <QTime>
#include <QFontDatabase>
#include <QMenu>
#include <QInputDialog>

using namespace Ipponboard;

//=========================================================
View::View(IController* pController, EType type, QWidget* parent)
	: QWidget(parent)
	, m_Type(type)
	, m_pController(pController)
	, ui(new Ui::ScoreViewHorizontal)
	, m_InfoHeaderFont("Calibri", 12, QFont::Bold, false)
	, m_FighterNameFont("Calibri", 12, QFont::Bold, true)
	, m_DigitFont("Calibri", 12, QFont::Bold, false)
	, m_TextColorBlue(Qt::white)
	, m_TextBgColorBlue(Qt::blue)
	, m_TextColorWhite(Qt::black)
	, m_TextBgColorWhite(Qt::white)
	, m_InfoTextColor(QColor(255, 255, 96)) //Qt::darkBlue
	, m_InfoTextBgColor(Qt::black) //Qt::lightGray
	, m_MainClockColorRunning(Qt::yellow)
	, m_MainClockColorStopped(Qt::red)
	, m_mat(QCoreApplication::applicationName() + " v" +
			QCoreApplication::applicationVersion())
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
	ui->image_golden_score->UpdateImage(":res/images/sand_clock.png");

	ui->text_hold_clock_blue->SetText("00", ScaledText::eSize_full);
	ui->text_hold_clock_white->SetText("00", ScaledText::eSize_full);

	QColor bgColor1 = get_color(blueBg);
	QColor fgColor1 = get_color(blueFg);
	QColor bgColor2 = get_color(whiteBg);
	QColor fgColor2 = get_color(whiteFg);

	// set point descriptions
	const QFont descFont("Calibri", 12, QFont::Bold, false);

	ui->text_ippon_desc1->setFont(descFont);
	ui->text_wazaari_desc1->setFont(descFont);
	ui->text_yuko_desc1->setFont(descFont);
	ui->text_ippon_desc1->SetColor(fgColor1);
	ui->text_wazaari_desc1->SetColor(fgColor1);
	ui->text_yuko_desc1->SetColor(fgColor1);
	ui->text_shido_desc->SetColor(fgColor1);
	ui->text_ippon_desc1->SetText("I");
	ui->text_wazaari_desc1->SetText("W");
	ui->text_yuko_desc1->SetText("Y");
	ui->text_shido_desc->SetText("");
	ui->text_ippon_desc2->setFont(descFont);
	ui->text_wazaari_desc2->setFont(descFont);
	ui->text_yuko_desc2->setFont(descFont);
	ui->text_ippon_desc2->SetColor(fgColor1, bgColor1);
	ui->text_wazaari_desc2->SetColor(fgColor1, bgColor1);
	ui->text_yuko_desc2->SetColor(fgColor1, bgColor1);
	ui->text_ippon_desc2->SetText("I");
	ui->text_wazaari_desc2->SetText("W");
	ui->text_yuko_desc2->SetText("Y");
	ui->text_ippon_desc1->SetColor(fgColor2, bgColor2);
	ui->text_wazaari_desc1->SetColor(fgColor2, bgColor2);
	ui->text_yuko_desc1->SetColor(fgColor2, bgColor2);
	ui->dummy_blue->SetBgColor(bgColor1);
	ui->dummy_white->SetBgColor(bgColor2);

#ifdef TEAM_VIEW
	ui->text_score_team_blue_label->SetColor(Qt::gray, Qt::black);
	ui->text_score_team_white_label->SetColor(Qt::gray, Qt::black);
	ui->text_score_team_blue->SetColor(Qt::gray, Qt::black);
	ui->text_score_team_white->SetColor(Qt::gray, Qt::black);
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

	if (is_secondary())
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

	ui->image_shido1_blue->SetBgColor(bgColor1);
	ui->image_shido2_blue->SetBgColor(bgColor1);
	ui->image_shido3_blue->SetBgColor(bgColor1);
	ui->image_hansokumake_blue->SetBgColor(bgColor1);
	ui->image_shido1_white->SetBgColor(bgColor2);
	ui->image_shido2_white->SetBgColor(bgColor2);
	ui->image_shido3_white->SetBgColor(bgColor2);
	ui->image_hansokumake_white->SetBgColor(bgColor2);

//	QFontDatabase fontDb;
//	QFont newFont = fontDb.font("Bonzai", "Normal", 12 );

	SetInfoHeaderFont(QFont("Calibri", 12, QFont::Bold, false));
	SetDigitFont(QFont("Arial", 12, QFont::Bold, false));

	// register view to receive updates
	m_pController->RegisterView(this);

	ui->text_mat->SetColor(m_InfoTextColor, m_InfoTextBgColor);
	ui->text_weight->SetColor(m_InfoTextColor, m_InfoTextBgColor);

	m_pBlinkTimer = new QTimer(this);
	connect(m_pBlinkTimer, SIGNAL(timeout()), this, SLOT(blink_()));
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

	show_golden_score(m_pController->IsGoldenScore());

	update_colors();

	//
	// weight class
	//
	ui->text_mat->SetText(m_mat, ScaledText::eSize_normal);
#ifdef TEAM_VIEW
	QString infoText/*(tr("Fight ").toUpper())*/;
	//infoText += QString::number(m_pController->GetRound()) + ": ";
	infoText += m_pController->GetWeight() + "kg";
	ui->text_weight->SetText(infoText, ScaledText::eSize_normal);
#else
	QString infoText(m_category);

	if (!infoText.isEmpty())
		infoText += " / ";

	infoText += m_weight;//.toUpper();
	ui->text_weight->SetText(infoText + "KG", ScaledText::eSize_full);
#endif

	if (m_showInfoHeader)
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
		m_pController->GetFighterLastName(GVF_(eFighter_Blue)),
		ScaledText::eSize_uppercase);
	ui->text_lastname_white->SetText(
		m_pController->GetFighterLastName(GVF_(eFighter_White)),
		ScaledText::eSize_uppercase);
	ui->text_firstname_blue->SetText(
		m_pController->GetFighterFirstName(GVF_(eFighter_Blue)),
		ScaledText::eSize_uppercase);
	ui->text_firstname_white->SetText(
		m_pController->GetFighterFirstName(GVF_(eFighter_White)),
		ScaledText::eSize_uppercase);

	// blue score
	update_ippon(eFighter_Blue);
	update_wazaari(eFighter_Blue);
	update_yuko(eFighter_Blue);
	update_shido(eFighter_Blue);
	update_hansokumake(eFighter_Blue);

	// white score
	update_ippon(eFighter_White);
	update_wazaari(eFighter_White);
	update_yuko(eFighter_White);
	update_shido(eFighter_White);
	update_hansokumake(eFighter_White);

	update_team_score();

	//
	// timers
	//
	ui->text_main_clock->SetText(
		m_pController->GetTimeText(eTimer_Main),
		ScaledText::eSize_full);

	const EFighter holder(m_pController->GetLastHolder());

	if (eFighter_Nobody == holder && is_secondary())
	{
		ui->layout_info->setStretchFactor(ui->layout_osaekomi, 0);
	}

	switch (m_pController->GetCurrentState())
	{
	case eState_TimerRunning:
		{
			ui->text_main_clock->SetColor(m_MainClockColorRunning);
			update_hold_clock(holder, eHoldState_pause);
		}
		break;

	case eState_TimerStopped:
		{
			ui->text_main_clock->SetColor(m_MainClockColorStopped);
			update_hold_clock(holder, eHoldState_pause);
		}
		break;

	case eState_Holding:
		{
			ui->text_main_clock->SetColor(m_MainClockColorRunning);
			update_hold_clock(holder, eHoldState_on);

			if (is_secondary())
			{
				ui->layout_info->setStretchFactor(ui->layout_name_blue, 3);
				ui->layout_info->setStretchFactor(ui->layout_osaekomi, 2);
				ui->layout_info->setStretchFactor(ui->layout_name_white, 3);
			}
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


//#ifdef _DEBUG
//	QString text;
//	switch(m_pController->GetCurrentState())
//	{
//		case eState_Holding:
//			text = "HOLDING";
//			break;
//		case eState_TimerRunning:
//			text = "RUNNING";
//			break;
//		case eState_TimerStopped:
//			text = "STOPPED";
//			break;
//		default:
//			text="OTHER";
//	}
//	ui->text_lastname_blue->SetText(text);
//	update();
//#endif

	// Note: with update() the area is scheduled for a redraw
	//       while repaint() does this immediately.
	repaint();
	// This is really important, or timer values are not redrawn!
}

//=========================================================
void View::SetController(IController* pController)
//=========================================================
{
	m_pController = pController;
}

//=========================================================
void View::SetInfoHeaderFont(const QFont& font)
//=========================================================
{
	m_InfoHeaderFont = font;

	ui->text_weight->SetFont(font);
	ui->text_mat->SetFont(font);

#ifdef TEAM_VIEW
	ui->text_score_team_blue_label->SetFont(font);
	ui->text_score_team_white_label->SetFont(font);
#endif
}

//=========================================================
void View::SetFighterNameFont(const QFont& font)
//=========================================================
{
	m_FighterNameFont = font;

	ui->text_lastname_white->setAlignment(Qt::AlignLeft);
	ui->text_firstname_white->setAlignment(Qt::AlignLeft);
	ui->text_lastname_blue->setAlignment(Qt::AlignRight);
	ui->text_firstname_blue->setAlignment(Qt::AlignRight);

	ui->text_lastname_white->SetFont(font);
	ui->text_firstname_white->SetFont(font);
	ui->text_lastname_blue->SetFont(font);
	ui->text_firstname_blue->SetFont(font);
}

//=========================================================
void View::SetDigitFont(const QFont& font)
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
	ui->text_hold_clock_blue->SetFont(font);
	ui->text_hold_clock_white->SetFont(font);
#ifdef TEAM_VIEW
	ui->text_score_team_blue->SetFont(font);
	ui->text_score_team_white->SetFont(font);
#endif
}

//=========================================================
void View::SetInfoTextColor(const QColor& color, const QColor& bgColor)
//=========================================================
{
	m_InfoTextColor = color;
	m_InfoTextBgColor = bgColor;

	ui->text_mat->SetColor(color, bgColor);
	ui->text_weight->SetColor(color, bgColor);
}


//=========================================================
void View::SetTextColorBlue(const QColor& color, const QColor& bgColor)
//=========================================================
{
	m_TextColorBlue = color;
	m_TextBgColorBlue = bgColor;

	update_colors();
}

//=========================================================
void View::SetTextColorWhite(const QColor& color, const QColor& bgColor)
//=========================================================
{
	m_TextColorWhite = color;
	m_TextBgColorWhite = bgColor;

	update_colors();
}


//=========================================================
void View::SetMainClockColor(const QColor& running, const QColor& stopped)
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
	ui->text_hold_clock_blue->SetColor(Qt::gray, Qt::black);
	ui->text_hold_clock_white->SetColor(Qt::gray, Qt::black);
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
void View::paintEvent(QPaintEvent* /*event*/)
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

		if (child == ui->text_main_clock)
		{
			if (doRevoke)   // right click!
			{
				QMenu menu;
				QAction* item(0);

				item = menu.addAction(tr("Reset"));
				connect(item, SIGNAL(triggered()), this, SLOT(resetMainTimerValue_()));

				menu.exec(QCursor::pos());

				return;
			}

			action = eAction_Hajime_Mate;
		}
		else if (child == ui->text_hold_clock_blue)
		{
			if (doRevoke)   // right click!
			{
				action = eAction_ResetOsaeKomi;
			}
			else
			{
				whos = eFighter_Blue;

				if (eState_Holding == m_pController->GetCurrentState() &&
						GVF_(eFighter_Blue) != m_pController->GetLead())
				{
					action = eAction_SetOsaekomi;
				}
				else
				{
					action = eAction_OsaeKomi_Toketa;
				}
			}
		}
		else if (child == ui->text_hold_clock_white)
		{
			if (doRevoke)   // right click!
			{
				action = eAction_ResetOsaeKomi;
			}
			else
			{
				whos = eFighter_White;

				if (eState_Holding == m_pController->GetCurrentState() &&
						GVF_(eFighter_White) != m_pController->GetLead())
				{
					action = eAction_SetOsaekomi;
				}
				else
				{
					action = eAction_OsaeKomi_Toketa;
				}
			}
		}
		else if (child == ui->text_ippon_white)
		{
			whos = eFighter_White;
			action = eAction_Ippon;
		}
		else if (child == ui->text_ippon_blue)
		{
			whos = eFighter_Blue;
			action = eAction_Ippon;
		}
		else if (child == ui->text_wazaari_white)
		{
			whos = eFighter_White;
			action = eAction_Wazaari;
		}
		else if (child == ui->text_wazaari_blue)
		{
			whos = eFighter_Blue;
			action = eAction_Wazaari;
		}
		else if (child == ui->text_yuko_white)
		{
			whos = eFighter_White;
			action = eAction_Yuko;
		}
		else if (child == ui->text_yuko_blue)
		{
			whos = eFighter_Blue;
			action = eAction_Yuko;
		}

		//else
		//{
		//	Q_ASSERT(!"action not defined for pointer!");
		//}
	}
	else
	{
		if (child == ui->image_shido1_white)
		{
			whos = eFighter_White;
			action = eAction_Shido;
		}
		else if (child == ui->image_shido3_white ||
				 child == ui->image_shido2_white ||
				 child == ui->image_shido1_white)
		{
			whos = eFighter_White;
			action = eAction_Shido;
		}
		else if (child == ui->image_shido3_blue ||
				 child == ui->image_shido2_blue ||
				 child == ui->image_shido1_blue)
		{
			whos = eFighter_Blue;
			action = eAction_Shido;
		}
		else if (child == ui->image_hansokumake_white)
		{
			whos = eFighter_White;
			action = eAction_Hansokumake;
		}
		else if (child == ui->image_hansokumake_blue)
		{
			whos = eFighter_Blue;
			action = eAction_Hansokumake;
		}

		//else
		//{
		//	Q_ASSERT(!"action not defined for pointer!");
		//}
	}

	whos = GVF_(whos); // get correct fighter for display
	m_pController->DoAction(action, whos, doRevoke);
}

//=========================================================
void View::setOsaekomiBlue_()
//=========================================================
{
	m_pController->DoAction(eAction_SetOsaekomi, eFighter_Blue, false/*doRevoke*/);
}

//=========================================================
void View::setOsaekomiWhite_()
//=========================================================
{
	m_pController->DoAction(eAction_SetOsaekomi, eFighter_White, false/*doRevoke*/);
}

//=========================================================
void View::resetMainTimerValue_()
//=========================================================
{
	m_pController->DoAction(eAction_ResetMainTimer, eFighter_Nobody, true/*doRevoke*/);
}

//=========================================================
void View::blink_()
//=========================================================
{
	m_drawIppon = !m_drawIppon;

	if (is_secondary())
	{
		update_ippon(eFighter_Blue);
		update_ippon(eFighter_White);
	}
}

//=========================================================
void View::update_ippon(Ipponboard::EFighter who) const
//=========================================================
{
	ScaledText* digit_ippon(ui->text_ippon_blue);
	QVBoxLayout* digit_wazaari(ui->layout_wazaari_blue);
	QVBoxLayout* digit_yuko(ui->layout_yuko_blue);
	EFighter uke(eFighter_White);

	if (eFighter_White == who)
	{
		digit_ippon = ui->text_ippon_white;
		digit_wazaari = ui->layout_wazaari_white;
		digit_yuko = ui->layout_yuko_white;
		uke = eFighter_Blue;
	}

	const int score = m_pController->GetScore(GVF_(who), ePoint_Ippon);

	if (score != 0)
	{
		Q_ASSERT(m_pBlinkTimer);

		if (!m_pBlinkTimer->isActive())
			m_pBlinkTimer->start(750);

		if (is_secondary())
		{
			if (m_drawIppon)
			{
				ui->layout_score->setStretchFactor(digit_ippon, 6);
				ui->layout_score->setStretchFactor(digit_wazaari, 0);
				ui->layout_score->setStretchFactor(digit_yuko, 0);
			}
			else
			{
				ui->layout_score->setStretchFactor(digit_ippon, 0);
				ui->layout_score->setStretchFactor(digit_wazaari, 3);
				ui->layout_score->setStretchFactor(digit_yuko, 3);
			}
		}
		else
		{
			ui->layout_score->setStretchFactor(digit_ippon, 2);
			ui->layout_score->setStretchFactor(digit_wazaari, 2);
			ui->layout_score->setStretchFactor(digit_yuko, 2);
		}

		digit_ippon->SetText("IPPON", ScaledText::eSize_full,
							 !is_secondary());
	}
	else
	{
		const int score_uke = m_pController->GetScore(GVF_(uke), ePoint_Ippon);

		if (m_pBlinkTimer->isActive() &&  0 == score_uke)
			m_pBlinkTimer->stop();

		digit_ippon->SetBlinking(false);

		if (!is_secondary())
		{
			ui->layout_score->setStretchFactor(digit_ippon, 2);
			ui->layout_score->setStretchFactor(digit_wazaari, 2);
			ui->layout_score->setStretchFactor(digit_yuko, 2);
			digit_ippon->SetText("-", ScaledText::eSize_full);
		}
		else
		{
			ui->layout_score->setStretchFactor(digit_ippon, 0);
			ui->layout_score->setStretchFactor(digit_wazaari, 3);
			ui->layout_score->setStretchFactor(digit_yuko, 3);
			digit_ippon->SetText("");
		}
	}
}

//=========================================================
void View::update_wazaari(Ipponboard::EFighter who) const
//=========================================================
{
	ScaledText* digit(ui->text_wazaari_blue);

	if (eFighter_White == who)
		digit = ui->text_wazaari_white;

	const int score = m_pController->GetScore(GVF_(who), ePoint_Wazaari);
	//digit->setDigitCount( score > 9 ? 2 : 1 );
	digit->SetText(QString::number(score), ScaledText::eSize_full);
}

//=========================================================
void View::update_yuko(Ipponboard::EFighter who) const
//=========================================================
{
	ScaledText* digit(ui->text_yuko_blue);

	if (eFighter_White == who)
		digit = ui->text_yuko_white;

	const int score = m_pController->GetScore(GVF_(who), ePoint_Yuko);
	digit->SetText(QString::number(score), ScaledText::eSize_full);
}

//=========================================================
void View::update_shido(Ipponboard::EFighter who) const
//=========================================================
{
	ScaledImage* pImage1(0);
	ScaledImage* pImage2(0);
	ScaledImage* pImage3(0);

	if (eFighter_Blue == who)
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

	const int score = m_pController->GetScore(GVF_(who), ePoint_Shido);

	if (score >= 3)
	{
		pImage3->UpdateImage(":res/images/on.png");
	}
	else
	{
		if (eTypePrimary == m_Type)
			pImage3->UpdateImage(":res/images/off.png");
		else
			pImage3->UpdateImage(":res/images/off_empty.png");
	}

	if (score >= 2)
	{
		pImage2->UpdateImage(":res/images/on.png");
	}
	else
	{
		if (eTypePrimary == m_Type)
			pImage2->UpdateImage(":res/images/off.png");
		else
			pImage2->UpdateImage(":res/images/off_empty.png");
	}

	if (score >= 1)
	{
		pImage1->UpdateImage(":res/images/on.png");
	}
	else
	{
		if (eTypePrimary == m_Type)
			pImage1->UpdateImage(":res/images/off.png");
		else
			pImage1->UpdateImage(":res/images/off_empty.png");
	}
}

//=========================================================
void View::update_hansokumake(Ipponboard::EFighter who) const
//=========================================================
{
	ScaledImage* pImage(ui->image_hansokumake_blue);

//	QHBoxLayout* pLayout(ui->horizontalLayout_score_blue);
//	QVBoxLayout* pShidoLayout(ui->verticalLayout_shido_blue);
	if (eFighter_White == who)
	{
		pImage = ui->image_hansokumake_white;
//		pLayout = ui->horizontalLayout_score_white;
//		pShidoLayout = ui->verticalLayout_shido_white;
	}

	const int score_hansokumake = m_pController->GetScore(GVF_(who), ePoint_Hansokumake);
	const int score_shido = m_pController->GetScore(GVF_(who), ePoint_Shido);

	if (score_hansokumake > 0 || score_shido == 4)
	{
		pImage->UpdateImage(":res/images/on_hansokumake.png");

		if (is_secondary())
		{
//			pLayout->setStretchFactor(pImage, 3);
//			pLayout->setStretchFactor(pShidoLayout, 3);
		}
	}
	else
	{
		if (eTypePrimary == m_Type)
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
void View::update_team_score() const
//=========================================================
{
	EFighter tori(eFighter_Blue);
	EFighter uke(eFighter_White);

	if (m_Type == eTypePrimary)
	{
		uke = eFighter_Blue;
		tori = eFighter_White;
	}

#ifdef TEAM_VIEW

	if (is_secondary())
	{
		ui->text_score_team_blue_label->SetText(
			tr("Home")/*, ScaledText::eSize_full*/);

		ui->text_score_team_white_label->SetText(
			tr("Guest")/*, ScaledText::eSize_uppercase*/);
	}
	else
	{
		ui->text_score_team_white_label->SetText(
			tr("Home")/*, ScaledText::eSize_full*/);

		ui->text_score_team_blue_label->SetText(
			tr("Guest")/*, ScaledText::eSize_uppercase*/);

	}

	ui->text_score_team_blue->SetText(
		QString::number(m_pController->GetTeamScore(GVF_(eFighter_Blue))),
		ScaledText::eSize_full);

	ui->text_score_team_white->SetText(
		QString::number(m_pController->GetTeamScore(GVF_(eFighter_White))),
		ScaledText::eSize_full);
#else
	ui->text_score_team_blue_label->SetText("");
	ui->text_score_team_white_label->SetText("");
	ui->text_score_team_blue->SetText("");
	ui->text_score_team_white->SetText("");
#endif
}

//=========================================================
void View::update_hold_clock(EFighter holder, EHoldState state) const
//=========================================================
{
	const QString value(m_pController->GetTimeText(eTimer_Hold));
	const EFighter blue = GVF_(eFighter_Blue);
	const EFighter white = GVF_(eFighter_White);

	struct ColorPair
	{
		ColorPair(QColor f = Qt::lightGray,
				  QColor b = Qt::black)
			: fg(f), bg(b)
		{}

		QColor fg;
		QColor bg;
	};

	ColorPair hold_clock_colors[3][2];
	hold_clock_colors[eHoldState_on][eFighter_Blue] = ColorPair(m_TextColorBlue, m_TextBgColorBlue);
	hold_clock_colors[eHoldState_on][eFighter_White] = ColorPair(m_TextColorWhite, m_TextBgColorWhite);
	hold_clock_colors[eHoldState_off][eFighter_Blue] = ColorPair(Qt::gray, Qt::black);
	hold_clock_colors[eHoldState_off][eFighter_White] = ColorPair(Qt::gray, Qt::black);
	hold_clock_colors[eHoldState_pause][eFighter_Blue] = ColorPair(Qt::lightGray, m_TextBgColorBlue);
	hold_clock_colors[eHoldState_pause][eFighter_White] = ColorPair(Qt::darkGray, m_TextBgColorWhite);

	ScaledText* pClocks[2] =
	{
		ui->text_hold_clock_blue,
		ui->text_hold_clock_white
	};

	// reset drawing first
	pClocks[blue]->SetColor(
		hold_clock_colors[eHoldState_off][eFighter_Blue].fg,
		hold_clock_colors[eHoldState_off][eFighter_Blue].bg);

	pClocks[white]->SetColor(
		hold_clock_colors[eHoldState_off][eFighter_White].fg,
		hold_clock_colors[eHoldState_off][eFighter_White].bg);

	pClocks[blue]->SetText("00", ScaledText::eSize_full);
	pClocks[white]->SetText("00", ScaledText::eSize_full);
	ui->image_sand_clock->SetBgColor(Qt::black);

	// no one holding?
	if (eFighter_Nobody == holder)
	{
		if (is_secondary())
		{
			pClocks[eFighter_Blue]->SetText("", ScaledText::eSize_full);
			pClocks[eFighter_White]->SetText("", ScaledText::eSize_full);
			ui->image_sand_clock->UpdateImage(":res/images/off_empty.png");
		}

		return;
	}

	// set spectial hold states
	if (eHoldState_on == state)
	{
		pClocks[GVF_(holder)]->SetColor(
			hold_clock_colors[eHoldState_on][holder].fg,
			hold_clock_colors[eHoldState_on][holder].bg);

		pClocks[GVF_(holder)]->SetText(value, ScaledText::eSize_full);

		ui->image_sand_clock->SetBgColor(
			hold_clock_colors[eHoldState_on][holder].bg);
	}
	else if (eHoldState_pause == state)
	{
		pClocks[GVF_(holder)]->SetColor(
			hold_clock_colors[eHoldState_pause][holder].fg,
			hold_clock_colors[eHoldState_pause][holder].bg);

		pClocks[GVF_(holder)]->SetText(value, ScaledText::eSize_full);

		ui->image_sand_clock->SetBgColor(
			hold_clock_colors[eHoldState_on][holder].bg);
	}
	else
	{
		Q_ASSERT(!"unknown state");
		return;
	}

	// on secondary screen only holder is shown
	if (is_secondary())
	{
		ui->image_sand_clock->UpdateImage(":res/images/sand_clock.png");
//		ui->layout_info->setStretchFactor(ui->layout_name_blue, 4);
//		ui->layout_info->setStretchFactor(ui->layout_name_white, 4);

		if (eFighter_Blue == holder)
		{
			ui->layout_osaekomi->setStretchFactor(ui->text_hold_clock_blue, 7);
			ui->layout_osaekomi->setStretchFactor(ui->text_hold_clock_white, 0);
		}
		else if (eFighter_White == holder)
		{
			ui->layout_osaekomi->setStretchFactor(ui->text_hold_clock_blue, 0);
			ui->layout_osaekomi->setStretchFactor(ui->text_hold_clock_white, 7);
		}
		else
		{
			Q_ASSERT(FALSE);
		}
	}
}

//=========================================================
Ipponboard::EFighter View::GVF_(const Ipponboard::EFighter f) const
//=========================================================
{
	if (!is_secondary())
	{
		return (f == eFighter_Blue) ? eFighter_White : eFighter_Blue;
	}

	return f;
}

//=========================================================
bool View::is_secondary() const
//=========================================================
{
	return eTypeSecondary == m_Type;
}

//=========================================================
const QColor& View::get_color(const ColorType t) const
//=========================================================
{
	const bool doSwap(eTypePrimary == m_Type);

	switch (t)
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
void View::update_colors()
//=========================================================
{
	ui->text_score_team_blue_label->SetColor(get_color(blueFg), get_color(blueBg));
	ui->text_score_team_blue->SetColor(get_color(blueFg), get_color(blueBg));
	ui->dummy_blue->SetBgColor(get_color(blueBg));
	ui->image_hansokumake_blue->SetBgColor(get_color(blueBg));
	ui->image_shido3_blue->SetBgColor(get_color(blueBg));
	ui->image_shido2_blue->SetBgColor(get_color(blueBg));
	ui->image_shido1_blue->SetBgColor(get_color(blueBg));
	ui->text_yuko_blue->SetColor(get_color(blueFg), get_color(blueBg));
	ui->text_wazaari_blue->SetColor(get_color(blueFg), get_color(blueBg));
	ui->text_ippon_blue->SetColor(get_color(blueFg), get_color(blueBg));
	ui->text_ippon_desc2->SetColor(get_color(blueFg), get_color(blueBg));
	ui->text_wazaari_desc2->SetColor(get_color(blueFg), get_color(blueBg));
	ui->text_yuko_desc2->SetColor(get_color(blueFg), get_color(blueBg));
	ui->text_lastname_blue->SetColor(get_color(blueFg), get_color(blueBg));
	ui->text_firstname_blue->SetColor(get_color(blueFg), get_color(blueBg));

	ui->text_score_team_white_label->SetColor(get_color(whiteFg), get_color(whiteBg));
	ui->text_score_team_white->SetColor(get_color(whiteFg), get_color(whiteBg));
	ui->dummy_white->SetBgColor(get_color(whiteBg));
	ui->image_hansokumake_white->SetBgColor(get_color(whiteBg));
	ui->image_shido3_white->SetBgColor(get_color(whiteBg));
	ui->image_shido2_white->SetBgColor(get_color(whiteBg));
	ui->image_shido1_white->SetBgColor(get_color(whiteBg));
	ui->text_yuko_white->SetColor(get_color(whiteFg), get_color(whiteBg));
	ui->text_wazaari_white->SetColor(get_color(whiteFg), get_color(whiteBg));
	ui->text_ippon_white->SetColor(get_color(whiteFg), get_color(whiteBg));
	ui->text_ippon_desc1->SetColor(get_color(whiteFg), get_color(whiteBg));
	ui->text_wazaari_desc1->SetColor(get_color(whiteFg), get_color(whiteBg));
	ui->text_yuko_desc1->SetColor(get_color(whiteFg), get_color(whiteBg));
	ui->text_lastname_white->SetColor(get_color(whiteFg), get_color(whiteBg));
	ui->text_firstname_white->SetColor(get_color(whiteFg), get_color(whiteBg));
}

//=========================================================
void View::show_golden_score(bool show)
//=========================================================
{
	ScaledImage* pWidgetGS(ui->image_golden_score);

	if (show)
		ui->horizontalLayout_bottom->setStretchFactor(pWidgetGS, 1);
	else
		ui->horizontalLayout_bottom->setStretchFactor(pWidgetGS, 0);
}
