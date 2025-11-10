// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../base/OnlineVersionChecker.cpp"
#include "../base/OnlineVersionChecker.h"
#include "TestDataPath.h"
#include "TestQtApp.h"

#include <QCoreApplication>
#include <QString>
#include <catch2/catch_test_macros.hpp>
#include <string>

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
