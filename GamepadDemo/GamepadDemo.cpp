// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "GamepadDemo.h"

#include "../base/versioninfo.h"
#include "../gamepad/Gamepad.h"
#include "ui_GamepadDemo.h"

#include <QDebug>
#include <QStringList>
#include <QTimer>
#include <array>

using namespace GamepadLib;

namespace
{
using ButtonInfo = std::pair<EButton, const char*>;

bool isGamepadDebugEnabled()
{
    static const bool enabled = qEnvironmentVariableIsSet("IPPONBOARD_DEBUG_GAMEPAD");
    return enabled;
}

void logGamepadDiagnostics(const GamepadLib::Gamepad& gamepad)
{
    if (!isGamepadDebugEnabled())
    {
        return;
    }

    static const std::array<ButtonInfo, 20> kButtons = {
        { { EButton::button1, "Btn1" },          { EButton::button2, "Btn2" },
          { EButton::button3, "Btn3" },          { EButton::button4, "Btn4" },
          { EButton::button5, "Btn5" },          { EButton::button6, "Btn6" },
          { EButton::button7, "Btn7" },          { EButton::button8, "Btn8" },
          { EButton::button9, "Select" },        { EButton::button10, "Start" },
          { EButton::button11, "L3" },           { EButton::button12, "R3" },
          { EButton::button13, "DPadUp" },       { EButton::button14, "DPadRight" },
          { EButton::button15, "DPadDown" },     { EButton::button16, "DPadLeft" },
          { EButton::button17, "Center" },       { EButton::button18, "Guide" },
          { EButton::button_pov_fwd, "POV_Up" }, { EButton::button_pov_back, "POV_Down" } }
    };

    static std::array<bool, kButtons.size()> s_lastButtons{};
    static bool s_initialised = false;

    QStringList buttonChanges;
    for (std::size_t i = 0; i < kButtons.size(); ++i)
    {
        const bool pressed = gamepad.IsPressed(kButtons[i].first);
        if (!s_initialised || pressed != s_lastButtons[i])
        {
            buttonChanges.push_back(QStringLiteral("%1=%2")
                                        .arg(QString::fromLatin1(kButtons[i].second))
                                        .arg(pressed ? QStringLiteral("1") : QStringLiteral("0")));
            s_lastButtons[i] = pressed;
        }
    }

    const auto axes =
        std::array<unsigned, 6>{ gamepad.GetXPos(), gamepad.GetYPos(), gamepad.GetZPos(),
                                 gamepad.GetRPos(), gamepad.GetUPos(), gamepad.GetVPos() };
    static std::array<unsigned, 6> s_lastAxes{ axes };
    QStringList axisChanges;
    for (std::size_t i = 0; i < axes.size(); ++i)
    {
        if (!s_initialised || s_lastAxes[i] != axes[i])
        {
            axisChanges.push_back(QStringLiteral("Axis%1=%2")
                                      .arg(static_cast<int>(i))
                                      .arg(static_cast<int>(axes[i])));
            s_lastAxes[i] = axes[i];
        }
    }

    const unsigned pov = gamepad.GetPOV();
    static unsigned s_lastPov = pov;
    if (!s_initialised || s_lastPov != pov)
    {
        qInfo().nospace() << "Gamepad POV=" << pov;
        s_lastPov = pov;
    }

    if (!buttonChanges.isEmpty())
    {
        qInfo().noquote() << "Gamepad buttons:" << buttonChanges.join(QLatin1String(", "));
    }

    if (!axisChanges.isEmpty())
    {
        qInfo().noquote() << "Gamepad axes:" << axisChanges.join(QLatin1String(", "));
    }

    s_initialised = true;
}
} // namespace

constexpr const char* kOffImage = ":/images/off.png";
constexpr const char* kOnImage = ":/images/on.png";

GamepadDemo::GamepadDemo(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::GamepadDemo()),
      m_pSBarText(nullptr),
      m_pTimer(nullptr),
      m_pGamepad(new Gamepad())
{
    ui->setupUi(this);
    m_pSBarText = new QLabel;
    m_pSBarText->setText("-");
    ui->statusBar->addWidget(m_pSBarText, 1);
    ui->label_copyRight->setText(
        QString::fromUtf8("© 2010-%1 Florian Mücke").arg(VersionInfo::CopyrightYear));

    ui->image_button_1->UpdateImage(kOffImage);
    ui->image_button_2->UpdateImage(kOffImage);
    ui->image_button_3->UpdateImage(kOffImage);
    ui->image_button_4->UpdateImage(kOffImage);
    ui->image_button_5->UpdateImage(kOffImage);
    ui->image_button_6->UpdateImage(kOffImage);
    ui->image_button_7->UpdateImage(kOffImage);
    ui->image_button_8->UpdateImage(kOffImage);
    ui->image_button_9->UpdateImage(kOffImage);
    ui->image_button_10->UpdateImage(kOffImage);
    ui->image_button_11->UpdateImage(kOffImage);
    ui->image_button_12->UpdateImage(kOffImage);
    m_pTimer = new QTimer(this);
    connect(m_pTimer, &QTimer::timeout, this, &GamepadDemo::GetData);

    if (GamepadLib::EGamepadState::ok != m_pGamepad->GetState())
    {
        m_pSBarText->setText("no controller found");
        ui->groupBox_caps->setEnabled(false);
        ui->groupBox_testing->setEnabled(false);
        return;
    }

    m_pGamepad->SetInverted(GamepadLib::EAxis::Y);
    m_pGamepad->SetInverted(GamepadLib::EAxis::Z);

    UpdateCapabilities();

    m_pTimer->start(50);
}

GamepadDemo::~GamepadDemo()
{
    // clean up member objects
    // ui --> unique_ptr
    // m_pGamepad --> unique_ptr
    // m_pTime --> killed via object hierarchy
    // m_pSBarText --> killed via object hierarchy
}

void GamepadDemo::changeEvent(QEvent* e)
{
    QMainWindow::changeEvent(e);

    switch (e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;

    default:
        break;
    }
}

void GamepadDemo::UpdateCapabilities()
{
    if (EGamepadState::ok != m_pGamepad->GetState())
    {
        m_pSBarText->setText("unable to retrieve gamepad capabilities");
    }

    ui->treeWidget->clear();
    QStringList vals;

    vals.clear();
    vals.append("Product name");
    vals.append(QString::fromWCharArray(m_pGamepad->GetProductName()));
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));

    vals.clear();
    vals.append("Manufacturer id");
    vals.append(QString::number(m_pGamepad->GetMId()));
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));

    vals.clear();
    vals.append("Product id");
    vals.append(QString::number(m_pGamepad->GetPId()));
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));

    vals.clear();
    vals.append("Number of buttons");
    vals.append(QString::number(m_pGamepad->GetNumButtons()));
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));

    vals.clear();
    vals.append("Number of axes");
    vals.append(QString::number(m_pGamepad->GetNumAxes()));
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));

    vals.clear();
    vals.append("Polling frequency");
    vals.append(QString::number(m_pGamepad->GetPollingFreq().first) + ".." +
                QString::number(m_pGamepad->GetPollingFreq().second));
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));

    vals.clear();
    vals.append("Range of X axis");
    vals.append(QString::number(m_pGamepad->GetRangeX().first) + ".." +
                QString::number(m_pGamepad->GetRangeX().second));
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));

    vals.clear();
    vals.append("Range of Y axis");
    vals.append(QString::number(m_pGamepad->GetRangeY().first) + ".." +
                QString::number(m_pGamepad->GetRangeY().second));
    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));

    vals.clear();
    vals.append("Range of Z axis");

    if (m_pGamepad->HasAxisZ())
    {
        vals.append(QString::number(m_pGamepad->GetRangeZ().first) + ".." +
                    QString::number(m_pGamepad->GetRangeZ().second));
    }
    else
    {
        vals.append("not supported");
    }

    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));

    vals.clear();
    vals.append("Range of rudder axis");

    if (m_pGamepad->HasAxisR())
    {
        vals.append(QString::number(m_pGamepad->GetRangeR().first) + ".." +
                    QString::number(m_pGamepad->GetRangeR().second));
    }
    else
    {
        vals.append("not supported");
    }

    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));

    vals.clear();
    vals.append("Range of U axis");

    if (m_pGamepad->HasAxisU())
    {
        vals.append(QString::number(m_pGamepad->GetRangeU().first) + ".." +
                    QString::number(m_pGamepad->GetRangeU().second));
    }
    else
    {
        vals.append("not supported");
    }

    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));

    vals.clear();
    vals.append("Range of V axis");

    if (m_pGamepad->HasAxisV())
    {
        vals.append(QString::number(m_pGamepad->GetRangeV().first) + ".." +
                    QString::number(m_pGamepad->GetRangeV().second));
    }
    else
    {
        vals.append("not supported");
    }

    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));

    vals.clear();
    vals.append("Point-of-view");
    EPovType povType(m_pGamepad->GetPovType());

    if (EPovType::no_pov != povType)
    {
        if (EPovType::discrete == povType)
            vals.append("discrete");
        else if (EPovType::continuous == povType)
            vals.append("continuous");
        else
            vals.append("unknown");
    }
    else
    {
        vals.append("not supported");
    }

    ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(vals));
}

void GamepadDemo::GetData()
{
    m_pGamepad->ReadData();
    logGamepadDiagnostics(*m_pGamepad);

    if (EGamepadState::ok != m_pGamepad->GetState())
        return;

    m_pSBarText->setText("controller found");

    ui->horizontalSlider_z->setEnabled(m_pGamepad->HasAxisZ());
    ui->horizontalSlider_r->setEnabled(m_pGamepad->HasAxisR());
    ui->horizontalSlider_u->setEnabled(m_pGamepad->HasAxisU());
    ui->horizontalSlider_v->setEnabled(m_pGamepad->HasAxisV());
    ui->label_axis_z->setEnabled(m_pGamepad->HasAxisZ());
    ui->label_axis_r->setEnabled(m_pGamepad->HasAxisR());
    ui->label_axis_u->setEnabled(m_pGamepad->HasAxisU());
    ui->label_axis_v->setEnabled(m_pGamepad->HasAxisV());

    ui->horizontalSlider_x->setValue(m_pGamepad->GetXPos());
    ui->horizontalSlider_y->setValue(m_pGamepad->GetYPos());
    ui->horizontalSlider_z->setValue(m_pGamepad->GetZPos());
    ui->horizontalSlider_r->setValue(m_pGamepad->GetRPos());
    ui->horizontalSlider_u->setValue(m_pGamepad->GetUPos());
    ui->horizontalSlider_v->setValue(m_pGamepad->GetVPos());

    ui->lineEdit_x_axis->setText(QString::number(m_pGamepad->GetXPos()));
    ui->lineEdit_y_axis->setText(QString::number(m_pGamepad->GetYPos()));
    ui->lineEdit_z_axis->setText(QString::number(m_pGamepad->GetZPos()));
    ui->lineEdit_r_axis->setText(QString::number(m_pGamepad->GetRPos()));
    ui->lineEdit_u_axis->setText(QString::number(m_pGamepad->GetUPos()));
    ui->lineEdit_v_axis->setText(QString::number(m_pGamepad->GetVPos()));
    ui->lineEdit_pov->setText(QString::number(m_pGamepad->GetPOV()));

    ui->lineEdit_degrees_1->setText(QString::number(m_pGamepad->GetAngleXY(), 'g', 3) +
                                    QString::fromUtf8("°"));
    ui->lineEdit_degrees_2->setText(QString::number(m_pGamepad->GetAngleRZ(), 'g', 3) +
                                    QString::fromUtf8("°"));

    UpdateButtonState(EButton::button1);
    UpdateButtonState(EButton::button2);
    UpdateButtonState(EButton::button3);
    UpdateButtonState(EButton::button4);
    UpdateButtonState(EButton::button5);
    UpdateButtonState(EButton::button6);
    UpdateButtonState(EButton::button7);
    UpdateButtonState(EButton::button8);
    UpdateButtonState(EButton::button9);
    UpdateButtonState(EButton::button10);
    UpdateButtonState(EButton::button11);
    UpdateButtonState(EButton::button12);

    //cout << "buttons: " << joyInfo.wButtons << "\n" << endl;
}

void GamepadDemo::UpdateButtonState(unsigned button) const
{
    ScaledImage* pImage = nullptr;

    switch (button)
    {
    case EButton::button1:
        pImage = ui->image_button_1;
        break;

    case EButton::button2:
        pImage = ui->image_button_2;
        break;

    case EButton::button3:
        pImage = ui->image_button_3;
        break;

    case EButton::button4:
        pImage = ui->image_button_4;
        break;

    case EButton::button5:
        pImage = ui->image_button_5;
        break;

    case EButton::button6:
        pImage = ui->image_button_6;
        break;

    case EButton::button7:
        pImage = ui->image_button_7;
        break;

    case EButton::button8:
        pImage = ui->image_button_8;
        break;

    case EButton::button9:
        pImage = ui->image_button_9;
        break;

    case EButton::button10:
        pImage = ui->image_button_10;
        break;

    case EButton::button11:
        pImage = ui->image_button_11;
        break;

    case EButton::button12:
        pImage = ui->image_button_12;
        break;

    default:
        break;
    }

    if (pImage)
    {
        auto updateImage = m_pGamepad->IsPressed(EButton(button)) ? kOnImage : kOffImage;

        pImage->UpdateImage(updateImage);
    }
}
