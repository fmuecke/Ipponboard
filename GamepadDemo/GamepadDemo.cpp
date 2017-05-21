#include "GamepadDemo.h"
#include "ui_gamepaddemo.h"
#include <QTimer>

#include "../gamepad/gamepad.h"
#include "../base/versioninfo.h"

using namespace FMlib;

GamepadDemo::GamepadDemo(QWidget* parent)
	: QMainWindow(parent)
	, ui(new Ui::GamepadDemo)
	, m_pSBarText(0)
	, m_pTimer(0)
	, m_pGamepad(new Gamepad)
{
	ui->setupUi(this);
	m_pSBarText = new QLabel;
	m_pSBarText->setText("-");
	ui->statusBar->addWidget(m_pSBarText, 1);
	ui->label_copyRight->setText(QString("© 2010-%1 Florian Mücke").arg(VersionInfo::CopyrightYear));

	ui->image_button_1->UpdateImage(":images/off.png");
	ui->image_button_2->UpdateImage(":images/off.png");
	ui->image_button_3->UpdateImage(":images/off.png");
	ui->image_button_4->UpdateImage(":images/off.png");
	ui->image_button_5->UpdateImage(":images/off.png");
	ui->image_button_6->UpdateImage(":images/off.png");
	ui->image_button_7->UpdateImage(":images/off.png");
	ui->image_button_8->UpdateImage(":images/off.png");
	ui->image_button_9->UpdateImage(":images/off.png");
	ui->image_button_10->UpdateImage(":images/off.png");
	ui->image_button_11->UpdateImage(":images/off.png");
	ui->image_button_12->UpdateImage(":images/off.png");
	QTimer* m_pTimer = new QTimer;
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(GetData()));

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
	delete ui;
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

	ui->lineEdit_degrees_1->setText(
		QString::number(m_pGamepad->GetAngleXY(), 'g', 3) + "°");

	ui->lineEdit_degrees_2->setText(
		QString::number(m_pGamepad->GetAngleRZ(), 'g', 3) + "°");

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
	ScaledImage* pImage(0);

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
		if (m_pGamepad->IsPressed(Gamepad::EButton(button)))
			pImage->UpdateImage(":images/on.png");
		else
			pImage->UpdateImage(":images/off.png");
	}

}
