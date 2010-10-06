#ifndef GAME_PAD_HEADER_INCLUDED
#define GAME_PAD_HEADER_INCLUDED

// Gamepad wrapper class
// Copyright 2010 by Florian Muecke <dev_AT_mueckeimnetz_DOT_de>
//
// $Id:

#include <bitset>
#include <boost/utility.hpp>

#include <Windows.h>
#include <Mmsystem.h>

namespace FMlib {

typedef std::pair<unsigned, unsigned> unsignedPair;

class Gamepad : public boost::noncopyable
{
public:
	enum EButton
	{
		eButton1 = JOY_BUTTON1,
		eButton2 = JOY_BUTTON2,
		eButton3 = JOY_BUTTON3,
		eButton4 = JOY_BUTTON4,
		eButton5 = JOY_BUTTON5,
		eButton6 = JOY_BUTTON6,
		eButton7 = JOY_BUTTON7,
		eButton8 = JOY_BUTTON8,
		eButton9 = JOY_BUTTON9,
		eButton10 = JOY_BUTTON10,
		eButton11 = JOY_BUTTON11,
		eButton12 = JOY_BUTTON12,
		eButton13 = JOY_BUTTON13,
		eButton14 = JOY_BUTTON14,
		eButton15 = JOY_BUTTON15,
		eButton16 = JOY_BUTTON16,
		eButton17 = JOY_BUTTON17,
		eButton18 = JOY_BUTTON18,
		eButton19 = JOY_BUTTON19,
		eButton20 = JOY_BUTTON20,
		eButton21 = JOY_BUTTON21,
		eButton22 = JOY_BUTTON22,
		eButton23 = JOY_BUTTON23,
		eButton24 = JOY_BUTTON24,
		eButton25 = JOY_BUTTON25,
		eButton26 = JOY_BUTTON26,
		eButton27 = JOY_BUTTON27,
		eButton28 = JOY_BUTTON28,
		eButton29 = JOY_BUTTON29,
		eButton30 = JOY_BUTTON30,
		eButton31 = JOY_BUTTON31,
		eButton32 = JOY_BUTTON32
	};

	enum EState
	{
		eState_unknown			= -1,
		eState_ok				= JOYERR_NOERROR,
		eState_no_driver		= MMSYSERR_NODRIVER,
		eState_invalid_param	= MMSYSERR_INVALPARAM,
		eState_bad_device_id	= MMSYSERR_BADDEVICEID,
		eState_no_service		= JOYERR_NOCANDO,
		eState_unplugged		= JOYERR_UNPLUGGED,
		eState_invalid_id		= JOYERR_PARMS
	};

	enum EUpdateAction
	{
		update_always = false,
		update_on_change = true
	};

	enum EPovType
	{
		ePovType_unknown = -1,
		ePovType_no_pov = 0,
		ePovType_discrete = JOYCAPS_POV4DIR,
		ePovType_continuous = JOYCAPS_POVCTS
	};

	enum EPOV
	{
		ePov_up = 0,
		ePov_right = 9000,
		ePov_down = 18000,
		ePov_left = 27000,
		ePov_upper_right = 4500,
		ePov_lower_right = 13500,
		ePov_lower_left = 22500,
		ePov_upper_left = 31500
	};

	enum EAxis
	{
		eAxis_X = 0,
		eAxis_Y,
		eAxis_Z,
		eAxis_R,
		eAxis_U,
		eAxis_V,
		eAxis_MAX
	};

	enum { eMax = 65535, eMid = 32767, eTolerance = 100 };
	Gamepad();
	virtual ~Gamepad();

	/// reads capabilities
	EState Init();

	inline const TCHAR* GetProductName() const
	{
		return m_caps.szPname;
	}
	inline WORD GetMId() const
	{
		return m_caps.wMid;
	}
	inline WORD GetPId() const
	{
		return m_caps.wPid;
	}
	inline unsigned GetNumButtons() const
	{
		return m_caps.wNumButtons;
	}
	inline unsigned GetNumAxes() const
	{
		return m_caps.wNumAxes;
	}
	inline unsignedPair GetPollingFreq() const
	{
		return std::make_pair(m_caps.wPeriodMin, m_caps.wPeriodMax);
	}
	inline unsignedPair GetRangeX() const
	{
		return std::make_pair(m_caps.wXmin, m_caps.wXmax);
	}
	inline unsignedPair GetRangeY() const
	{
		return std::make_pair(m_caps.wYmin, m_caps.wYmax);
	}
	inline unsignedPair GetRangeZ() const
	{
		return std::make_pair(m_caps.wZmin, m_caps.wZmax);
	}
	inline unsignedPair GetRangeR() const
	{
		return std::make_pair(m_caps.wRmin, m_caps.wRmax);
	}
	inline unsignedPair GetRangeU() const
	{
		return std::make_pair(m_caps.wUmin, m_caps.wUmax);
	}
	inline unsignedPair GetRangeV() const
	{
		return std::make_pair(m_caps.wVmin, m_caps.wVmax);
	}
	inline bool HasAxisZ() const
	{
		return m_caps.wCaps & JOYCAPS_HASZ;
	}
	inline bool HasAxisR() const
	{
		return (m_caps.wCaps & JOYCAPS_HASR) != 0;
	}
	inline bool HasAxisU() const
	{
		return (m_caps.wCaps & JOYCAPS_HASU) != 0;
	}
	inline bool HasAxisV() const
	{
		return (m_caps.wCaps & JOYCAPS_HASV) != 0;
	}
	inline EPovType GetPovType() const
	{
		if( 0 == (m_caps.wCaps & JOYCAPS_HASPOV) )
			return ePovType_no_pov;
		if( 0 != (m_caps.wCaps & JOYCAPS_POVCTS) )
			return ePovType_continuous;
		else if( 0 != (m_caps.wCaps & JOYCAPS_POV4DIR) )
			return ePovType_discrete;
		else
			return ePovType_unknown;
	}

	void SetInverted( EAxis axis, bool val = true );
	bool IsInverted( EAxis axis) const;

	bool WasPressed( EButton b ) const;
	bool WasReleased( EButton b ) const;
	bool IsPressed( EButton b ) const;

	bool WasPovPressed( EPOV direction ) const;

	bool WasSectionEnteredXY(float min, float max) const;
	bool WasSectionEnteredRZ(float min, float max) const;
	bool WasSectionLeft(float min, float max) const;
	bool IsInSection(const float alpha, const float min, const float max) const;

	template<class T>
	T GetAngle( int x, int y, float factor = 0.0f, int max = 0 ) const;
	float GetAngleXY() const;
	float GetAngleRZ() const;

	int PressedCount() const;

	unsigned GetXPos() const;
	unsigned GetYPos() const;
	unsigned GetZPos() const;
	unsigned GetRPos() const;
	unsigned GetUPos() const;
	unsigned GetVPos() const;
	unsigned GetPOV() const;

	EState ReadData();
	EState GetState() const;

	/// will return -1 if threshold can not be retrieved
	unsigned GetThreshold() const;

	///	Sets the movement threshold for the current controller.
	/// @return true if value has been successfully set
	bool SetThreshold( unsigned threshold ) const;

	bool Capture( HWND hwnd, unsigned int period = 125,
				  EUpdateAction when = update_on_change );
	bool Release();

private:
	void Reset_();

private:
	unsigned int m_currentId;
	std::bitset<eAxis_MAX> m_invertedAxes;
	JOYCAPS m_caps;
	JOYINFOEX m_data;
	JOYINFOEX m_lastData;
	EState m_state;
	HWND m_hwnd;
};


} //namespace FMlib

#endif //GAME_PAD_HEADER_INCLUDED
