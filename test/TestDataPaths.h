// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#include <QCoreApplication>
#include <QDir>
#include <QString>
#include <string>

namespace Ipponboard::TestSupport
{

[[nodiscard]] inline QString TestDataDirectory()
{
    if (QCoreApplication::instance() != nullptr)
    {
        QDir appDir(QCoreApplication::applicationDirPath());
        return appDir.filePath(QStringLiteral("TestData"));
    }

#ifdef IPPONBOARD_TESTDATA_DIR
    return QString::fromUtf8(IPPONBOARD_TESTDATA_DIR);
#else
    return QString();
#endif
}

[[nodiscard]] inline QString ResolveTestDataPath(const QString& relativeFileName)
{
    const QDir dataDir(TestDataDirectory());
    return dataDir.filePath(relativeFileName);
}

[[nodiscard]] inline std::string ResolveTestDataPathStd(const QString& relativeFileName)
{
    return ResolveTestDataPath(relativeFileName).toUtf8().constData();
}

} // namespace Ipponboard::TestSupport
