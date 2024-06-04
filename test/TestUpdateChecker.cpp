// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../base/UpdateChecker.h"
#include "../base/UpdateChecker.cpp"

#include <QCoreApplication>
#include <QString>
#include <iostream>

struct QApplicationFixture {
    QApplication* app;
    int argc = 0;
    char* argv[1] = { nullptr };

    QApplicationFixture() {
        app = new QApplication(argc, argv); // required for QXmlQuery and QNetworkAccessManager
    }

    ~QApplicationFixture() {
        delete app; // Cleanup
    }
};

TEST_CASE_METHOD(QApplicationFixture, "[UpdateChecker] get online version document (github json)")
{
    auto start = std::chrono::high_resolution_clock::now();

    auto versionData = UpdateChecker::get_version_document(UpdateChecker::VersionDocumentUrl);
	REQUIRE_FALSE(versionData.isEmpty());

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;
    INFO("retrieving version data took " << elapsed.count() << "ms");
    CHECK(elapsed.count() <= 500);
}

TEST_CASE_METHOD(QApplicationFixture, "[UpdateChecker] parse version document (github json)")
{
    auto versionInfo = UpdateChecker::parse_version_document(
        UpdateChecker::get_version_document("TestData/latest_version.json"));

    REQUIRE_FALSE(versionInfo.version.isEmpty());
    REQUIRE_FALSE(versionInfo.infoUrl.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_en.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_de.isEmpty());
    REQUIRE_FALSE(versionInfo.downloadUrl.isEmpty());
}

TEST_CASE_METHOD(QApplicationFixture, "[UpdateChecker] parse online version document (github json)")
{
    auto versionInfo = UpdateChecker::parse_version_document(
        UpdateChecker::get_version_document(UpdateChecker::VersionDocumentUrl));

    REQUIRE_FALSE(versionInfo.version.isEmpty());
    REQUIRE_FALSE(versionInfo.infoUrl.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_en.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_de.isEmpty());
    REQUIRE_FALSE(versionInfo.downloadUrl.isEmpty());
}

