// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../util/path_helpers.h"

TEST_CASE("GetSettingsFilePath: returns back empty filename")
{
	auto path = fm::GetSettingsFilePath("");
	REQUIRE_FALSE(path.empty());
	REQUIRE(path.find("\\Ipponboard\\") == path.size() - 12);
}

TEST_CASE("GetSettingsFilePath: returns a non empty value")
{
	auto path = fm::GetSettingsFilePath("somefile.ext");
	REQUIRE_FALSE(path.empty());
	REQUIRE(path.find("\\Ipponboard\\somefile.ext") != std::string::npos );
}

TEST_CASE("KnowFolders: Desktop returns path to desktop")
{
	auto folder = fm::KnownFolders::get_Desktop();
	auto pos = folder.find("Desktop");
	REQUIRE(pos == folder.size() - 7);
}


TEST_CASE("KnowFolders: LocalAppData returns valid path ")
{
	auto folder = fm::KnownFolders::get_LocalAppData();
	auto pos = folder.find("AppData\\Local");
	REQUIRE(pos != folder.npos);
}
