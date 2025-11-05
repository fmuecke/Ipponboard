// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../base/VersionComparer.cpp"
#include "../base/VersionComparer.h"
#include "../util/catch2/catch_amalgamated.hpp"

#include <QString>
#include <iostream>

//using namespace Ipponboard;

// see testcases for semver here: https://github.com/coolaj86/semver-utils/blob/master/test/spec.js

TEST_CASE("[VersionComparer] Versions_are_equal")
{
    REQUIRE_FALSE(VersionComparer::IsVersionLess("", ""));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("1", "1"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.0", "1.0"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2.3", "1.2.3"));
}

TEST_CASE("[VersionComparer] Major_version_is_lower")
{
    REQUIRE(VersionComparer::IsVersionLess("1", "2"));
    REQUIRE(VersionComparer::IsVersionLess("1.0", "2.0"));
    REQUIRE(VersionComparer::IsVersionLess("1.2", "2.0"));
    REQUIRE(VersionComparer::IsVersionLess("1.2.3", "2.0"));
    REQUIRE(VersionComparer::IsVersionLess("1.2.3.4", "2.0"));

    REQUIRE_FALSE(VersionComparer::IsVersionLess("2", "1"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("2.0", "1.0"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("2.0", "1.2"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("2.0", "1.2.3"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("2.0", "1.2.3.4"));
}

TEST_CASE("[VersionComparer] Minor_version_is_lower")
{
    REQUIRE(VersionComparer::IsVersionLess("1.1", "1.2"));
    REQUIRE(VersionComparer::IsVersionLess("1.1.0", "1.2.0"));
    REQUIRE(VersionComparer::IsVersionLess("1.1.2", "1.2.0"));
    REQUIRE(VersionComparer::IsVersionLess("1.1.2.3", "1.2.0"));

    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2", "1.1"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2.0", "1.1.0"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2.0", "1.1.2"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2.0", "1.1.2.3"));
}

TEST_CASE("[VersionComparer] Patch_level_is_lower")
{
    REQUIRE(VersionComparer::IsVersionLess("1.0.1", "1.0.2"));
    REQUIRE(VersionComparer::IsVersionLess("1.0.1.0", "1.0.2.0"));
    REQUIRE(VersionComparer::IsVersionLess("1.0.1.2", "1.0.2.0"));

    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.0.2", "1.0.1"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.0.2.0", "1.0.1.0"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.0.2.0", "1.0.1.2"));
}

TEST_CASE("[VersionComparer] Prerelease_is_lower")
{
    REQUIRE(VersionComparer::IsVersionLess("1.2.3-beta1", "1.2.3-beta2"));
    REQUIRE(VersionComparer::IsVersionLess("1.2.3-beta", "1.2.3"));
    REQUIRE(VersionComparer::IsVersionLess("1.2.3-beta", "1.2.4"));
    REQUIRE(VersionComparer::IsVersionLess("1.2.3-beta", "1.2.4-beta"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2.4", "1.2.3-beta"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2.3-beta2", "1.2.3-beta1"));
    REQUIRE(VersionComparer::IsVersionLess("1.2.3", "1.2.4-beta"));
}

TEST_CASE("[VersionComparer] Version_may_start_with_v")
{
    REQUIRE(VersionComparer::IsVersionLess("v1.0.0", "1.1"));
    REQUIRE(VersionComparer::IsVersionLess("1.0.0", "v1.1"));

    REQUIRE_FALSE(VersionComparer::IsVersionLess("v1.2.3", "v1.2.3"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2.3", "v1.2.3"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("v1.2.3", "1.2.3"));
}

TEST_CASE("[VersionComparer] Version_1_2_is_lower_than_1_10")
{
    REQUIRE_FALSE(VersionComparer::IsVersionLess("v1.10", "1.2.0"));
}

TEST_CASE("[VersionComparer] minor version has prerelease tag")
{
    REQUIRE(VersionComparer::IsVersionLess("1.10-beta", "1.10"));
    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.10", "1.10-beta"));
    REQUIRE(VersionComparer::IsVersionLess("1.10-beta", "1.10.0"));
    REQUIRE(VersionComparer::IsVersionLess("1.10-beta", "1.10.1"));

    REQUIRE_FALSE(VersionComparer::IsVersionLess("1.10-beta", "1.2"));
}