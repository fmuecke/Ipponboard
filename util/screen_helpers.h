#ifndef X11_HELPERS_H
#define X11_HELPERS_H

#include "debug.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QList>
#include <QRect>
#include <QScreen>

#ifdef __linux__
#include<X11/Xlib.h>
#ifdef __QT4__
#include<X11/extensions/Xrandr.h>
#endif
#endif

class ScreenHelpers
{
public:
    ScreenHelpers()
    {
        TRACE(4, "ScreenHelpers::ScreenHelpers()");
        init();
    }

    static ScreenHelpers* getInstance()
    {
        TRACE(4, "ScreenHelpers::getInstance()");
        if(s_pInstance==NULL)
            s_pInstance = new ScreenHelpers();

        return s_pInstance;
    }

    void init()
    {
        TRACE(4, "ScreenHelpers::init()");
        m_screenResolutionsList.clear();
#ifdef __linux__
#ifdef __WITH_X11__
        // if the QT function doesnt work under Linux, try a X11 solution
#ifdef __QT4__
        if (QApplication::desktop()->numScreens() < 2)
#else
        if (QGuiApplication::screens().count() < 2)
#endif // __QT4__
        {
            Display *display = XOpenDisplay(NULL);
            Window root = RootWindow(display, 0);
            XRRScreenResources* screenr = XRRGetScreenResources(display, root);
            int output = screenr->noutput;
            for (int i = 0; i < output; ++i)
            {
                XRROutputInfo* out_info = XRRGetOutputInfo(display, screenr, screenr->outputs[i]);
                if (NULL != out_info && out_info->connection == RR_Connected)
                {
                    XRRCrtcInfo* crt_info = XRRGetCrtcInfo(display, screenr, out_info->crtc);

                    m_screenResolutionsList.append(QRect(crt_info->x, crt_info->y, crt_info->width, crt_info->height));

                    XRRFreeCrtcInfo(crt_info);
                }
            }
        }
#endif // __WITH_X11__
#endif // __linux__
    }

    const int numScreens()
    {
        TRACE(2, "ScreenHelpers::numScreens()");
        if (m_screenResolutionsList.size() > 1)
            return m_screenResolutionsList.size();
        else
#ifdef __QT4__
            return QApplication::desktop()->numScreens();
#else
            return QGuiApplication::screens().count();
#endif // __QT4__
    }

    const QRect getScreenGeometry(int screen_no=-1)
    {
        if (screen_no == -1)
            screen_no = 0;

        if (m_screenResolutionsList.size() > 1)
            return m_screenResolutionsList.at(screen_no);
        else
#ifdef __QT4__
            return QApplication::desktop()->availableGeometry(screen_no);
#else
            return QGuiApplication::screens().at(screen_no)->availableGeometry();
#endif // __QT4__
    }

protected:
    inline static ScreenHelpers* s_pInstance = NULL;

private:
    QList<QRect> m_screenResolutionsList;
};
#endif // X11_HELPERS_H

