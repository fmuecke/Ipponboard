#pragma once

#include <QCoreApplication>

inline QCoreApplication& ensure_qt_app()
{
    if (QCoreApplication::instance())
    {
        return *QCoreApplication::instance();
    }

    static int argc = 0;
    static char appName[] = "test";
    static char* argv[] = { appName, nullptr };
    static QCoreApplication app(argc, argv);
    return app;
}
