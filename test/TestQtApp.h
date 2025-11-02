#pragma once

#include <QApplication>

inline QApplication& ensure_qt_app()
{
    if (auto* instance = qobject_cast<QApplication*>(QCoreApplication::instance()))
    {
        return *instance;
    }

    static int argc = 0;
    static char appName[] = "test";
    static char* argv[] = { appName, nullptr };
    // NOTE: We intentionally leak the heap-allocated QApplication. Letting Qt
    // run ~QApplication without ever entering exec() (as our tests do) causes a
    // Qt 6.9.2 multimedia backend double-destroy; deferring cleanup to process
    // teardown keeps the test runner stable. Revisit once Qt fixes the issue.
    static QApplication* app = new QApplication(argc, argv);
    return *app;
}
