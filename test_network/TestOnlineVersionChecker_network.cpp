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

TEST_CASE("[OnlineVersionChecker] get online version document (github json)", "[network]")
{
    ensure_qt_app();

    const auto versionData =
        OnlineVersionChecker::get_version_document(OnlineVersionChecker::VersionDocumentUrl);

    REQUIRE_FALSE(versionData.isEmpty());
}

TEST_CASE("[OnlineVersionChecker] parse online version document (github json)", "[network]")
{
    ensure_qt_app();

    const auto versionInfo = OnlineVersionChecker::parse_version_document(
        OnlineVersionChecker::get_version_document(OnlineVersionChecker::VersionDocumentUrl));

    REQUIRE_FALSE(versionInfo.version.isEmpty());
    REQUIRE_FALSE(versionInfo.infoUrl.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_en.isEmpty());
    REQUIRE_FALSE(versionInfo.changes_de.isEmpty());
    REQUIRE_FALSE(versionInfo.downloadUrl.isEmpty());
}
