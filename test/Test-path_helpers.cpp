// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/path_helpers.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <catch2/catch_test_macros.hpp>

#ifndef EXPERIMENTAL

#ifdef _WIN32

TEST_CASE("GetSettingsFilePath: returns back empty filename")
{
    auto path = fm::GetSettingsFilePath("");
    REQUIRE_FALSE(path.isEmpty());
    INFO(path.toStdString());
    REQUIRE(path.indexOf("\\Ipponboard") == path.size() - 11);
}

TEST_CASE("GetSettingsFilePath: returns a non empty value")
{
    auto path = fm::GetSettingsFilePath("somefile.ext");
    REQUIRE_FALSE(path.isEmpty());
    INFO(path.toStdString());
    REQUIRE(path.indexOf("\\Ipponboard\\somefile.ext") != -1);
}

TEST_CASE("KnowFolders: Desktop returns path to desktop")
{
    auto folder = fm::KnownFolders::get_Desktop();
    auto pos = folder.indexOf("Desktop");
    INFO(folder.toStdString());
    REQUIRE(pos == folder.size() - 7);
}

TEST_CASE("KnowFolders: LocalAppData returns valid path ")
{
    auto folder = fm::KnownFolders::get_LocalAppData();
    INFO(folder.toStdString());
    auto pos = folder.indexOf("AppData\\Local");
    REQUIRE(pos != -1);
}

#endif
#endif

struct QCoreApplicationFixture
{
	int argc = 0;
	char* argv[1] = { nullptr };
	QCoreApplication* app = nullptr;

	QCoreApplicationFixture()
	{
		app = new QCoreApplication(argc, argv);
		app->setApplicationName("IpponboardTestApp");
		app->setApplicationVersion("0.1-test");
	}

	~QCoreApplicationFixture()
	{
		delete app;
	}
};

TEST_CASE_METHOD(QCoreApplicationFixture, "[PathHelpers] Enumerate storage locations")
{
	const auto programDir = QCoreApplication::applicationDirPath();
	INFO("Program directory: " + programDir.toStdString());
	INFO("Program dir writable: " + std::to_string(QFileInfo(programDir).isWritable()));
	REQUIRE_FALSE(programDir.isEmpty());
	REQUIRE(QDir(programDir).exists());

	REQUIRE(QCoreApplication::applicationName().length() > 0); // application name must be set

	const auto configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
	INFO("Config / autosave directory: " + configDir.toStdString());
	REQUIRE_FALSE(configDir.isEmpty());
	REQUIRE(QDir().mkpath(configDir));
	REQUIRE(QFileInfo(configDir).isWritable());

	const auto dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
	INFO("User data directory: " + dataDir.toStdString());
	REQUIRE_FALSE(dataDir.isEmpty());
	REQUIRE(QDir().mkpath(dataDir));
	REQUIRE(QFileInfo(dataDir).isWritable());

#ifdef _WIN32
	INFO("Platform: Windows");
#else
	INFO("Platform: Linux / Unix");
#endif
}
