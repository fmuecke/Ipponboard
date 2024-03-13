// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../util/json.hpp"
#include "../util/jsoncpp/json.cpp"

TEST_CASE("JSON read file")
{
	try
	{
        auto value1 = fm::Json::ReadFile("TestData/utf8_with_bom.json");
        auto value2 = fm::Json::ReadFile("TestData/utf8.json");

		REQUIRE(value1.toStyledString() == value2.toStyledString());
	}
	catch (std::exception const& e)
	{
		REQUIRE(e.what() == 0);
	}
}

TEST_CASE("JSON write file")
{

}

TEST_CASE("JSON read string")
{
	auto str = "[{\"string\": \"Florian Mücke\", \"int\": 300, \"double\": 3.1415, 	\"array\": [1,2,3,4,5] }]";

	try
	{
		auto value1 = fm::Json::ReadString(str);
        auto value2 = fm::Json::ReadFile("TestData/utf8.json");

		REQUIRE(value1.toStyledString() == value2.toStyledString());
	}
	catch (std::exception const& e)
	{
		REQUIRE(e.what() == 0);
	}
}

