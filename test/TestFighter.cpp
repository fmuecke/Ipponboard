// Copyright 2021 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../core/Fighter.h"

#include "../core/Fighter.cpp"

using Ipponboard::Fighter;

static Fighter GetDummy()
{
	Fighter f("first", "last");
	f.weight = "weight";
	f.category = "category";
	f.club = "club";

	return f;
}

TEST_CASE("operator == matches identity")
{
	Fighter f("one", "two");
	REQUIRE(f == f);
}

TEST_CASE("operator == matches copy")
{
	Fighter f1("one", "two");
	auto f2(f1);
	REQUIRE(f1 == f2);
	REQUIRE(GetDummy() == GetDummy());
}

TEST_CASE("Matches matches empty weight")
{
	auto f1 = GetDummy();
	f1.weight = "";
	REQUIRE(f1.Matches(GetDummy()));
	REQUIRE(GetDummy().Matches(f1));
}

TEST_CASE("operator == does not match empty weight")
{
	auto f1 = GetDummy();
	f1.weight = "";
	REQUIRE_FALSE(f1 == GetDummy());
	REQUIRE_FALSE(GetDummy() == f1);
	REQUIRE(f1 != GetDummy());
	REQUIRE(GetDummy() != f1);
}

TEST_CASE("Matches matches empty category")
{
	auto f1 = GetDummy();
	f1.category = "";
	REQUIRE(f1.Matches(GetDummy()));
	REQUIRE(GetDummy().Matches(f1));
}

TEST_CASE("operator == does not match empty category")
{
	auto f1 = GetDummy();
	f1.category = "";
	REQUIRE_FALSE(f1 == GetDummy());
	REQUIRE_FALSE(GetDummy() == f1);
	REQUIRE(f1 != GetDummy());
	REQUIRE(GetDummy() != f1);
}

TEST_CASE("Matches matches empty club")
{
    auto f1 = GetDummy();
    f1.club = "";
    REQUIRE(f1.Matches(GetDummy()));
    REQUIRE(GetDummy().Matches(f1));
}

TEST_CASE("operator == does not match empty club")
{
    auto f1 = GetDummy();
    f1.club = "";
    REQUIRE_FALSE(f1 == GetDummy());
    REQUIRE_FALSE(GetDummy() == f1);
    REQUIRE(f1 != GetDummy());
    REQUIRE(GetDummy() != f1);
}
