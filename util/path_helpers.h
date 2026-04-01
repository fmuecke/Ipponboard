// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef UTIL__PATH_HELPERS_H_
#define UTIL__PATH_HELPERS_H_

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <utility>

namespace fm
{

inline QString ensureDirectory(QString directory)
{
    if (!directory.isEmpty())
    {
        QDir().mkpath(directory);
    }
    return directory;
}

inline QString GetProgramDataDirForExecutableDir(QString executableDir,
                                                 bool preferBundleResources = false)
{
    const auto cleanedExecutableDir = QDir::cleanPath(std::move(executableDir));

    if (preferBundleResources)
    {
        const auto resourcesDir =
            QDir(cleanedExecutableDir).absoluteFilePath(QStringLiteral("../Resources"));
        const QFileInfo resourcesInfo(resourcesDir);

        if (resourcesInfo.exists() && resourcesInfo.isDir())
        {
            return QDir::cleanPath(resourcesInfo.absoluteFilePath());
        }
    }

    return cleanedExecutableDir;
}

inline QString GetProgramDataDir()
{
#if defined(Q_OS_MACOS)
    constexpr bool kPreferBundleResources = true;
#else
    constexpr bool kPreferBundleResources = false;
#endif

    return GetProgramDataDirForExecutableDir(QCoreApplication::applicationDirPath(),
                                             kPreferBundleResources);
}

inline QString GetProgramDataFilePath(QString fileName)
{
    const auto programDataDir = GetProgramDataDir();
    return QDir::toNativeSeparators(QDir(programDataDir).filePath(std::move(fileName)));
}

inline QString GetConfigDir()
{
    return ensureDirectory(QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation));
}

inline QString GetLocalDataDir()
{
    return ensureDirectory(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation));
}

inline QString GetConfigFilePath(QString fileName)
{
    const auto configPath = GetConfigDir();
    return QDir::toNativeSeparators(QDir(configPath).filePath(std::move(fileName)));
}

inline QString GetLocalDataFilePath(QString fileName)
{
    const auto localDataPath = GetLocalDataDir();
    return QDir::toNativeSeparators(QDir(localDataPath).filePath(std::move(fileName)));
}

inline QString ResolveConfigFileForRead(QString fileName)
{
    const auto userConfigFile = GetConfigFilePath(fileName);
    if (QFile::exists(userConfigFile))
    {
        return userConfigFile;
    }

    return GetProgramDataFilePath(std::move(fileName));
}

inline QString ResolveAssetReference(QString reference, QString ownerDir)
{
    if (reference.isEmpty())
    {
        return reference;
    }

    if (reference.startsWith(QStringLiteral(":/")) || QFileInfo(reference).isAbsolute())
    {
        return reference;
    }

    reference.replace(QChar('\\'), QChar('/'));

    if (!ownerDir.isEmpty())
    {
        const auto ownerRelativePath = QDir::toNativeSeparators(QDir(ownerDir).filePath(reference));
        if (QFile::exists(ownerRelativePath))
        {
            return ownerRelativePath;
        }
    }

    return GetProgramDataFilePath(std::move(reference));
}

inline QString ResolveConfigOwnedAsset(QString configFilePath, QString relativeReference)
{
    if (relativeReference.isEmpty())
    {
        return relativeReference;
    }

    if (relativeReference.startsWith(QStringLiteral(":/")) ||
        QFileInfo(relativeReference).isAbsolute())
    {
        return relativeReference;
    }

    const QFileInfo configInfo(configFilePath);
    const auto ownerDir = configInfo.exists() ? configInfo.dir().absolutePath() : GetConfigDir();
    return ResolveAssetReference(std::move(relativeReference), ownerDir);
}

} // namespace fm

#endif // UTIL__PATH_HELPERS_H_
