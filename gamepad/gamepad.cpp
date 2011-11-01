///////////////////////////////////////////////////////////////////////////
// Gamepad wrapper class
// (c) 2010 Florian Muecke <dev_AT_mueckeimnetz_DOT_de>
// $Id$
///////////////////////////////////////////////////////////////////////////
#include "gamepad.h"
#include "math.h"
#include "assert.h"

//#pragma comment(lib,"Winmm.lib")

using namespace FMlib;

const unsigned int Gamepad::button_code[] =
{
	JOY_BUTTON1,
	JOY_BUTTON2,
	JOY_BUTTON3,
	JOY_BUTTON4,
	JOY_BUTTON5,
	JOY_BUTTON6,
	JOY_BUTTON7,
	JOY_BUTTON8,
	JOY_BUTTON9,
	JOY_BUTTON10,
	JOY_BUTTON11,
	JOY_BUTTON12,
	JOY_BUTTON13,
	JOY_BUTTON14,
	JOY_BUTTON15,
	JOY_BUTTON16,
	JOY_BUTTON17,
	JOY_BUTTON18,
	JOY_BUTTON19,
	JOY_BUTTON20,
	JOY_BUTTON21,
	JOY_BUTTON22,
	JOY_BUTTON23,
	JOY_BUTTON24,
	JOY_BUTTON25,
	JOY_BUTTON26,
	JOY_BUTTON27,
	JOY_BUTTON28,
	JOY_BUTTON29,
	JOY_BUTTON30,
	JOY_BUTTON31,
	JOY_BUTTON32,
	0,		// eButton_pov_fwd
	9000,	// eButton_pov_right
	18000,	// eButton_pov_back
	27000,	// eButton_pov_left
	4500,	// ePov_right_fwd
	13500,	// eButton_pov_right_back
	22500,	// eButton_pov_left_back
	31500	// eButton_pov_left_fwd
};


//---------------------------------------------------------
Gamepad::Gamepad()
	: m_currentId(0)
	, m_invertedAxes()
	//, m_caps
	//, m_data
	, m_state(eState_unknown)
	, m_hwnd(NULL)
//---------------------------------------------------------
{
	reset();
	Init();
}

//---------------------------------------------------------
Gamepad::~Gamepad()
//---------------------------------------------------------
{
	if (m_hwnd)
		Release();
}

//---------------------------------------------------------
Gamepad::EState Gamepad::Init()
//---------------------------------------------------------
{
	m_state = EState(::joyGetDevCaps(0, &m_caps, sizeof(JOYCAPS)));
	return m_state;
}

//---------------------------------------------------------
void Gamepad::SetInverted(EAxis axis, bool val)
//---------------------------------------------------------
{
	m_invertedAxes.set(axis, val);
}

//---------------------------------------------------------
bool Gamepad::IsInverted(EAxis axis) const
//---------------------------------------------------------
{
	return m_invertedAxes.test(axis);
}

//---------------------------------------------------------
bool Gamepad::WasPressed(EButton b) const
//---------------------------------------------------------
{
	if (b > eButton32)
	{
		if (button_code[b] == m_data.dwPOV &&
				m_data.dwPOV != m_lastData.dwPOV)
			return true;

		return false;
	}

	return (m_lastData.dwButtons & button_code[b]) == 0 &&
		   (m_data.dwButtons & button_code[b]) > 0;
}

//---------------------------------------------------------
bool Gamepad::WasReleased(EButton b) const
//---------------------------------------------------------
{
	assert(b <= eButton32);

	return (m_lastData.dwButtons & button_code[b]) > 0 &&
		   (m_data.dwButtons & button_code[b]) == 0;
}

//---------------------------------------------------------
bool Gamepad::IsPressed(EButton b) const
//---------------------------------------------------------
{
	assert(b <= eButton32);

	return (m_data.dwButtons & button_code[b]) > 0;
}

//---------------------------------------------------------
bool Gamepad::WasSectionEnteredXY(float min, float max) const
//---------------------------------------------------------
{
	const int lastX = m_invertedAxes.test(eAxis_X) ?
					  eMax - m_lastData.dwXpos : m_lastData.dwXpos;
	const int lastY = m_invertedAxes.test(eAxis_Y) ?
					  eMax - m_lastData.dwYpos : m_lastData.dwYpos;

	int curX = m_invertedAxes.test(eAxis_X) ?
			   eMax - m_data.dwXpos : m_data.dwXpos;
	int curY = m_invertedAxes.test(eAxis_Y) ?
			   eMax - m_data.dwYpos : m_data.dwYpos;

	const float lastAlpha = GetAngle<float>(
								lastX - eMid, lastY - eMid, 1.02f, eMid);

	const float curAlpha = GetAngle<float>(
							   curX - eMid, curY - eMid, 1.02f, eMid);

	bool changed(false);

	if (-1.0f == lastAlpha || -1.0f == curAlpha ||
			fabs(lastAlpha - curAlpha) > eTolerance)
		changed = true;

	if (changed && IsInSection(curAlpha, min, max))
		return true;

	return false;
}

//---------------------------------------------------------
bool Gamepad::WasSectionEnteredRZ(float min, float max) const
//---------------------------------------------------------
{
	const int lastR = m_invertedAxes.test(eAxis_R) ?
					  eMax - m_lastData.dwRpos : m_lastData.dwRpos;
	const int lastZ = m_invertedAxes.test(eAxis_Z) ?
					  eMax - m_lastData.dwZpos : m_lastData.dwZpos;

	int curR = m_invertedAxes.test(eAxis_R) ?
			   eMax - m_data.dwRpos : m_data.dwRpos;
	int curZ = m_invertedAxes.test(eAxis_Z) ?
			   eMax - m_data.dwZpos : m_data.dwZpos;

	const float lastAlpha = GetAngle<float>(
								lastR - eMid, lastZ - eMid, 1.02f, eMid);

	const float curAlpha = GetAngle<float>(
							   curR - eMid, curZ - eMid, 1.02f, eMid);

	bool changed(false);

	if (-1.0f == lastAlpha || -1.0f == curAlpha ||
			fabs(lastAlpha - curAlpha) > eTolerance)
		changed = true;

	if (changed && IsInSection(curAlpha, min, max))
		return true;

	return false;
}


//---------------------------------------------------------
bool Gamepad::WasSectionLeft(float min, float max) const
//---------------------------------------------------------
{
	return false;
}

//---------------------------------------------------------
bool Gamepad::IsInSection(
	const float alpha, const float min, const float max) const
//---------------------------------------------------------
{
	bool ret(false);

	if (min > max)
	{
		if (min <= alpha && alpha <= 360.0f)
			ret = true;
		else if (0 < alpha && alpha <= max)
			ret = true;
	}
	else
	{
		if (min <= alpha && max >= alpha)
			ret = true;
	}

	return ret;
}

template<class T>
T Gamepad::GetAngle(int x, int y, float factor, int max) const
{
	if (0 == x && 0 == y)
		return -1;

	T h = sqrt((T)x * x + (T)y * y);

	if (h * factor < (float)max)
		return -1;

	T angle = asin((T)y / h) * 180 / (T)3.141592653589793L;

	if (x > 0)
		angle = 90 - angle;
	else
		angle = 270 + angle;

	return angle;
}

float Gamepad::GetAngleXY() const
{
	return GetAngle<float>(GetXPos() - eMid, GetYPos() - eMid, 1.2f, eMid);
}
float Gamepad::GetAngleRZ() const
{
	return GetAngle<float>(GetRPos() - eMid, GetZPos() - eMid, 1.2f, eMid);
}


//---------------------------------------------------------
int Gamepad::PressedCount() const
//---------------------------------------------------------
{
	return m_data.dwButtonNumber;
}

//---------------------------------------------------------
unsigned Gamepad::GetXPos() const
//---------------------------------------------------------
{
	if (m_invertedAxes.test(eAxis_X))
		return 65535 - m_data.dwXpos;

	return m_data.dwXpos;
}

//---------------------------------------------------------
unsigned Gamepad::GetYPos() const
//---------------------------------------------------------
{
	if (m_invertedAxes.test(eAxis_Y))
		return 65535 - m_data.dwYpos;

	return m_data.dwYpos;
}

//---------------------------------------------------------
unsigned Gamepad::GetZPos() const
//---------------------------------------------------------
{
	if (m_invertedAxes.test(eAxis_Z))
		return 65535 - m_data.dwZpos;

	return m_data.dwZpos;
}

//---------------------------------------------------------
unsigned Gamepad::GetRPos() const
//---------------------------------------------------------
{
	if (m_invertedAxes.test(eAxis_R))
		return 65535 - m_data.dwRpos;

	return m_data.dwRpos;
}
//---------------------------------------------------------
unsigned Gamepad::GetUPos() const
//---------------------------------------------------------
{
	if (m_invertedAxes.test(eAxis_U))
		return 65535 - m_data.dwUpos;

	return m_data.dwUpos;
}

//---------------------------------------------------------
unsigned Gamepad::GetVPos() const
//---------------------------------------------------------
{
	if (m_invertedAxes.test(eAxis_V))
		return 65535 - m_data.dwVpos;

	return m_data.dwVpos;
}

//---------------------------------------------------------
unsigned Gamepad::GetPOV() const
//---------------------------------------------------------
{
	return m_data.dwPOV;
}

//---------------------------------------------------------
Gamepad::EState Gamepad::ReadData()
//---------------------------------------------------------
{
	m_lastData = m_data;
	m_state = EState(::joyGetPosEx(m_currentId, &m_data));
	return m_state;
}

//---------------------------------------------------------
Gamepad::EState Gamepad::GetState() const
//---------------------------------------------------------
{
	return m_state;
}

//---------------------------------------------------------
unsigned Gamepad::GetThreshold() const
//---------------------------------------------------------
{
	unsigned t(0);

	if (eState_ok != ::joyGetThreshold(m_currentId, &t))
		return static_cast<unsigned>(-1);

	return t;
}

//---------------------------------------------------------
bool Gamepad::SetThreshold(unsigned threshold) const
//---------------------------------------------------------
{
	return eState_ok == ::joySetThreshold(m_currentId, threshold);
}

//---------------------------------------------------------
bool Gamepad::Capture(HWND hwnd, unsigned int period, EUpdateAction when)
//---------------------------------------------------------
{
	bool ret(false);

	if (NULL == m_hwnd)
	{
		int s(::joySetCapture(hwnd, m_currentId, period,
							  when ? TRUE : FALSE));

		if (eState_ok == s)
		{
			m_hwnd = hwnd;
			ret = true;
		}
	}

	return ret;
}

//---------------------------------------------------------
bool Gamepad::Release()
//---------------------------------------------------------
{
	bool ret(false);

	if (NULL != m_hwnd)
	{
		int s(::joyReleaseCapture(m_currentId));

		if (eState_ok == s)
		{
			m_hwnd = NULL;
			ret = true;
		}
	}

	return ret;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void Gamepad::reset()
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
{
	m_invertedAxes.reset();

	memset(&m_caps, 0, sizeof(JOYCAPS));
	memset(&m_data, 0, sizeof(JOYINFOEX));
	m_data.dwSize = sizeof(JOYINFOEX);
	m_data.dwFlags = JOY_RETURNALL;
	m_lastData = m_data;
}

