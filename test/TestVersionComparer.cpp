#include "../util/catch/catch.hpp"
#include "../base/VersionComparer.h"
#include "../base/VersionComparer.cpp"

#include <QString>
#include <iostream>

//using namespace Ipponboard;

// see testcases for semver here: https://github.com/coolaj86/semver-utils/blob/master/test/spec.js


TEST_CASE("Versions_are_equal")
{
	REQUIRE_FALSE(VersionComparer::IsVersionLess(""     , ""));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("1"    , "1"));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("1.0"  , "1.0"));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2.3", "1.2.3"));
}

TEST_CASE("Major_version_is_lower")
{
	REQUIRE(VersionComparer::IsVersionLess("1"      , "2"));
	REQUIRE(VersionComparer::IsVersionLess("1.0"    , "2.0"));
	REQUIRE(VersionComparer::IsVersionLess("1.2"    , "2.0"));
	REQUIRE(VersionComparer::IsVersionLess("1.2.3"  , "2.0"));
	REQUIRE(VersionComparer::IsVersionLess("1.2.3.4", "2.0"));

	REQUIRE_FALSE(VersionComparer::IsVersionLess("2"  , "1"));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("2.0", "1.0"));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("2.0", "1.2"));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("2.0", "1.2.3"));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("2.0", "1.2.3.4"));
}

TEST_CASE("Minor_version_is_lower")
{
	REQUIRE(VersionComparer::IsVersionLess("1.1"    , "1.2"));
	REQUIRE(VersionComparer::IsVersionLess("1.1.0"  , "1.2.0"));
	REQUIRE(VersionComparer::IsVersionLess("1.1.2"  , "1.2.0"));
	REQUIRE(VersionComparer::IsVersionLess("1.1.2.3", "1.2.0"));

	REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2"  , "1.1"));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2.0", "1.1.0"));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2.0", "1.1.2"));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2.0", "1.1.2.3"));
}

TEST_CASE("Patch_level_is_lower")
{
	REQUIRE(VersionComparer::IsVersionLess("1.0.1"    , "1.0.2"));
	REQUIRE(VersionComparer::IsVersionLess("1.0.1.0"  , "1.0.2.0"));
	REQUIRE(VersionComparer::IsVersionLess("1.0.1.2"  , "1.0.2.0"));

	REQUIRE_FALSE(VersionComparer::IsVersionLess("1.0.2"  , "1.0.1"));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("1.0.2.0", "1.0.1.0"));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("1.0.2.0", "1.0.1.2"));
}

TEST_CASE("Prerelease_is_lower")
{
	REQUIRE(VersionComparer::IsVersionLess("1.2.3-beta1", "1.2.3-beta2"));
	REQUIRE(VersionComparer::IsVersionLess("1.2.3-beta", "1.2.3"));
	REQUIRE(VersionComparer::IsVersionLess("1.2.3-beta", "1.2.4"));
	REQUIRE(VersionComparer::IsVersionLess("1.2.3-beta", "1.2.4-beta"));
	REQUIRE_FALSE(VersionComparer::IsVersionLess("1.2.4", "1.2.3-beta"));
}

