#ifndef X11_HELPERS_H
#define X11_HELPERS_H

#include <QApplication>
#include <QDesktopWidget>
#include <QList>
#include <QRect>
#include <QScreen>

#ifdef __linux__
#include<X11/Xlib.h>
#include<X11/extensions/Xrandr.h>
#endif

class ScreenHelpers
{
public:
    ScreenHelpers()
    {
        init();
    }

    static ScreenHelpers* getInstance()
    {
        if(s_pInstance==NULL)
            s_pInstance = new ScreenHelpers();

        return s_pInstance;
    }

    void init()
    {
        m_screenResolutionsList.clear();

#ifdef __WITH_X11__
        // if the QT function doesnt work under Linux, try a X11 solution
#ifdef __QT5__
        if (QGuiApplication::screens().count() < 2)
#elif __QT4__
        if (QApplication::desktop()->numScreens() < 2)
#endif
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
#endif
    }

    const int numScreens()
    {
        if (m_screenResolutionsList.size() > 1)
            return m_screenResolutionsList.size();
        else
#ifdef __QT5__
            return QGuiApplication::screens().count();
#elif __QT4__
            return QApplication::desktop()->numScreens();
#endif
    }

    const QRect getScreenGeometry(int screen_no=-1)
    {
        if (screen_no == -1)
            screen_no = 0;

        if (m_screenResolutionsList.size() > 1)
            return m_screenResolutionsList.at(screen_no);
        else
#ifdef __QT5__
            return QGuiApplication::screens().at(screen_no)->availableGeometry();
#elif __QT4__
            return QApplication::desktop()->availableGeometry(screen_no);
#endif
    }

protected:
    inline static ScreenHelpers* s_pInstance = NULL;

private:
    QList<QRect> m_screenResolutionsList;
};
#endif // X11_HELPERS_H

