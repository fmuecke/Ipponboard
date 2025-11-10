#pragma once

#include <QApplication>
#include <QCoreApplication>
#include <QMessageLogContext>

inline QApplication& ensure_qt_app()
{
    if (auto* instance = qobject_cast<QApplication*>(QCoreApplication::instance()))
    {
        return *instance;
    }

    static QtMessageHandler previousHandler = nullptr;
    static bool handlerRegistered = false;
    if (!handlerRegistered)
    {
        previousHandler = qInstallMessageHandler(
            [](QtMsgType type, const QMessageLogContext& context, const QString& msg)
            {
                if (type == QtWarningMsg && msg.startsWith(QStringLiteral("QThreadStorage: entry")))
                {
                    return;
                }
                if (previousHandler != nullptr)
                {
                    previousHandler(type, context, msg);
                }
                else
                {
                    qt_message_output(type, context, msg);
                }
            });
        handlerRegistered = true;
    }

    static int argc = 0;
    static char appName[] = "test";
    static char* argv[] = { appName, nullptr };
    static QApplication* app = new QApplication(argc, argv);
    return *app;
}
