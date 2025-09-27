// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef FMLIB_GAMEPAD_H_
#define FMLIB_GAMEPAD_H_

#pragma once

#include <Mmsystem.h>
#include <Windows.h>
#include <bitset>
#include <cassert>
#include <cmath>

#pragma comment(lib, "Winmm.lib")

namespace FMlib
{
class Gamepad
{
  public:
    using UnsignedPair = std::pair<unsigned, unsigned>;

    enum EButton
    {
        eButton1 = 0,
        eButton2,
        eButton3,
        eButton4,
        eButton5,
        eButton6,
        eButton7,
        eButton8,
        eButton9,
        eButton10,
        eButton11,
        eButton12,
        eButton13,
        eButton14,
        eButton15,
        eButton16,
        eButton17,
        eButton18,
        eButton19,
        eButton20,
        eButton21,
        eButton22,
        eButton23,
        eButton24,
        eButton25,
        eButton26,
        eButton27,
        eButton28,
        eButton29,
        eButton30,
        eButton31,
        eButton32,
        eButton_pov_fwd,
        eButton_pov_right,
        eButton_pov_back,
        eButton_pov_left,
        eButton_pov_right_fwd,
        eButton_pov_right_back,
        eButton_pov_left_back,
        eButton_pov_left_fwd
    };

    enum EState
    {
        eState_unknown = -1,
        eState_ok = JOYERR_NOERROR,
        eState_no_driver = MMSYSERR_NODRIVER,
        eState_invalid_param = MMSYSERR_INVALPARAM,
        eState_bad_device_id = MMSYSERR_BADDEVICEID,
        eState_no_service = JOYERR_NOCANDO,
        eState_unplugged = JOYERR_UNPLUGGED,
        eState_invalid_id = JOYERR_PARMS
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

    enum
    {
        eMax = 65535,
        eMid = 32767,
        eTolerance = 100
    };

    Gamepad()
    {
        reset();
        Init();
    }

    virtual ~Gamepad()
    {
        if (m_hwnd)
            Release();
    }

    Gamepad(const Gamepad&) = delete;
    Gamepad& operator=(const Gamepad&) = delete;

    /// reads capabilities
    EState Init()
    {
        m_state = EState(::joyGetDevCapsW(0, &m_caps, sizeof(JOYCAPSW)));
        return m_state;
    }

    inline const wchar_t* GetProductName() const { return m_caps.szPname; }

    inline WORD GetMId() const { return m_caps.wMid; }

    inline WORD GetPId() const { return m_caps.wPid; }

    inline unsigned GetNumButtons() const { return m_caps.wNumButtons; }

    inline unsigned GetNumAxes() const { return m_caps.wNumAxes; }

    inline UnsignedPair GetPollingFreq() const
    {
        return std::make_pair(m_caps.wPeriodMin, m_caps.wPeriodMax);
    }

    inline UnsignedPair GetRangeX() const { return std::make_pair(m_caps.wXmin, m_caps.wXmax); }

    inline UnsignedPair GetRangeY() const { return std::make_pair(m_caps.wYmin, m_caps.wYmax); }

    inline UnsignedPair GetRangeZ() const { return std::make_pair(m_caps.wZmin, m_caps.wZmax); }

    inline UnsignedPair GetRangeR() const { return std::make_pair(m_caps.wRmin, m_caps.wRmax); }

    inline UnsignedPair GetRangeU() const { return std::make_pair(m_caps.wUmin, m_caps.wUmax); }

    inline UnsignedPair GetRangeV() const { return std::make_pair(m_caps.wVmin, m_caps.wVmax); }

    inline bool HasAxisZ() const { return m_caps.wCaps & JOYCAPS_HASZ; }

    inline bool HasAxisR() const { return (m_caps.wCaps & JOYCAPS_HASR) != 0; }

    inline bool HasAxisU() const { return (m_caps.wCaps & JOYCAPS_HASU) != 0; }

    inline bool HasAxisV() const { return (m_caps.wCaps & JOYCAPS_HASV) != 0; }
    inline EPovType GetPovType() const
    {
        if (0 == (m_caps.wCaps & JOYCAPS_HASPOV))
            return ePovType_no_pov;

        if (0 != (m_caps.wCaps & JOYCAPS_POVCTS))
            return ePovType_continuous;
        else if (0 != (m_caps.wCaps & JOYCAPS_POV4DIR))
            return ePovType_discrete;
        else
            return ePovType_unknown;
    }

    void SetInverted(EAxis axis, bool val = true) { m_invertedAxes.set(axis, val); }

    bool IsInverted(EAxis axis) const { return m_invertedAxes.test(axis); }

    bool WasPressed(EButton b) const
    {
        if (b < 0 || b >= sizeof(button_code) / sizeof(button_code[0]))
            return false; // invalid button (check for out of bounds)

        if (b > eButton32)
        {
            if (button_code[b] == m_data.dwPOV && m_data.dwPOV != m_lastData.dwPOV)
            {
                return true;
            }

            return false;
        }

        return (m_lastData.dwButtons & button_code[b]) == 0 &&
               (m_data.dwButtons & button_code[b]) > 0;
    }

    bool WasReleased(EButton b) const
    {
        if (b < 0 || b >= sizeof(button_code) / sizeof(button_code[0]))
            return false; // invalid button (check for out of bounds)

        return (m_lastData.dwButtons & button_code[b]) > 0 &&
               (m_data.dwButtons & button_code[b]) == 0;
    }

    bool IsPressed(EButton b) const
    {
        if (b < 0 || b >= sizeof(button_code) / sizeof(button_code[0]))
            return false; // invalid button (check for out of bounds)

        return (m_data.dwButtons & button_code[b]) > 0;
    }

    bool WasSectionEnteredXY(float min, float max) const
    {
        const int lastX =
            m_invertedAxes.test(eAxis_X) ? eMax - m_lastData.dwXpos : m_lastData.dwXpos;
        const int lastY =
            m_invertedAxes.test(eAxis_Y) ? eMax - m_lastData.dwYpos : m_lastData.dwYpos;

        int curX = m_invertedAxes.test(eAxis_X) ? eMax - m_data.dwXpos : m_data.dwXpos;
        int curY = m_invertedAxes.test(eAxis_Y) ? eMax - m_data.dwYpos : m_data.dwYpos;

        const float lastAlpha = GetAngle<float>(lastX - eMid, lastY - eMid, 1.02f, eMid);

        const float curAlpha = GetAngle<float>(curX - eMid, curY - eMid, 1.02f, eMid);

        bool changed(false);

        if (-1.0f == lastAlpha || -1.0f == curAlpha || fabs(lastAlpha - curAlpha) > eTolerance)
        {
            changed = true;
        }

        if (changed && IsInSection(curAlpha, min, max))
        {
            return true;
        }

        return false;
    }

    bool WasSectionEnteredRZ(float min, float max) const
    {
        const int lastR =
            m_invertedAxes.test(eAxis_R) ? eMax - m_lastData.dwRpos : m_lastData.dwRpos;
        const int lastZ =
            m_invertedAxes.test(eAxis_Z) ? eMax - m_lastData.dwZpos : m_lastData.dwZpos;

        int curR = m_invertedAxes.test(eAxis_R) ? eMax - m_data.dwRpos : m_data.dwRpos;
        int curZ = m_invertedAxes.test(eAxis_Z) ? eMax - m_data.dwZpos : m_data.dwZpos;

        const float lastAlpha = GetAngle<float>(lastR - eMid, lastZ - eMid, 1.02f, eMid);

        const float curAlpha = GetAngle<float>(curR - eMid, curZ - eMid, 1.02f, eMid);

        bool changed(false);

        if (-1.0f == lastAlpha || -1.0f == curAlpha || fabs(lastAlpha - curAlpha) > eTolerance)
        {
            changed = true;
        }

        if (changed && IsInSection(curAlpha, min, max))
        {
            return true;
        }

        return false;
    }

    bool WasSectionLeft(float /*min*/, float /*max*/) const { return false; }

    bool IsInSection(const float alpha, const float min, const float max) const
    {
        bool ret(false);

        if (min > max)
        {
            if (min <= alpha && alpha <= 360.0f)
            {
                ret = true;
            }
            else if (0 < alpha && alpha <= max)
            {
                ret = true;
            }
        }
        else
        {
            if (min <= alpha && max >= alpha)
            {
                ret = true;
            }
        }

        return ret;
    }

    template <class T> T GetAngle(int x, int y, float factor = 0.0f, int max = 0) const
    {
        if (0 == x && 0 == y)
        {
            return -1;
        }

        T h = sqrt((T)x * x + (T)y * y);

        if (h * factor < (float)max)
        {
            return -1;
        }

        T angle = asin((T)y / h) * 180 / (T)3.141592653589793L;
        angle = x > 0 ? 90 - angle : 270 + angle;

        return angle;
    }

    //FIXME: use double
    float GetAngleXY() const
    {
        return GetAngle<float>(GetXPos() - eMid, GetYPos() - eMid, 1.2f, eMid);
    }
    float GetAngleRZ() const
    {
        return GetAngle<float>(GetRPos() - eMid, GetZPos() - eMid, 1.2f, eMid);
    }

    int PressedCount() const { return m_data.dwButtonNumber; }

    unsigned GetXPos() const
    {
        if (m_invertedAxes.test(eAxis_X))
            return 65535 - m_data.dwXpos;

        return m_data.dwXpos;
    }

    unsigned GetYPos() const
    {
        if (m_invertedAxes.test(eAxis_Y))
            return 65535 - m_data.dwYpos;

        return m_data.dwYpos;
    }

    unsigned GetZPos() const
    {
        if (m_invertedAxes.test(eAxis_Z))
            return 65535 - m_data.dwZpos;

        return m_data.dwZpos;
    }

    unsigned GetRPos() const
    {
        if (m_invertedAxes.test(eAxis_R))
            return 65535 - m_data.dwRpos;

        return m_data.dwRpos;
    }
    unsigned GetUPos() const
    {
        if (m_invertedAxes.test(eAxis_U))
            return 65535 - m_data.dwUpos;

        return m_data.dwUpos;
    }

    unsigned GetVPos() const
    {
        if (m_invertedAxes.test(eAxis_V))
            return 65535 - m_data.dwVpos;

        return m_data.dwVpos;
    }

    unsigned GetPOV() const { return m_data.dwPOV; }

    EState ReadData()
    {
        m_lastData = m_data;
        m_state = EState(::joyGetPosEx(m_currentId, &m_data));
        return m_state;
    }

    EState GetState() const { return m_state; }

    /// will return -1 if threshold can not be retrieved
    unsigned GetThreshold() const
    {
        unsigned t(0);

        if (eState_ok != ::joyGetThreshold(m_currentId, &t))
            return static_cast<unsigned>(-1);

        return t;
    }

    ///	Sets the movement threshold for the current controller.
    /// @return true if value has been successfully set
    bool SetThreshold(unsigned threshold) const
    {
        return eState_ok == ::joySetThreshold(m_currentId, threshold);
    }

    bool Capture(HWND hwnd, unsigned int period = 125, EUpdateAction when = update_on_change)
    {
        bool ret(false);

        if (NULL == m_hwnd)
        {
            int s(::joySetCapture(hwnd, m_currentId, period, when ? TRUE : FALSE));

            if (eState_ok == s)
            {
                m_hwnd = hwnd;
                ret = true;
            }
        }

        return ret;
    }

    bool Release()
    {
        bool ret(false);

        if (NULL != m_hwnd)
        {
            int s(::joyReleaseCapture(m_currentId));

            if (eState_ok == s)
            {
                m_hwnd = nullptr;
                ret = true;
            }
        }

        return ret;
    }

  private:
    void reset()
    {
        m_invertedAxes.reset();

        memset(&m_caps, 0, sizeof(JOYCAPSW));
        memset(&m_data, 0, sizeof(JOYINFOEX));
        m_data.dwSize = sizeof(JOYINFOEX);
        m_data.dwFlags = JOY_RETURNALL;
        m_lastData = m_data;
    }

    const unsigned int button_code[40] = {
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
        0,     // eButton_pov_fwd
        9000,  // eButton_pov_right
        18000, // eButton_pov_back
        27000, // eButton_pov_left
        4500,  // ePov_right_fwd
        13500, // eButton_pov_right_back
        22500, // eButton_pov_left_back
        31500  // eButton_pov_left_fwd
    };

    unsigned int m_currentId{ 0 };
    std::bitset<eAxis_MAX> m_invertedAxes{ 0 };

    JOYCAPSW m_caps{ 0 };
    JOYINFOEX m_data{ 0 };
    JOYINFOEX m_lastData{ 0 };
    EState m_state{ eState_unknown };
    HWND m_hwnd{ nullptr };
};

} //namespace FMlib

#endif // FMLIB_GAMEPAD_H_
