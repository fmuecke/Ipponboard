// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../base/OnlineVersionChecker.h"
#include "../base/OnlineVersionChecker.cpp"

#include <QCoreApplication>
#include "TestQtApp.h"
#include <QString>
#include "TestDataPath.h"

#include <iostream>

TEST_CASE("[OnlineVersionChecker] get online version document (github json)")
{
    ensure_qt_app();

    auto start = std::chrono::high_resolution_clock::now();

    auto versionData = OnlineVersionChecker::get_version_document(OnlineVersionChecker::VersionDocumentUrl);
	REQUIRE_FALSE(versionData.isEmpty());

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    INFO("retrieving version data took " << elapsed.count() << "ms");

    CHECK(elapsed.count() <= 5000);
}

TEST_CASE("[OnlineVersionChecker] parse version document (github json)")
{
    ensure_qt_app();

    auto versionInfo = OnlineVersionChecker::parse_version_document(
        OnlineVersionChecker::get_version_document(
            QString::fromStdString(ippn_test_data_string("latest_version.json"))));

    REQUIRE_FALSE(versionInfo.version.isEmpty());
    REQUIRE_FALSE(versionInfo.infoUrl.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_en.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_de.isEmpty());
    REQUIRE_FALSE(versionInfo.downloadUrl.isEmpty());
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
