// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../base/OnlineVersionChecker.cpp"
#include "../base/OnlineVersionChecker.h"
#include "TestDataPaths.h"
#include "TestQtApp.h"

#include <QCoreApplication>
#include <QString>
#include <catch2/catch_test_macros.hpp>
#include <string>

TEST_CASE("[OnlineVersionChecker] get online version document (github json)")
{
    ensure_qt_app();

    auto start = std::chrono::high_resolution_clock::now();
    auto versionData =
        OnlineVersionChecker::get_version_document(OnlineVersionChecker::VersionDocumentUrl);
    REQUIRE_FALSE(versionData.isEmpty());
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    INFO("retrieving version data took " << elapsed.count() << "ms");

    CHECK(elapsed.count() <= 5000);
}

TEST_CASE("[OnlineVersionChecker] parse version document (github json)")
{
    ensure_qt_app();

    auto versionInfo =
        OnlineVersionChecker::parse_version_document(OnlineVersionChecker::get_version_document(
            Ipponboard::TestSupport::ResolveTestDataPath(QStringLiteral("latest_version.json"))));

    REQUIRE_FALSE(versionInfo.version.isEmpty());
    REQUIRE_FALSE(versionInfo.infoUrl.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_en.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_de.isEmpty());
    REQUIRE_FALSE(versionInfo.downloadUrl.isEmpty());

#if defined(Q_OS_WIN)
    CHECK(versionInfo.downloadUrl.endsWith(".exe"));
#else
    CHECK(versionInfo.downloadUrl ==
          QStringLiteral("https://github.com/fmuecke/Ipponboard/releases/tag/Releases/1.10.beta"));
#endif
}

TEST_CASE("[OnlineVersionChecker] parse online version document (github json)")
{
    ensure_qt_app();

    auto versionInfo = OnlineVersionChecker::parse_version_document(
        OnlineVersionChecker::get_version_document(OnlineVersionChecker::VersionDocumentUrl));

    REQUIRE_FALSE(versionInfo.version.isEmpty());
    REQUIRE_FALSE(versionInfo.infoUrl.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_en.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_de.isEmpty());
    REQUIRE_FALSE(versionInfo.downloadUrl.isEmpty());
}
