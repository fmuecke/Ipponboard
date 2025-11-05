// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../base/OnlineVersionChecker.cpp"
#include "../base/OnlineVersionChecker.h"
#include "../util/catch2/catch_amalgamated.hpp"
#include "TestDataPath.h"
#include "TestQtApp.h"

#include <QCoreApplication>
#include <QString>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cstdlib>
#include <string>

namespace
{
[[nodiscard]] bool network_tests_enabled()
{
    const char* const rawValue = std::getenv("IPPONBOARD_ENABLE_NETWORK_TESTS");
    if (rawValue == nullptr || *rawValue == '\0')
    {
        return false;
    }

    std::string value(rawValue);
    std::transform(value.begin(),
                   value.end(),
                   value.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });

    return value != "0" && value != "false" && value != "off" && value != "no";
}

void skip_unless_network_enabled()
{
    if (!network_tests_enabled())
    {
        SKIP("Network tests disabled; set IPPONBOARD_ENABLE_NETWORK_TESTS to enable.");
    }
    else
    {
        INFO("Network tests enabled.");
    }
}
} // namespace

TEST_CASE("[OnlineVersionChecker] get online version document (github json)", "[network]")
{
    skip_unless_network_enabled();
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
            QString::fromStdString(ippn_test_data_string("latest_version.json"))));

    REQUIRE_FALSE(versionInfo.version.isEmpty());
    REQUIRE_FALSE(versionInfo.infoUrl.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_en.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_de.isEmpty());
    REQUIRE_FALSE(versionInfo.downloadUrl.isEmpty());
}

TEST_CASE("[OnlineVersionChecker] parse online version document (github json)", "[network]")
{
    skip_unless_network_enabled();
    ensure_qt_app();

    auto versionInfo = OnlineVersionChecker::parse_version_document(
        OnlineVersionChecker::get_version_document(OnlineVersionChecker::VersionDocumentUrl));

    REQUIRE_FALSE(versionInfo.version.isEmpty());
    REQUIRE_FALSE(versionInfo.infoUrl.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_en.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_de.isEmpty());
    REQUIRE_FALSE(versionInfo.downloadUrl.isEmpty());
}
