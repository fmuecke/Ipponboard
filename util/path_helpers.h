// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef UTIL__PATH_HELPERS_H_
#define UTIL__PATH_HELPERS_H_

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QProcessEnvironment>
#include <QStandardPaths>
#include <string>
#include <utility>

namespace fm
{

namespace
{

QString ensureDirectory(QString directory)
{
    if (!directory.isEmpty())
    {
        QDir().mkpath(directory); // make sure directory exists
    }
    return directory;
}

QString GetAppConfigDir()
{
#ifdef Q_OS_WIN
    const auto roamingBase = qEnvironmentVariable("APPDATA");
    if (!roamingBase.isEmpty())
    {
        return ensureDirectory(QDir(QDir::fromNativeSeparators(roamingBase))
                                   .filePath(QCoreApplication::applicationName()));
    }
#endif
    return ensureDirectory(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
}

QString GetAppLocalDataDir()
{
#ifdef Q_OS_WIN
    const auto localBase = qEnvironmentVariable("LOCALAPPDATA");
    if (!localBase.isEmpty())
    {
        return ensureDirectory(QDir(QDir::fromNativeSeparators(localBase))
                                   .filePath(QCoreApplication::applicationName()));
    }
#endif
    return ensureDirectory(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
}

QString GetAppConfigFilePath(QString fileName)
{
    auto configPath = GetAppConfigDir();
    return QDir::toNativeSeparators(QDir(configPath).filePath(std::move(fileName)));
}

QString GetAppDirFilePath(QString fileName)
{
    auto dataPath = QCoreApplication::applicationDirPath();
    return QDir::toNativeSeparators(QDir(dataPath).filePath(std::move(fileName)));
}

QString ResolveConfigFileForRead(QString fileName)
{
    const auto userConfigFile = GetAppConfigFilePath(fileName);
    if (QFile::exists(userConfigFile))
    {
        return userConfigFile;
    }

    return GetAppDirFilePath(std::move(fileName));
}

} // anonymous namespace
} // namespace fm

#endif // UTIL__PATH_HELPERS_H_

