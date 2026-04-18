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
#include <QTemporaryDir>
#include <QUuid>
#include <catch2/catch_test_macros.hpp>

namespace
{
QCoreApplication& prepareQtApp()
{
    QStandardPaths::setTestModeEnabled(true);
    auto& app = ensure_qt_app();
    app.setApplicationName(QStringLiteral("IpponboardTestApp"));
    app.setApplicationVersion(QStringLiteral("0.1-test"));
    return app;
}
} // namespace

TEST_CASE("[PathHelpers] Settings helper uses config directory")
{
    auto& app = prepareQtApp();
    Q_UNUSED(app);

    const auto expectedDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    const auto configDir = fm::GetConfigDir();
    const auto settingsPath = fm::GetConfigFilePath(QStringLiteral("example.ini"));

    INFO("Config dir: " + configDir.toStdString());
    INFO("Settings path: " + settingsPath.toStdString());
    REQUIRE(configDir == expectedDir);
    REQUIRE(settingsPath ==
            QDir::toNativeSeparators(QDir(configDir).filePath(QStringLiteral("example.ini"))));
}

TEST_CASE("[PathHelpers] Local data helper uses AppLocalDataLocation")
{
    auto& app = prepareQtApp();
    Q_UNUSED(app);

    const auto localDir = fm::GetLocalDataDir();
    const auto expectedLocal =
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);

    INFO("Local data dir: " + localDir.toStdString());
    REQUIRE_FALSE(localDir.isEmpty());
    REQUIRE(localDir == expectedLocal);
    REQUIRE(QFileInfo(localDir).isWritable());
}

TEST_CASE("[PathHelpers] Local data file helper uses local app data directory")
{
    auto& app = prepareQtApp();
    Q_UNUSED(app);

    const auto localFilePath = fm::GetLocalDataFilePath(QStringLiteral("example.log"));

    INFO("Local data file path: " + localFilePath.toStdString());
    REQUIRE(localFilePath ==
            QDir::toNativeSeparators(
                QDir(fm::GetLocalDataDir()).filePath(QStringLiteral("example.log"))));
}

TEST_CASE("[PathHelpers] ResolveConfigFileForRead prefers user config")
{
    auto& app = prepareQtApp();
    Q_UNUSED(app);

    const auto fileName = QStringLiteral("resolve_read_prefer_user_%1.config")
                              .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));

    const auto userConfigPath = fm::GetConfigFilePath(fileName);
    const auto programDataPath = fm::GetProgramDataFilePath(fileName);

    QFile::remove(userConfigPath);
    QFile::remove(programDataPath);

    QFile programFile(programDataPath);
    if (programFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        programFile.write("program");
        programFile.close();
    }

    QFile userFile(userConfigPath);
    REQUIRE(userFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    userFile.write("user");
    userFile.close();

    const auto resolved = fm::ResolveConfigFileForRead(fileName);
    REQUIRE(resolved == userConfigPath);

    QFile::remove(userConfigPath);
    QFile::remove(programDataPath);
}

TEST_CASE("[PathHelpers] ResolveConfigFileForRead falls back to packaged program data")
{
    auto& app = prepareQtApp();
    Q_UNUSED(app);

    const auto fileName = QStringLiteral("resolve_read_fallback_%1.config")
                              .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));

    const auto userConfigPath = fm::GetConfigFilePath(fileName);
    const auto programDataPath = fm::GetProgramDataFilePath(fileName);

    QFile::remove(userConfigPath);

    const auto resolved = fm::ResolveConfigFileForRead(fileName);
    REQUIRE(resolved == programDataPath);
}

TEST_CASE("[PathHelpers] ResolveAssetReference keeps absolute and qrc references")
{
    auto& app = prepareQtApp();
    Q_UNUSED(app);

    const auto absoluteReference =
        QDir(fm::GetConfigDir()).filePath(QStringLiteral("absolute.txt"));
    REQUIRE(fm::ResolveAssetReference(QStringLiteral(":/sounds/Gong"), QString()) ==
            QStringLiteral(":/sounds/Gong"));
    REQUIRE(fm::ResolveAssetReference(absoluteReference, QString()) == absoluteReference);
}

TEST_CASE("[PathHelpers] ResolveAssetReference prefers owner-relative files")
{
    auto& app = prepareQtApp();
    Q_UNUSED(app);

    QTemporaryDir ownerDir;
    REQUIRE(ownerDir.isValid());

    const auto relativeName = QStringLiteral("templates/local-template.html");
    const auto ownerRelativePath = QDir(ownerDir.path()).filePath(relativeName);
    REQUIRE(QDir().mkpath(QFileInfo(ownerRelativePath).path()));

    QFile file(ownerRelativePath);
    REQUIRE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
    file.write("owner");
    file.close();

    const auto resolved = fm::ResolveAssetReference(relativeName, ownerDir.path());
    REQUIRE(resolved == QDir::toNativeSeparators(ownerRelativePath));
}

TEST_CASE("[PathHelpers] ResolveAssetReference normalizes backslash-separated relative files")
{
    auto& app = prepareQtApp();
    Q_UNUSED(app);

    QTemporaryDir ownerDir;
    REQUIRE(ownerDir.isValid());

    const auto unixStyleRelativeName = QStringLiteral("templates/local-template.html");
    const auto windowsStyleRelativeName = QStringLiteral("templates\\local-template.html");
    const auto ownerRelativePath = QDir(ownerDir.path()).filePath(unixStyleRelativeName);
    REQUIRE(QDir().mkpath(QFileInfo(ownerRelativePath).path()));

    QFile file(ownerRelativePath);
    REQUIRE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
    file.write("owner");
    file.close();

    const auto resolved = fm::ResolveAssetReference(windowsStyleRelativeName, ownerDir.path());
    REQUIRE(resolved == QDir::toNativeSeparators(ownerRelativePath));
}

TEST_CASE("[PathHelpers] ResolveAssetReference falls back to program data")
{
    auto& app = prepareQtApp();
    Q_UNUSED(app);

    const auto relativeName = QStringLiteral("path_helper_program_asset_%1.txt")
                                  .arg(QUuid::createUuid().toString(QUuid::WithoutBraces));
    const auto programDataPath = fm::GetProgramDataFilePath(relativeName);

    QFile file(programDataPath);
    REQUIRE(file.open(QIODevice::WriteOnly | QIODevice::Truncate));
    file.write("program");
    file.close();

    const auto resolved = fm::ResolveAssetReference(relativeName, fm::GetConfigDir());
    REQUIRE(resolved == programDataPath);

    QFile::remove(programDataPath);
}

TEST_CASE("[PathHelpers] ResolveConfigOwnedAsset prefers config-owned files and falls back")
{
    auto& app = prepareQtApp();
    Q_UNUSED(app);

    const auto configFilePath = fm::GetConfigFilePath(QStringLiteral("clubs.config"));
    const auto userRelative = QStringLiteral("TestData/CompetitionModes-test.ini");
    const auto userRelativePath = QDir(fm::GetConfigDir()).filePath(userRelative);
    const auto programRelativePath = fm::GetProgramDataFilePath(userRelative);

    REQUIRE(QDir().mkpath(QFileInfo(userRelativePath).path()));

    QFile userFile(userRelativePath);
    REQUIRE(userFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    userFile.write("user");
    userFile.close();

    REQUIRE(fm::ResolveConfigOwnedAsset(configFilePath, userRelative) ==
            QDir::toNativeSeparators(userRelativePath));

    QFile::remove(userRelativePath);
    REQUIRE(fm::ResolveConfigOwnedAsset(configFilePath, userRelative) == programRelativePath);
}

TEST_CASE("[PathHelpers] Program data directory resolves bundle resources when requested")
{
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());

    const auto executableDir =
        QDir(tempDir.path()).filePath(QStringLiteral("Ipponboard.app/Contents/MacOS"));
    const auto resourcesDir =
        QDir(tempDir.path()).filePath(QStringLiteral("Ipponboard.app/Contents/Resources"));
    REQUIRE(QDir().mkpath(executableDir));
    REQUIRE(QDir().mkpath(resourcesDir));

    const auto resolved = fm::GetProgramDataDirForExecutableDir(executableDir, true);
    REQUIRE(resolved == QDir::cleanPath(resourcesDir));
}

TEST_CASE("[PathHelpers] Program data directory falls back to executable directory")
{
    QTemporaryDir tempDir;
    REQUIRE(tempDir.isValid());

    const auto executableDir = QDir(tempDir.path()).filePath(QStringLiteral("bin"));
    REQUIRE(QDir().mkpath(executableDir));

    const auto resolved = fm::GetProgramDataDirForExecutableDir(executableDir, true);
    REQUIRE(resolved == QDir::cleanPath(executableDir));
}

TEST_CASE("[PathHelpers] Enumerate storage locations")
{
    auto& app = prepareQtApp();
    Q_UNUSED(app);

    const auto programDir = fm::GetProgramDataDir();
    INFO("Program directory: " + programDir.toStdString());
    INFO("Program dir writable: " + std::to_string(QFileInfo(programDir).isWritable()));
    REQUIRE_FALSE(programDir.isEmpty());
    REQUIRE(QDir(programDir).exists());

    const auto configDir = fm::GetConfigDir();
    INFO("Config directory: " + configDir.toStdString());
    REQUIRE_FALSE(configDir.isEmpty());
    REQUIRE(QDir().mkpath(configDir));
    REQUIRE(QFileInfo(configDir).isWritable());
    REQUIRE(configDir == QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));

    const auto localDataDir = fm::GetLocalDataDir();
    INFO("Local data directory: " + localDataDir.toStdString());
    REQUIRE_FALSE(localDataDir.isEmpty());
    REQUIRE(QDir().mkpath(localDataDir));
    REQUIRE(QFileInfo(localDataDir).isWritable());
    REQUIRE(localDataDir == QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
}
