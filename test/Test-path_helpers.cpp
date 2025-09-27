// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../util/path_helpers.h"

#ifndef EXPERIMENTAL

#ifdef _WIN32

TEST_CASE("GetSettingsFilePath: returns back empty filename")
{
    auto path = fm::GetSettingsFilePath("");
    REQUIRE_FALSE(path.isEmpty());
    INFO(path.toStdString());
    REQUIRE(path.indexOf("\\Ipponboard") == path.size() - 11);
}

TEST_CASE("GetSettingsFilePath: returns a non empty value")
{
    auto path = fm::GetSettingsFilePath("somefile.ext");
    REQUIRE_FALSE(path.isEmpty());
    INFO(path.toStdString());
    REQUIRE(path.indexOf("\\Ipponboard\\somefile.ext") != -1);
}

TEST_CASE("KnowFolders: Desktop returns path to desktop")
{
    auto folder = fm::KnownFolders::get_Desktop();
    auto pos = folder.indexOf("Desktop");
    INFO(folder.toStdString());
    REQUIRE(pos == folder.size() - 7);
}

TEST_CASE("KnowFolders: LocalAppData returns valid path ")
{
    auto folder = fm::KnownFolders::get_LocalAppData();
    INFO(folder.toStdString());
    auto pos = folder.indexOf("AppData\\Local");
    REQUIRE(pos != -1);
}

#endif
#endif