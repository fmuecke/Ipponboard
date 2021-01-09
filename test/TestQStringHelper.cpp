// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../util/qstring_helper.h"

TEST_CASE("NormalizeSpaces converts underscores to a single space")
{
	REQUIRE(fm::NormalizeSpaces(QString("_")).toStdString() == "");
	REQUIRE(fm::NormalizeSpaces(QString("__")).toStdString() == "");
	REQUIRE(fm::NormalizeSpaces(QString("_ _")).toStdString() == "");
	REQUIRE(fm::NormalizeSpaces(QString("a_")).toStdString() == "a");
	REQUIRE(fm::NormalizeSpaces(QString("_a")).toStdString() == "a");
	REQUIRE(fm::NormalizeSpaces(QString("a_a")).toStdString() == "a a");
	REQUIRE(fm::NormalizeSpaces(QString("_a_a")).toStdString() == "a a");
	REQUIRE(fm::NormalizeSpaces(QString("a_a_")).toStdString() == "a a");
	REQUIRE(fm::NormalizeSpaces(QString("_a_a_")).toStdString() == "a a");
	REQUIRE(fm::NormalizeSpaces(QString("____a____a____")).toStdString() == "a a");
}

