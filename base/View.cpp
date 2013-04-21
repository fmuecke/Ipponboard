#include "View.h"
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
	, m_TextColorFirst(Qt::black)
	, m_TextBgColorFirst(Qt::white)
	, m_TextColorSecond(Qt::white)
	, m_TextBgColorSecond(Qt::blue)
	, m_InfoTextColor(QColor(255, 255, 96)) //Qt::darkFirst
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
	ui->image_shido1_second->UpdateImage(":res/images/off.png");
	ui->image_shido2_second->UpdateImage(":res/images/off.png");
	ui->image_shido3_second->UpdateImage(":res/images/off.png");
	ui->image_shido1_first->UpdateImage(":res/images/off.png");
	ui->image_shido2_first->UpdateImage(":res/images/off.png");
	ui->image_shido3_first->UpdateImage(":res/images/off.png");
	ui->image_hansokumake_first->UpdateImage(":res/images/off.png");
	ui->image_hansokumake_second->UpdateImage(":res/images/off.png");
	ui->image_sand_clock->UpdateImage(":res/images/sand_clock.png");
	ui->dummy_first->UpdateImage(":res/images/off_empty.png");
	ui->dummy_second->UpdateImage(":res/images/off_empty.png");
	ui->image_golden_score->UpdateImage(":res/images/golden_score.png");

	ui->text_hold_clock_first->SetText("00", ScaledText::eSize_full);
	ui->text_hold_clock_second->SetText("00", ScaledText::eSize_full);

	QColor bgColor1 = get_color(firstBg);
	QColor fgColor1 = get_color(firstFg);
	QColor bgColor2 = get_color(secondBg);
	QColor fgColor2 = get_color(secondFg);

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
	ui->dummy_first->SetBgColor(bgColor1);
	ui->dummy_second->SetBgColor(bgColor2);

#ifdef TEAM_VIEW
	ui->text_score_team_first_label->SetColor(Qt::gray, Qt::black);
	ui->text_score_team_second_label->SetColor(Qt::gray, Qt::black);
	ui->text_score_team_first->SetColor(Qt::gray, Qt::black);
	ui->text_score_team_second->SetColor(Qt::gray, Qt::black);
	ui->layout_info_top->setStretchFactor(ui->text_mat, 3);
	ui->layout_info_top->setStretchFactor(ui->text_weight, 1);
#endif

	ui->text_ippon_first->SetColor(fgColor1, bgColor1);
	ui->text_wazaari_first->SetColor(fgColor1, bgColor1);
	ui->text_yuko_first->SetColor(fgColor1, bgColor1);
	ui->text_ippon_second->SetColor(fgColor2, bgColor2);
	ui->text_wazaari_second->SetColor(fgColor2, bgColor2);
	ui->text_yuko_second->SetColor(fgColor2, bgColor2);

	ui->text_lastname_first->SetColor(fgColor1, bgColor1);
	ui->text_firstname_first->SetColor(fgColor1, bgColor1);
	ui->text_lastname_second->SetColor(fgColor2, bgColor2);
	ui->text_firstname_second->SetColor(fgColor2, bgColor2);

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

	ui->image_shido1_first->SetBgColor(bgColor1);
	ui->image_shido2_first->SetBgColor(bgColor1);
	ui->image_shido3_first->SetBgColor(bgColor1);
	ui->image_hansokumake_first->SetBgColor(bgColor1);
	ui->image_shido1_second->SetBgColor(bgColor2);
	ui->image_shido2_second->SetBgColor(bgColor2);
	ui->image_shido3_second->SetBgColor(bgColor2);
	ui->image_hansokumake_second->SetBgColor(bgColor2);

//	QFontDatabase fontDb;
//	QFont newFont = fontDb.font("Bonzai", "Normal", 12 );

	SetInfoHeaderFont(QFont("Calibri", 12, QFont::Bold, false));
	SetDigitFont(QFont("Arial", 12, QFont::Bold, false));

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
	ui->text_lastname_first->SetText(
		m_pController->GetFighterLastName(GVF_(eFighter1)),
		ScaledText::eSize_uppercase);
	ui->text_lastname_second->SetText(
		m_pController->GetFighterLastName(GVF_(eFighter2)),
		ScaledText::eSize_uppercase);
	ui->text_firstname_first->SetText(
		m_pController->GetFighterFirstName(GVF_(eFighter1)),
		ScaledText::eSize_uppercase);
	ui->text_firstname_second->SetText(
		m_pController->GetFighterFirstName(GVF_(eFighter2)),
		ScaledText::eSize_uppercase);

	// first score
	update_ippon(eFighter1);
	update_wazaari(eFighter1);
	update_yuko(eFighter1);
	update_shido(eFighter1);
	update_hansokumake(eFighter1);

	// second score
	update_ippon(eFighter2);
	update_wazaari(eFighter2);
	update_yuko(eFighter2);
	update_shido(eFighter2);
	update_hansokumake(eFighter2);

	update_team_score();

	//
	// timers
	//
	ui->text_main_clock->SetText(
		m_pController->GetTimeText(eTimer_Main),
		ScaledText::eSize_full);

	const EFighter holder(m_pController->GetLastHolder());

	if (eFighterNobody == holder && is_secondary())
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
				ui->layout_info->setStretchFactor(ui->layout_name_first, 3);
				ui->layout_info->setStretchFactor(ui->layout_osaekomi, 2);
				ui->layout_info->setStretchFactor(ui->layout_name_second, 3);
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
//	ui->text_lastname_first->SetText(text);
//	update();
//#endif

	// Note: with update() the area is scheduled for a redraw
	//       while repaint() does this immediately.
	repaint();
	// This is really important, or timer values are not redrawn!
}

//=========================================================
void View::SetInfoHeaderFont(const QFont& font)
//=========================================================
{
	m_InfoHeaderFont = font;

	ui->text_weight->SetFont(font);
	ui->text_mat->SetFont(font);

#ifdef TEAM_VIEW
	ui->text_score_team_first_label->SetFont(font);
	ui->text_score_team_second_label->SetFont(font);
#endif
}

//=========================================================
void View::SetFighterNameFont(const QFont& font)
//=========================================================
{
	m_FighterNameFont = font;

	ui->text_lastname_second->setAlignment(Qt::AlignLeft);
	ui->text_firstname_second->setAlignment(Qt::AlignLeft);
	ui->text_lastname_first->setAlignment(Qt::AlignRight);
	ui->text_firstname_first->setAlignment(Qt::AlignRight);

	ui->text_lastname_second->SetFont(font);
	ui->text_firstname_second->SetFont(font);
	ui->text_lastname_first->SetFont(font);
	ui->text_firstname_first->SetFont(font);
}

//=========================================================
void View::SetDigitFont(const QFont& font)
//=========================================================
{
	m_DigitFont = font;

	ui->text_main_clock->SetFont(font);
	ui->text_ippon_first->SetFont(font);
	ui->text_wazaari_first->SetFont(font);
	ui->text_yuko_first->SetFont(font);
	ui->text_ippon_second->SetFont(font);
	ui->text_wazaari_second->SetFont(font);
	ui->text_yuko_second->SetFont(font);
	ui->text_hold_clock_first->SetFont(font);
	ui->text_hold_clock_second->SetFont(font);
#ifdef TEAM_VIEW
	ui->text_score_team_first->SetFont(font);
	ui->text_score_team_second->SetFont(font);
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
void View::SetTextColorFirst(const QColor& color, const QColor& bgColor)
//=========================================================
{
	m_TextColorFirst = color;
	m_TextBgColorFirst = bgColor;

	update_colors();
}

//=========================================================
void View::SetTextColorSecond(const QColor& color, const QColor& bgColor)
//=========================================================
{
	m_TextColorSecond = color;
	m_TextBgColorSecond = bgColor;

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
	ui->text_hold_clock_first->SetColor(Qt::gray, Qt::black);
	ui->text_hold_clock_second->SetColor(Qt::gray, Qt::black);
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

	EFighter whos(eFighterNobody);
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

				item = menu.addAction(tr("Set time"));
				connect(item, SIGNAL(triggered()), this, SLOT(setMainTimerValue_()));

				item = menu.addAction(tr("Reset"));
				connect(item, SIGNAL(triggered()), this, SLOT(resetMainTimerValue_()));

				menu.exec(QCursor::pos());

				return;
			}

			action = eAction_Hajime_Mate;
		}
		else if (child == ui->text_hold_clock_first)
		{
			if (doRevoke)   // right click!
			{
				action = eAction_ResetOsaeKomi;
			}
			else
			{
				whos = eFighter1;

				if (eState_Holding == m_pController->GetCurrentState() &&
						GVF_(eFighter1) != m_pController->GetLead())
				{
					action = eAction_SetOsaekomi;
				}
				else
				{
					action = eAction_OsaeKomi_Toketa;
				}
			}
		}
		else if (child == ui->text_hold_clock_second)
		{
			if (doRevoke)   // right click!
			{
				action = eAction_ResetOsaeKomi;
			}
			else
			{
				whos = eFighter2;

				if (eState_Holding == m_pController->GetCurrentState() &&
						GVF_(eFighter2) != m_pController->GetLead())
				{
					action = eAction_SetOsaekomi;
				}
				else
				{
					action = eAction_OsaeKomi_Toketa;
				}
			}
		}
		else if (child == ui->text_ippon_second)
		{
			whos = eFighter2;
			action = eAction_Ippon;
		}
		else if (child == ui->text_ippon_first)
		{
			whos = eFighter1;
			action = eAction_Ippon;
		}
		else if (child == ui->text_wazaari_second)
		{
			whos = eFighter2;
			action = eAction_Wazaari;
		}
		else if (child == ui->text_wazaari_first)
		{
			whos = eFighter1;
			action = eAction_Wazaari;
		}
		else if (child == ui->text_yuko_second)
		{
			whos = eFighter2;
			action = eAction_Yuko;
		}
		else if (child == ui->text_yuko_first)
		{
			whos = eFighter1;
			action = eAction_Yuko;
		}

		//else
		//{
		//	Q_ASSERT(!"action not defined for pointer!");
		//}
	}
	else
	{
		if (child == ui->image_shido1_second)
		{
			whos = eFighter2;
			action = eAction_Shido;
		}
		else if (child == ui->image_shido3_second ||
				 child == ui->image_shido2_second ||
				 child == ui->image_shido1_second)
		{
			whos = eFighter2;
			action = eAction_Shido;
		}
		else if (child == ui->image_shido3_first ||
				 child == ui->image_shido2_first ||
				 child == ui->image_shido1_first)
		{
			whos = eFighter1;
			action = eAction_Shido;
		}
		else if (child == ui->image_hansokumake_second)
		{
			whos = eFighter2;
			action = eAction_Hansokumake;
		}
		else if (child == ui->image_hansokumake_first)
		{
			whos = eFighter1;
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
void View::setOsaekomiFirst_()
//=========================================================
{
	m_pController->DoAction(eAction_SetOsaekomi, eFighter1, false/*doRevoke*/);
}

//=========================================================
void View::setOsaekomiSecond_()
//=========================================================
{
	m_pController->DoAction(eAction_SetOsaekomi, eFighter2, false/*doRevoke*/);
}

//=========================================================
void View::resetMainTimerValue_()
//=========================================================
{
	m_pController->DoAction(eAction_ResetMainTimer, eFighterNobody, true/*doRevoke*/);
}

//=========================================================
void View::setMainTimerValue_()
//=========================================================
{
	// Note: this is implemented in the MainWindowBase as well!

	//if( m_pController->GetCurrentState() == eState_SonoMama ||
	//  m_pController->GetCurrentState() == eState_TimerStopped )
	{
		bool ok(false);
		const QString time = QInputDialog::getText(
								 this,
								 tr("Set Value"),
								 tr("Set value to (m:ss):"),
								 QLineEdit::Normal,
								 m_pController->GetTimeText(eTimer_Main),
								 &ok);

		if (ok)
		{
			m_pController->SetTimerValue(eTimer_Main, time);
		}
	}
}

//=========================================================
void View::blink_()
//=========================================================
{
	m_drawIppon = !m_drawIppon;

	if (is_secondary())
	{
		update_ippon(eFighter1);
		update_ippon(eFighter2);
	}
}

//=========================================================
void View::update_ippon(Ipponboard::EFighter who) const
//=========================================================
{
	ScaledText* digit_ippon(ui->text_ippon_first);
	QVBoxLayout* digit_wazaari(ui->layout_wazaari_first);
	QVBoxLayout* digit_yuko(ui->layout_yuko_first);
	EFighter uke(eFighter2);

	if (eFighter2 == who)
	{
		digit_ippon = ui->text_ippon_second;
		digit_wazaari = ui->layout_wazaari_second;
		digit_yuko = ui->layout_yuko_second;
		uke = eFighter1;
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
	ScaledText* digit(ui->text_wazaari_first);

	if (eFighter2 == who)
		digit = ui->text_wazaari_second;

	const int score = m_pController->GetScore(GVF_(who), ePoint_Wazaari);
	//digit->setDigitCount( score > 9 ? 2 : 1 );
	digit->SetText(QString::number(score), ScaledText::eSize_full);
}

//=========================================================
void View::update_yuko(Ipponboard::EFighter who) const
//=========================================================
{
	ScaledText* digit(ui->text_yuko_first);

	if (eFighter2 == who)
		digit = ui->text_yuko_second;

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

	if (eFighter1 == who)
	{
		pImage1 = ui->image_shido1_first;
		pImage2 = ui->image_shido2_first;
		pImage3 = ui->image_shido3_first;
	}
	else
	{
		pImage1 = ui->image_shido1_second;
		pImage2 = ui->image_shido2_second;
		pImage3 = ui->image_shido3_second;
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
	ScaledImage* pImage(ui->image_hansokumake_first);

//	QHBoxLayout* pLayout(ui->horizontalLayout_score_first);
//	QVBoxLayout* pShidoLayout(ui->verticalLayout_shido_first);
	if (eFighter2 == who)
	{
		pImage = ui->image_hansokumake_second;
//		pLayout = ui->horizontalLayout_score_second;
//		pShidoLayout = ui->verticalLayout_shido_second;
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
	//unused code...
	//EFighter tori(eFighter1);
	//EFighter uke(eFighter2);
	//
	//if (m_Type == eTypePrimary)
	//{
	//    uke = eFighter1;
	//    tori = eFighter2;
	//}

#ifdef TEAM_VIEW

	if (is_secondary())
	{
		ui->text_score_team_first_label->SetText(m_pController->GetHomeLabel());
		ui->text_score_team_second_label->SetText(m_pController->GetGuestLabel());
	}
	else
	{
		ui->text_score_team_second_label->SetText(m_pController->GetHomeLabel());
		ui->text_score_team_first_label->SetText(m_pController->GetGuestLabel());
	}

	ui->text_score_team_first->SetText(
		QString::number(m_pController->GetTeamScore(GVF_(eFighter1))),
		ScaledText::eSize_full);

	ui->text_score_team_second->SetText(
		QString::number(m_pController->GetTeamScore(GVF_(eFighter2))),
		ScaledText::eSize_full);
#else
	ui->text_score_team_first_label->SetText("");
	ui->text_score_team_second_label->SetText("");
	ui->text_score_team_first->SetText("");
	ui->text_score_team_second->SetText("");
#endif
}

//=========================================================
void View::update_hold_clock(EFighter holder, EHoldState state) const
//=========================================================
{
	const QString value(m_pController->GetTimeText(eTimer_Hold));
	const EFighter first = GVF_(eFighter1);
	const EFighter second = GVF_(eFighter2);

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
	hold_clock_colors[eHoldState_on][eFighter1] = ColorPair(m_TextColorFirst, m_TextBgColorFirst);
	hold_clock_colors[eHoldState_on][eFighter2] = ColorPair(m_TextColorSecond, m_TextBgColorSecond);
	hold_clock_colors[eHoldState_off][eFighter1] = ColorPair(Qt::gray, Qt::black);
	hold_clock_colors[eHoldState_off][eFighter2] = ColorPair(Qt::gray, Qt::black);
	hold_clock_colors[eHoldState_pause][eFighter1] = ColorPair(Qt::lightGray, m_TextBgColorFirst);
	hold_clock_colors[eHoldState_pause][eFighter2] = ColorPair(Qt::darkGray, m_TextBgColorSecond);

	ScaledText* pClocks[2] =
	{
		ui->text_hold_clock_first,
		ui->text_hold_clock_second
	};

	// reset drawing first
	pClocks[first]->SetColor(
		hold_clock_colors[eHoldState_off][eFighter1].fg,
		hold_clock_colors[eHoldState_off][eFighter1].bg);

	pClocks[second]->SetColor(
		hold_clock_colors[eHoldState_off][eFighter2].fg,
		hold_clock_colors[eHoldState_off][eFighter2].bg);

	pClocks[first]->SetText("00", ScaledText::eSize_full);
	pClocks[second]->SetText("00", ScaledText::eSize_full);
	ui->image_sand_clock->SetBgColor(Qt::black);

	// no one holding?
	if (eFighterNobody == holder)
	{
		if (is_secondary())
		{
			pClocks[eFighter1]->SetText("", ScaledText::eSize_full);
			pClocks[eFighter2]->SetText("", ScaledText::eSize_full);
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
//		ui->layout_info->setStretchFactor(ui->layout_name_first, 4);
//		ui->layout_info->setStretchFactor(ui->layout_name_second, 4);

		if (eFighter1 == holder)
		{
			ui->layout_osaekomi->setStretchFactor(ui->text_hold_clock_first, 7);
			ui->layout_osaekomi->setStretchFactor(ui->text_hold_clock_second, 0);
		}
		else if (eFighter2 == holder)
		{
			ui->layout_osaekomi->setStretchFactor(ui->text_hold_clock_first, 0);
			ui->layout_osaekomi->setStretchFactor(ui->text_hold_clock_second, 7);
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
		return (f == eFighter1) ? eFighter2 : eFighter1;
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
	case firstFg:
		return doSwap ? m_TextColorSecond : m_TextColorFirst;

	case firstBg:
		return doSwap ? m_TextBgColorSecond : m_TextBgColorFirst;

	case secondFg:
		return doSwap ? m_TextColorFirst : m_TextColorSecond;

	case secondBg:
		return doSwap ? m_TextBgColorFirst : m_TextBgColorSecond;

	default:
		assert(!"unhandled switch case!");
	}

	return m_TextBgColorFirst; // just for debug purpose...
}

//=========================================================
void View::update_colors()
//=========================================================
{
	ui->text_score_team_first_label->SetColor(get_color(firstFg), get_color(firstBg));
	ui->text_score_team_first->SetColor(get_color(firstFg), get_color(firstBg));
	ui->dummy_first->SetBgColor(get_color(firstBg));
	ui->image_hansokumake_first->SetBgColor(get_color(firstBg));
	ui->image_shido3_first->SetBgColor(get_color(firstBg));
	ui->image_shido2_first->SetBgColor(get_color(firstBg));
	ui->image_shido1_first->SetBgColor(get_color(firstBg));
	ui->text_yuko_first->SetColor(get_color(firstFg), get_color(firstBg));
	ui->text_wazaari_first->SetColor(get_color(firstFg), get_color(firstBg));
	ui->text_ippon_first->SetColor(get_color(firstFg), get_color(firstBg));
	ui->text_ippon_desc2->SetColor(get_color(firstFg), get_color(firstBg));
	ui->text_wazaari_desc2->SetColor(get_color(firstFg), get_color(firstBg));
	ui->text_yuko_desc2->SetColor(get_color(firstFg), get_color(firstBg));
	ui->text_lastname_first->SetColor(get_color(firstFg), get_color(firstBg));
	ui->text_firstname_first->SetColor(get_color(firstFg), get_color(firstBg));

	ui->text_score_team_second_label->SetColor(get_color(secondFg), get_color(secondBg));
	ui->text_score_team_second->SetColor(get_color(secondFg), get_color(secondBg));
	ui->dummy_second->SetBgColor(get_color(secondBg));
	ui->image_hansokumake_second->SetBgColor(get_color(secondBg));
	ui->image_shido3_second->SetBgColor(get_color(secondBg));
	ui->image_shido2_second->SetBgColor(get_color(secondBg));
	ui->image_shido1_second->SetBgColor(get_color(secondBg));
	ui->text_yuko_second->SetColor(get_color(secondFg), get_color(secondBg));
	ui->text_wazaari_second->SetColor(get_color(secondFg), get_color(secondBg));
	ui->text_ippon_second->SetColor(get_color(secondFg), get_color(secondBg));
	ui->text_ippon_desc1->SetColor(get_color(secondFg), get_color(secondBg));
	ui->text_wazaari_desc1->SetColor(get_color(secondFg), get_color(secondBg));
	ui->text_yuko_desc1->SetColor(get_color(secondFg), get_color(secondBg));
	ui->text_lastname_second->SetColor(get_color(secondFg), get_color(secondBg));
	ui->text_firstname_second->SetColor(get_color(secondFg), get_color(secondBg));
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
