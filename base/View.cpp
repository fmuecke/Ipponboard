// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "View.h"

#include "../core/Rules.h"
#include "ui_view_horizontal.h"

#include <QFontDatabase>
#include <QInputDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QResource>
#include <QTime>
#include <QTimer>

static const int resourceInitialized = []()
{
    Q_INIT_RESOURCE(ipponboard);
    return 0;
}();

using namespace Ipponboard;
using Point = Score::Point;

constexpr auto kImageOn = ":res/images/on.png";
constexpr auto kImageOff = ":res/images/off.png";
constexpr auto kImageEmpty = ":res/images/off_empty.png";

//=========================================================
View::View(IController* pController, EditionType edition, EType type, QWidget* parent)
    : QWidget(parent),
      m_Edition(edition),
      m_Type(type),
      m_pController(pController),
      ui(new Ui::ScoreViewHorizontal),
      m_InfoHeaderFont("Calibri", 12, QFont::Bold, false),
      m_AthleteNameFont("Calibri", 12, QFont::Bold, true),
      m_DigitFont("Calibri", 12, QFont::Bold, false),
      m_TextColorFirst(Qt::black),
      m_TextBgColorFirst(Qt::white),
      m_TextColorSecond(Qt::white),
      m_TextBgColorSecond(Qt::blue),
      m_InfoTextColor(QColor(255, 255, 96)) //Qt::darkFirst
      ,
      m_InfoTextBgColor(Qt::black) //Qt::lightGray
      ,
      m_MainClockBgColor(Qt::black),
      m_MainClockColorRunning(Qt::yellow),
      m_MainClockColorStopped(Qt::red),
      m_mat(QCoreApplication::applicationName() + " v" + QCoreApplication::applicationVersion()),
      m_weight(""),
      m_category(""),
      m_drawIppon(false),
      m_showInfoHeader(true),
      m_pBlinkTimer(nullptr)
//=========================================================
{
    // init widgets
    ui->setupUi(this);
    ui->image_shido1_second->UpdateImage(kImageOff);
    ui->image_shido2_second->UpdateImage(kImageOff);
    ui->image_shido3_second->UpdateImage(kImageOff);
    ui->image_shido1_first->UpdateImage(kImageOff);
    ui->image_shido2_first->UpdateImage(kImageOff);
    ui->image_shido3_first->UpdateImage(kImageOff);
    ui->image_hansokumake_first->UpdateImage(kImageOff);
    ui->image_hansokumake_second->UpdateImage(kImageOff);
    ui->image_sand_clock->UpdateImage(":res/images/sand_clock.png");
    ui->dummy_first->UpdateImage(":res/images/off_empty.png");
    ui->dummy_second->UpdateImage(":res/images/off_empty.png");
    ui->image_golden_score->UpdateImage(":res/images/golden_score.png");
    ui->image_golden_score->SetBgColor(Qt::black);

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
    ui->text_ippon_desc1->SetText("I");
    ui->text_wazaari_desc1->SetText("W");
    ui->text_yuko_desc1->SetText("Y");
    ui->text_ippon_desc2->setFont(descFont);
    ui->text_wazaari_desc2->setFont(descFont);
    ui->text_yuko_desc2->setFont(descFont);
    ui->text_ippon_desc2->SetColor(fgColor2, bgColor2);
    ui->text_wazaari_desc2->SetColor(fgColor2, bgColor2);
    ui->text_yuko_desc2->SetColor(fgColor2, bgColor2);
    ui->text_ippon_desc2->SetText("I");
    ui->text_wazaari_desc2->SetText("W");
    ui->text_yuko_desc2->SetText("Y");
    ui->text_ippon_desc1->SetColor(fgColor1, bgColor1);
    ui->text_wazaari_desc1->SetColor(fgColor1, bgColor1);
    ui->text_yuko_desc1->SetColor(fgColor1, bgColor1);
    ui->dummy_first->SetBgColor(bgColor1);
    ui->dummy_second->SetBgColor(bgColor2);

    if (m_Edition == EditionType::Team)
    {
        ui->text_score_team_first_label->SetColor(Qt::gray, Qt::black);
        ui->text_score_team_second_label->SetColor(Qt::gray, Qt::black);
        ui->text_score_team_first->SetColor(Qt::gray, Qt::black);
        ui->text_score_team_second->SetColor(Qt::gray, Qt::black);
        ui->layout_info_top->setStretchFactor(ui->text_mat, 3);
        ui->layout_info_top->setStretchFactor(ui->text_weight, 1);
    }

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
        ui->text_ippon_desc1->hide();
        ui->text_ippon_desc2->hide();
        ui->text_wazaari_desc1->SetText("Waza-ari");
        ui->text_wazaari_desc1->SetText("Waza-ari");
        ui->text_yuko_desc1->SetText("Yuko");
        ui->text_yuko_desc2->SetText("Yuko");
    }
    else
    {
        ui->text_ippon_desc1->show();
        ui->text_ippon_desc2->show();
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
    connect(m_pBlinkTimer, &QTimer::timeout, this, &View::blink_);
}

//=========================================================
View::~View()
//=========================================================
{
    m_pController = nullptr;
    delete ui;
}

//=========================================================
void View::UpdateView()
//=========================================================
{
    Q_ASSERT(m_pController && "Controller not set!");

    if (m_pController->GetRules()->GetMaxShidoCount() < 3)
    {
        ui->image_shido3_first->hide();
        ui->image_shido3_second->hide();
    }
    else
    {
        ui->image_shido3_first->show();
        ui->image_shido3_second->show();
    }

    if (m_pController->GetRules()->IsOption_HasYuko())
    {
        ui->text_yuko_first->show();
        ui->text_yuko_second->show();
        ui->text_yuko_desc1->show();
        ui->text_yuko_desc2->show();
    }
    else
    {
        ui->text_yuko_first->hide();
        ui->text_yuko_second->hide();
        ui->text_yuko_desc1->hide();
        ui->text_yuko_desc2->hide();
    }

    show_golden_score(m_pController->IsGoldenScore());

    update_colors();

    ui->text_mat->SetText(m_mat, ScaledText::eSize_normal);

    // display weight class
    if (m_Edition == EditionType::Team)
    {
        QString infoText /*(tr("Contest ").toUpper())*/;
        //infoText += QString::number(m_pController->GetRound()) + ": ";
        infoText += m_pController->GetWeight();
        ui->text_weight->SetText(infoText, ScaledText::eSize_normal);
    }
    else
    {
        QString infoText(m_category);

        if (!infoText.isEmpty())
            infoText += "  ";

        infoText += m_weight; //.toUpper();
        ui->text_weight->SetText(infoText, ScaledText::eSize_normal);
    }

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
    // athlete names
    //
    ui->text_lastname_first->SetText(
        m_pController->GetAthleteLastName(MapDisplayPositionToContestSide(ContestSide::SideA)),
        ScaledText::eSize_uppercase);
    ui->text_lastname_second->SetText(
        m_pController->GetAthleteLastName(MapDisplayPositionToContestSide(ContestSide::SideB)),
        ScaledText::eSize_uppercase);
    ui->text_firstname_first->SetText(
        m_pController->GetAthleteFirstName(MapDisplayPositionToContestSide(ContestSide::SideA)),
        ScaledText::eSize_uppercase);
    ui->text_firstname_second->SetText(
        m_pController->GetAthleteFirstName(MapDisplayPositionToContestSide(ContestSide::SideB)),
        ScaledText::eSize_uppercase);

    // side A score
    update_ippon(ContestSide::SideA);
    update_wazaari(ContestSide::SideA);
    update_yuko(ContestSide::SideA);
    update_shido(ContestSide::SideA);
    update_hansokumake(ContestSide::SideA);

    // side B score
    update_ippon(ContestSide::SideB);
    update_wazaari(ContestSide::SideB);
    update_yuko(ContestSide::SideB);
    update_shido(ContestSide::SideB);
    update_hansokumake(ContestSide::SideB);

    update_team_score();

    //
    // timers
    //
    ui->text_main_clock->SetText(m_pController->GetTimeText(eTimer_Main), ScaledText::eSize_full);

    const ContestSide holder(m_pController->GetCurrentHoldSide());

    if (ContestSide::None == holder && is_secondary())
    {
        ui->layout_info->setStretchFactor(ui->layout_osaekomi, 0);
    }

    switch (m_pController->GetCurrentState())
    {
    case eState_TimerRunning:
    {
        ui->text_main_clock->SetColor(m_MainClockColorRunning, m_MainClockBgColor);
        update_hold_clock(holder, eHoldState_pause);
    }
    break;

    case eState_TimerStopped:
    {
        ui->text_main_clock->SetColor(m_MainClockColorStopped, m_MainClockBgColor);
        update_hold_clock(holder, eHoldState_pause);
    }
    break;

    case eState_Holding:
    {
        ui->text_main_clock->SetColor(m_MainClockColorRunning, m_MainClockBgColor);
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
    //const ContestSide lead( m_pController->GetLead() );
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

    if (m_Edition == EditionType::Team)
    {
        ui->text_score_team_first_label->SetFont(font);
        ui->text_score_team_second_label->SetFont(font);
    }
}

//=========================================================
void View::SetAthleteNameFont(const QFont& font)
//=========================================================
{
    m_AthleteNameFont = font;

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

    if (m_Edition == EditionType::Team)
    {

        ui->text_score_team_first->SetFont(font);
        ui->text_score_team_second->SetFont(font);
    }
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

    ContestSide whos(ContestSide::None);
    EAction action(eAction_NONE);
    const bool doRevoke = event->button() & Qt::RightButton;

    ScaledImage* imageChild = dynamic_cast<ScaledImage*>(childAt(event->pos()));

    if (!imageChild)
    {
        ScaledText* textChild = dynamic_cast<ScaledText*>(childAt(event->pos()));

        if (!textChild)
            return;

        if (textChild == ui->text_main_clock)
        {
            if (doRevoke) // right click!
            {
                QMenu menu;
                QAction* item(nullptr);

                item = menu.addAction(tr("Set time"));
                connect(item, &QAction::triggered, this, [this]() { setMainTimerValue_(); });

                item = menu.addAction(tr("Reset"));
                connect(item, &QAction::triggered, this, [this]() { resetMainTimerValue_(); });

                menu.exec(QCursor::pos());

                return;
            }

            action = eAction_Hajime_Mate;
        }
        else if (textChild == ui->text_hold_clock_first)
        {
            if (doRevoke) // right click!
            {
                action = eAction_ResetOsaeKomi;
            }
            else
            {
                whos = ContestSide::SideA;

                if (eState_Holding == m_pController->GetCurrentState() &&
                    MapDisplayPositionToContestSide(ContestSide::SideA) !=
                        m_pController->GetLeadingSide())
                {
                    action = eAction_SetOsaekomi;
                }
                else
                {
                    action = eAction_OsaeKomi_Toketa;
                }
            }
        }
        else if (textChild == ui->text_hold_clock_second)
        {
            if (doRevoke) // right click!
            {
                action = eAction_ResetOsaeKomi;
            }
            else
            {
                whos = ContestSide::SideB;

                if (eState_Holding == m_pController->GetCurrentState() &&
                    MapDisplayPositionToContestSide(ContestSide::SideB) !=
                        m_pController->GetLeadingSide())
                {
                    action = eAction_SetOsaekomi;
                }
                else
                {
                    action = eAction_OsaeKomi_Toketa;
                }
            }
        }
        else if (textChild == ui->text_ippon_second)
        {
            whos = ContestSide::SideB;
            action = eAction_Ippon;
        }
        else if (textChild == ui->text_ippon_first)
        {
            whos = ContestSide::SideA;
            action = eAction_Ippon;
        }
        else if (textChild == ui->text_wazaari_second)
        {
            whos = ContestSide::SideB;
            action = eAction_Wazaari;
        }
        else if (textChild == ui->text_wazaari_first)
        {
            whos = ContestSide::SideA;
            action = eAction_Wazaari;
        }
        else if (textChild == ui->text_yuko_second)
        {
            whos = ContestSide::SideB;
            action = eAction_Yuko;
        }
        else if (textChild == ui->text_yuko_first)
        {
            whos = ContestSide::SideA;
            action = eAction_Yuko;
        }

        //else
        //{
        //	Q_ASSERT(!"action not defined for pointer!");
        //}
    }
    else
    {
        if (imageChild == ui->image_shido1_second)
        {
            whos = ContestSide::SideB;
            action = eAction_Shido;
        }
        else if (imageChild == ui->image_shido3_second || imageChild == ui->image_shido2_second ||
                 imageChild == ui->image_shido1_second)
        {
            whos = ContestSide::SideB;
            action = eAction_Shido;
        }
        else if (imageChild == ui->image_shido3_first || imageChild == ui->image_shido2_first ||
                 imageChild == ui->image_shido1_first)
        {
            whos = ContestSide::SideA;
            action = eAction_Shido;
        }
        else if (imageChild == ui->image_hansokumake_second)
        {
            whos = ContestSide::SideB;
            action = eAction_Hansokumake;
        }
        else if (imageChild == ui->image_hansokumake_first)
        {
            whos = ContestSide::SideA;
            action = eAction_Hansokumake;
        }

        //else
        //{
        //	Q_ASSERT(!"action not defined for pointer!");
        //}
    }

    if (IsContestSide(whos))
    {
        whos = MapDisplayPositionToContestSide(whos);
    }
    m_pController->DoAction(action, whos, doRevoke);
}

//=========================================================
void View::setOsaekomiFirst_()
//=========================================================
{
    m_pController->DoAction(eAction_SetOsaekomi, ContestSide::SideA, false /*doRevoke*/);
}

//=========================================================
void View::setOsaekomiSecond_()
//=========================================================
{
    m_pController->DoAction(eAction_SetOsaekomi, ContestSide::SideB, false /*doRevoke*/);
}

//=========================================================
void View::resetMainTimerValue_()
//=========================================================
{
    m_pController->DoAction(eAction_ResetMainTimer, ContestSide::None, true /*doRevoke*/);
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
        this->setStyleSheet("");
        const QString time = QInputDialog::getText(this,
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
        update_ippon(ContestSide::SideA);
        update_ippon(ContestSide::SideB);
    }
}

//=========================================================
void View::update_ippon(Ipponboard::ContestSide who) const
//=========================================================
{
    auto digit_ippon = ui->text_ippon_first;
    auto digit_wazaari = ui->text_wazaari_first;
    auto digit_yuko = ui->text_yuko_first;
    auto wazaariLabel = ui->text_wazaari_desc1;
    auto yukoLabel = ui->text_yuko_desc1;
    auto scoreLayout = ui->layout_score_first;
    ContestSide opposingSide(ContestSide::SideB);

    if (opposingSide == who)
    {
        scoreLayout = ui->layout_score_second;
        digit_ippon = ui->text_ippon_second;
        digit_wazaari = ui->text_wazaari_second;
        digit_yuko = ui->text_yuko_second;
        wazaariLabel = ui->text_wazaari_desc2;
        yukoLabel = ui->text_yuko_desc2;
        opposingSide = ContestSide::SideA;
    }

    const int score = m_pController->GetScore(MapDisplayPositionToContestSide(who), Point::Ippon);

    if (score != 0)
    {
        Q_ASSERT(m_pBlinkTimer);

        if (!m_pBlinkTimer->isActive())
            m_pBlinkTimer->start(750);

        if (is_secondary())
        {
            if (m_drawIppon)
            {
                digit_ippon->show();
                digit_wazaari->hide();
                wazaariLabel->SetText("");

                if (m_pController->GetRules()->IsOption_HasYuko())
                {
                    digit_yuko->hide();
                    yukoLabel->SetText("");
                }
            }
            else
            {
                digit_ippon->hide();
                digit_wazaari->show();
                wazaariLabel->SetText("Waza-ari");

                if (m_pController->GetRules()->IsOption_HasYuko())
                {
                    digit_yuko->show();
                    yukoLabel->SetText("Yuko");
                }
            }
        }
        else
        {
            digit_ippon->show();
            digit_wazaari->show();
            wazaariLabel->SetText("W");

            if (m_pController->GetRules()->IsOption_HasYuko())
            {
                digit_yuko->show();
                yukoLabel->SetText("Y");
            }
        }

        digit_ippon->SetText("IPPON", ScaledText::eSize_full, !is_secondary());
    }
    else
    {
        const int opposingScore =
            m_pController->GetScore(MapDisplayPositionToContestSide(opposingSide), Point::Ippon);

        if (m_pBlinkTimer->isActive() && 0 == opposingScore)
        {
            m_pBlinkTimer->stop();
        }

        digit_ippon->SetBlinking(false);

        if (!is_secondary())
        {
            digit_ippon->show();
            digit_wazaari->show();
            wazaariLabel->SetText("W");

            if (m_pController->GetRules()->IsOption_HasYuko())
            {
                digit_yuko->show();
                yukoLabel->SetText("Y");
            }

            digit_ippon->SetText("-", ScaledText::eSize_full);
        }
        else
        {
            digit_ippon->hide();
            digit_wazaari->show();
            wazaariLabel->SetText("Waza-ari");

            if (m_pController->GetRules()->IsOption_HasYuko())
            {
                digit_yuko->show();
                yukoLabel->SetText("Yuko");
            }

            digit_ippon->SetText("");
        }
    }
}

//=========================================================
void View::update_wazaari(Ipponboard::ContestSide who) const
//=========================================================
{
    ScaledText* digit(ui->text_wazaari_first);

    if (ContestSide::SideB == who)
        digit = ui->text_wazaari_second;

    const int score = m_pController->GetScore(MapDisplayPositionToContestSide(who), Point::Wazaari);
    //digit->setDigitCount( score > 9 ? 2 : 1 );
    digit->SetText(QString::number(score), ScaledText::eSize_full);
}

//=========================================================
void View::update_yuko(Ipponboard::ContestSide who) const
//=========================================================
{
    ScaledText* digit(ui->text_yuko_first);

    if (ContestSide::SideB == who)
        digit = ui->text_yuko_second;

    const int score = m_pController->GetScore(MapDisplayPositionToContestSide(who), Point::Yuko);
    digit->SetText(QString::number(score), ScaledText::eSize_full);
}

//=========================================================
void View::update_shido(Ipponboard::ContestSide who) const
//=========================================================
{
    ScaledImage* pImage1(nullptr);
    ScaledImage* pImage2(nullptr);
    ScaledImage* pImage3(nullptr);

    if (ContestSide::SideA == who)
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

    const int score = m_pController->GetScore(MapDisplayPositionToContestSide(who), Point::Shido);

    pImage3->UpdateImage(score >= 3 ? kImageOn : eTypePrimary == m_Type ? kImageOff : kImageEmpty);
    pImage2->UpdateImage(score >= 2 ? kImageOn : eTypePrimary == m_Type ? kImageOff : kImageEmpty);
    pImage1->UpdateImage(score >= 1 ? kImageOn : eTypePrimary == m_Type ? kImageOff : kImageEmpty);
}

//=========================================================
void View::update_hansokumake(Ipponboard::ContestSide who) const
//=========================================================
{
    ScaledImage* pImage(ui->image_hansokumake_first);

    if (ContestSide::SideB == who)
    {
        pImage = ui->image_hansokumake_second;
    }

    const int score_hansokumake =
        m_pController->GetScore(MapDisplayPositionToContestSide(who), Point::Hansokumake);
    const int score_shido =
        m_pController->GetScore(MapDisplayPositionToContestSide(who), Point::Shido);

    if (score_hansokumake > 0 || score_shido == m_pController->GetRules()->GetMaxShidoCount() + 1)
    {
        pImage->UpdateImage(":res/images/on_hansokumake.png");
    }
    else
    {
        pImage->UpdateImage(eTypePrimary == m_Type ? ":res/images/off_hansokumake.png"
                                                   : ":res/images/off_empty.png");
    }
}

//=========================================================
void View::update_team_score() const
//=========================================================
{
    if (m_Edition == EditionType::Team)
    {
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
            QString::number(
                m_pController->GetTeamScore(MapDisplayPositionToContestSide(ContestSide::SideA))),
            ScaledText::eSize_full);

        ui->text_score_team_second->SetText(
            QString::number(
                m_pController->GetTeamScore(MapDisplayPositionToContestSide(ContestSide::SideB))),
            ScaledText::eSize_full);
    }
    else
    {
        ui->text_score_team_first_label->SetText("");
        ui->text_score_team_second_label->SetText("");
        ui->text_score_team_first->SetText("");
        ui->text_score_team_second->SetText("");
    }
}

//=========================================================
void View::update_hold_clock(ContestSide holder, EHoldState state) const
//=========================================================
{
    const QString value(m_pController->GetTimeText(eTimer_Hold));
    const ContestSide sideA = MapDisplayPositionToContestSide(ContestSide::SideA);
    const ContestSide sideB = MapDisplayPositionToContestSide(ContestSide::SideB);

    struct ColorPair
    {
        ColorPair(QColor f = Qt::lightGray, QColor b = Qt::black) : fg(f), bg(b) {}

        QColor fg;
        QColor bg;
    };

    ColorPair hold_clock_colors[3][ToIndex(ContestSide::Count)];
    hold_clock_colors[eHoldState_on][ToIndex(ContestSide::SideA)] =
        ColorPair(m_TextColorFirst, m_TextBgColorFirst);
    hold_clock_colors[eHoldState_on][ToIndex(ContestSide::SideB)] =
        ColorPair(m_TextColorSecond, m_TextBgColorSecond);
    hold_clock_colors[eHoldState_off][ToIndex(ContestSide::SideA)] = ColorPair(Qt::gray, Qt::black);
    hold_clock_colors[eHoldState_off][ToIndex(ContestSide::SideB)] = ColorPair(Qt::gray, Qt::black);
    hold_clock_colors[eHoldState_pause][ToIndex(ContestSide::SideA)] =
        ColorPair(Qt::lightGray, m_TextBgColorFirst);
    hold_clock_colors[eHoldState_pause][ToIndex(ContestSide::SideB)] =
        ColorPair(Qt::darkGray, m_TextBgColorSecond);

    ScaledText* pClocks[ToIndex(ContestSide::Count)] = { ui->text_hold_clock_first,
                                                         ui->text_hold_clock_second };

    // reset drawing first
    pClocks[ToIndex(sideA)]->SetColor(
        hold_clock_colors[eHoldState_off][ToIndex(ContestSide::SideA)].fg,
        hold_clock_colors[eHoldState_off][ToIndex(ContestSide::SideA)].bg);

    pClocks[ToIndex(sideB)]->SetColor(
        hold_clock_colors[eHoldState_off][ToIndex(ContestSide::SideB)].fg,
        hold_clock_colors[eHoldState_off][ToIndex(ContestSide::SideB)].bg);

    pClocks[ToIndex(sideA)]->SetText("00", ScaledText::eSize_full);
    pClocks[ToIndex(sideB)]->SetText("00", ScaledText::eSize_full);
    ui->image_sand_clock->SetBgColor(Qt::black);

    // no one holding?
    if (ContestSide::None == holder)
    {
        if (is_secondary())
        {
            pClocks[ToIndex(ContestSide::SideA)]->SetText("", ScaledText::eSize_full);
            pClocks[ToIndex(ContestSide::SideB)]->SetText("", ScaledText::eSize_full);
            ui->image_sand_clock->UpdateImage(":res/images/off_empty.png");
        }

        return;
    }

    // set spectial hold states
    if (eHoldState_on == state)
    {
        const auto mappedHolder = MapDisplayPositionToContestSide(holder);
        pClocks[ToIndex(mappedHolder)]->SetColor(
            hold_clock_colors[eHoldState_on][ToIndex(holder)].fg,
            hold_clock_colors[eHoldState_on][ToIndex(holder)].bg);

        pClocks[ToIndex(mappedHolder)]->SetText(value, ScaledText::eSize_full);

        ui->image_sand_clock->SetBgColor(hold_clock_colors[eHoldState_on][ToIndex(holder)].bg);
    }
    else if (eHoldState_pause == state)
    {
        const auto mappedHolder = MapDisplayPositionToContestSide(holder);
        pClocks[ToIndex(mappedHolder)]->SetColor(
            hold_clock_colors[eHoldState_pause][ToIndex(holder)].fg,
            hold_clock_colors[eHoldState_pause][ToIndex(holder)].bg);

        pClocks[ToIndex(mappedHolder)]->SetText(value, ScaledText::eSize_full);

        ui->image_sand_clock->SetBgColor(hold_clock_colors[eHoldState_on][ToIndex(holder)].bg);
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

        if (ContestSide::SideA == holder)
        {
            ui->layout_osaekomi->setStretchFactor(ui->text_hold_clock_first, 7);
            ui->layout_osaekomi->setStretchFactor(ui->text_hold_clock_second, 0);
        }
        else if (ContestSide::SideB == holder)
        {
            ui->layout_osaekomi->setStretchFactor(ui->text_hold_clock_first, 0);
            ui->layout_osaekomi->setStretchFactor(ui->text_hold_clock_second, 7);
        }
        else
        {
            Q_ASSERT(false);
        }
    }
}

//=========================================================
Ipponboard::ContestSide View::MapDisplayPositionToContestSide(
    const Ipponboard::ContestSide side) const
//=========================================================
{
    if (!IsContestSide(side))
    {
        return side;
    }

    if (!is_secondary())
    {
        return OpposingSide(side);
    }

    return side;
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
        Q_ASSERT(!"unhandled switch case!");
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
    ui->text_ippon_desc1->SetColor(get_color(firstFg), get_color(firstBg));
    ui->text_wazaari_desc1->SetColor(get_color(firstFg), get_color(firstBg));
    ui->text_yuko_desc1->SetColor(get_color(firstFg), get_color(firstBg));
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
    ui->text_ippon_desc2->SetColor(get_color(secondFg), get_color(secondBg));
    ui->text_wazaari_desc2->SetColor(get_color(secondFg), get_color(secondBg));
    ui->text_yuko_desc2->SetColor(get_color(secondFg), get_color(secondBg));
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
