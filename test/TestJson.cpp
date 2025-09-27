// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../util/json.hpp"
#include "../util/jsoncpp/json.cpp"
#include "TestDataPath.h"

TEST_CASE("[json] read file")
{
	try
	{
        const auto fileWithBom = ippn_test_data_string("utf8_with_bom.json");
        const auto fileUtf8 = ippn_test_data_string("utf8.json");

        auto value1 = fm::Json::ReadFile(fileWithBom.c_str());
        auto value2 = fm::Json::ReadFile(fileUtf8.c_str());

		REQUIRE(value1.toStyledString() == value2.toStyledString());
	}
	catch (std::exception const& e)
	{
		REQUIRE(e.what() == 0);
	}
}

TEST_CASE("[json] write file")
{

}

TEST_CASE("[json] read string")
{
	auto str = "[{\"string\": \"Florian Mücke\", \"int\": 300, \"double\": 3.1415, 	\"array\": [1,2,3,4,5] }]";

	try
	{
		auto value1 = fm::Json::ReadString(str);
        const auto fileUtf8 = ippn_test_data_string("utf8.json");
        auto value2 = fm::Json::ReadFile(fileUtf8.c_str());

		REQUIRE(value1.toStyledString() == value2.toStyledString());
	}
	catch (std::exception const& e)
	{
		REQUIRE(e.what() == 0);
	}
}
