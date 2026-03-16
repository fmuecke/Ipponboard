// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/path_helpers.h"
#include "TestQtApp.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QUuid>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("[PathHelpers] Settings helper uses config directory")
{
    auto& app = ensure_qt_app();
    app.setApplicationName(QStringLiteral("IpponboardTestApp"));
    const auto expectedDir = fm::GetAppConfigDir();
    const auto settingsPath = fm::GetAppConfigFilePath(QStringLiteral("example.ini"));
    INFO("Settings path: " + settingsPath.toStdString());
    REQUIRE(settingsPath ==
            QDir::toNativeSeparators(QDir(expectedDir).filePath(QStringLiteral("example.ini"))));
}

TEST_CASE("[PathHelpers] Local data helper uses AppLocalDataLocation")
{
    auto& app = ensure_qt_app();
    app.setApplicationName(QStringLiteral("IpponboardTestApp"));
    const auto localDir = fm::GetAppLocalDataDir();
    const auto expectedLocal =
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    INFO("Local data dir: " + localDir.toStdString());
    REQUIRE_FALSE(localDir.isEmpty());
    REQUIRE(localDir == expectedLocal);
    REQUIRE(QFileInfo(localDir).isWritable());
}
TEST_CASE("[PathHelpers] ResolveConfigFileForRead prefers user config")
{
    auto& app = ensure_qt_app();
    app.setApplicationName(QStringLiteral("IpponboardTestApp"));

    const auto fileName = QStringLiteral("resolve_read_prefer_user_%1.config")
                              .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));

    const auto userConfigPath = fm::GetAppConfigFilePath(fileName);
    const auto appDirPath = fm::GetAppDirFilePath(fileName);

    QFile::remove(userConfigPath);
    QFile::remove(appDirPath);

    QFile appFile(appDirPath);
    if (appFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        appFile.write("app");
        appFile.close();
    }

    QFile userFile(userConfigPath);
    REQUIRE(userFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    userFile.write("user");
    userFile.close();

    const auto resolved = fm::ResolveConfigFileForRead(fileName);
    REQUIRE(resolved == userConfigPath);

    QFile::remove(userConfigPath);
    QFile::remove(appDirPath);
}

TEST_CASE("[PathHelpers] ResolveConfigFileForRead falls back to app directory")
{
    auto& app = ensure_qt_app();
    app.setApplicationName(QStringLiteral("IpponboardTestApp"));

    const auto fileName = QStringLiteral("resolve_read_fallback_%1.config")
                              .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));

    const auto userConfigPath = fm::GetAppConfigFilePath(fileName);
    const auto appDirPath = fm::GetAppDirFilePath(fileName);

    QFile::remove(userConfigPath);

    const auto resolved = fm::ResolveConfigFileForRead(fileName);
    REQUIRE(resolved == appDirPath);
}

TEST_CASE("[PathHelpers] Enumerate storage locations")
{
    auto& app = ensure_qt_app();
    app.setApplicationName(QStringLiteral("IpponboardTestApp"));
    app.setApplicationVersion(QStringLiteral("0.1-test"));

    const auto programDir = QCoreApplication::applicationDirPath();
    INFO("Program directory: " + programDir.toStdString());
    INFO("Program dir writable: " + std::to_string(QFileInfo(programDir).isWritable()));
    REQUIRE_FALSE(programDir.isEmpty());
    REQUIRE(QDir(programDir).exists());

    REQUIRE(QCoreApplication::applicationName().length() > 0); // application name must be set

    const auto configDir = fm::GetAppConfigDir();
    INFO("Config / autosave directory: " + configDir.toStdString());
    REQUIRE_FALSE(configDir.isEmpty());
    REQUIRE(QDir().mkpath(configDir));
    REQUIRE(QFileInfo(configDir).isWritable());
    const auto nativeConfigDir = QDir::toNativeSeparators(configDir);
#ifdef Q_OS_WIN
    REQUIRE(nativeConfigDir.contains(QStringLiteral("AppData\\Roaming")));
#else
    const auto expectedConfig = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    REQUIRE(configDir == expectedConfig);
#endif

    const auto localDataDir = fm::GetAppLocalDataDir();
    INFO("Static data directory: " + localDataDir.toStdString());
    REQUIRE_FALSE(localDataDir.isEmpty());
    REQUIRE(QDir().mkpath(localDataDir));
    REQUIRE(QFileInfo(localDataDir).isWritable());
    const auto nativeLocalDir = QDir::toNativeSeparators(localDataDir);
#ifdef Q_OS_WIN
    REQUIRE(nativeLocalDir.contains(QStringLiteral("AppData\\Local")));
#else
    const auto expectedLocal =
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    REQUIRE(localDataDir == expectedLocal);
#endif

#ifdef _WIN32
    INFO("Platform: Windows");
#else
    INFO("Platform: Linux / Unix");
#endif
}
