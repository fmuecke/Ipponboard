// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "GamepadDemo.h"

#include "../base/versioninfo.h"
#include "../gamepad/Gamepad.h"
#include "../gamepad/GamepadImpl.h"
#include "ui_GamepadDemo.h"

#include <QDebug>
#include <QStringList>
#include <QTimer>
#include <array>

namespace
{
using ButtonInfo = std::pair<FMlib::Gamepad::EButton, const char*>;

bool isGamepadDebugEnabled()
{
    static const bool enabled = qEnvironmentVariableIsSet("IPPONBOARD_DEBUG_GAMEPAD");
    return enabled;
}

void logGamepadDiagnostics(const FMlib::Gamepad& gamepad)
{
    if (!isGamepadDebugEnabled())
    {
        return;
    }

    static const std::array<ButtonInfo, 20> kButtons = {
        { { FMlib::Gamepad::eButton1, "Btn1" },
          { FMlib::Gamepad::eButton2, "Btn2" },
          { FMlib::Gamepad::eButton3, "Btn3" },
          { FMlib::Gamepad::eButton4, "Btn4" },
          { FMlib::Gamepad::eButton5, "Btn5" },
          { FMlib::Gamepad::eButton6, "Btn6" },
          { FMlib::Gamepad::eButton7, "Btn7" },
          { FMlib::Gamepad::eButton8, "Btn8" },
          { FMlib::Gamepad::eButton9, "Select" },
          { FMlib::Gamepad::eButton10, "Start" },
          { FMlib::Gamepad::eButton11, "L3" },
          { FMlib::Gamepad::eButton12, "R3" },
          { FMlib::Gamepad::eButton13, "DPadUp" },
          { FMlib::Gamepad::eButton14, "DPadRight" },
          { FMlib::Gamepad::eButton15, "DPadDown" },
          { FMlib::Gamepad::eButton16, "DPadLeft" },
          { FMlib::Gamepad::eButton17, "Center" },
          { FMlib::Gamepad::eButton18, "Guide" },
          { FMlib::Gamepad::eButton_pov_fwd, "POV_Up" },
          { FMlib::Gamepad::eButton_pov_back, "POV_Down" } }
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

using namespace FMlib;

constexpr const char* kOffImage = ":/images/off.png";
constexpr const char* kOnImage = ":/images/on.png";

GamepadDemo::GamepadDemo(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::GamepadDemo()),
      m_pSBarText(nullptr),
      m_pTimer(nullptr),
      m_pGamepad(new Gamepad(std::make_unique<GamepadImpl>()))
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

    if (Gamepad::eState_ok != m_pGamepad->GetState())
    {
        m_pSBarText->setText("no controller found");
        ui->groupBox_caps->setEnabled(false);
        ui->groupBox_testing->setEnabled(false);
        return;
    }

    m_pGamepad->SetInverted(Gamepad::eAxis_Y);
    m_pGamepad->SetInverted(Gamepad::eAxis_Z);

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
    if (Gamepad::eState_ok != m_pGamepad->GetState())
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
    Gamepad::EPovType povType(m_pGamepad->GetPovType());

    if (Gamepad::ePovType_no_pov != povType)
    {
        if (Gamepad::ePovType_discrete == povType)
            vals.append("discrete");
        else if (Gamepad::ePovType_continuous == povType)
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

    if (Gamepad::eState_ok != m_pGamepad->GetState())
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

    UpdateButtonState(Gamepad::eButton1);
    UpdateButtonState(Gamepad::eButton2);
    UpdateButtonState(Gamepad::eButton3);
    UpdateButtonState(Gamepad::eButton4);
    UpdateButtonState(Gamepad::eButton5);
    UpdateButtonState(Gamepad::eButton6);
    UpdateButtonState(Gamepad::eButton7);
    UpdateButtonState(Gamepad::eButton8);
    UpdateButtonState(Gamepad::eButton9);
    UpdateButtonState(Gamepad::eButton10);
    UpdateButtonState(Gamepad::eButton11);
    UpdateButtonState(Gamepad::eButton12);

    //cout << "buttons: " << joyInfo.wButtons << "\n" << endl;
}

void GamepadDemo::UpdateButtonState(unsigned button) const
{
    ScaledImage* pImage = nullptr;

    switch (button)
    {
    case Gamepad::eButton1:
        pImage = ui->image_button_1;
        break;

    case Gamepad::eButton2:
        pImage = ui->image_button_2;
        break;

    case Gamepad::eButton3:
        pImage = ui->image_button_3;
        break;

    case Gamepad::eButton4:
        pImage = ui->image_button_4;
        break;

    case Gamepad::eButton5:
        pImage = ui->image_button_5;
        break;

    case Gamepad::eButton6:
        pImage = ui->image_button_6;
        break;

    case Gamepad::eButton7:
        pImage = ui->image_button_7;
        break;

    case Gamepad::eButton8:
        pImage = ui->image_button_8;
        break;

    case Gamepad::eButton9:
        pImage = ui->image_button_9;
        break;

    case Gamepad::eButton10:
        pImage = ui->image_button_10;
        break;

    case Gamepad::eButton11:
        pImage = ui->image_button_11;
        break;

    case Gamepad::eButton12:
        pImage = ui->image_button_12;
        break;

    default:
        break;
    }

    if (pImage)
    {
        auto updateImage = m_pGamepad->IsPressed(Gamepad::EButton(button)) ? kOnImage : kOffImage;

        pImage->UpdateImage(updateImage);
    }
}
