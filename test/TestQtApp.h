#pragma once

#include <QApplication>

inline QApplication& ensure_qt_app()
{
	if (auto* instance = qobject_cast<QApplication*>(QCoreApplication::instance())) { return *instance; }

	static int argc = 0;
	static char appName[] = "test";
	static char* argv[] = { appName, nullptr };
	static QApplication app(argc, argv);
	return app;
}
