#include "../../../../devtools/Catch/single_include/catch.hpp"
#include "../util/toml.h"
#include <sstream>

TEST_CASE("utf8 TOML files can be read")
{
	auto result1 = toml::parseFile("TestData\\utf8.toml");
	if (!result1.valid()) FAIL(result1.errorReason);

	auto result2 = toml::parseFile("TestData\\utf8_with_bom.toml");
	if (!result2.valid()) FAIL(result2.errorReason);

	REQUIRE(result1.value == result2.value);
}

TEST_CASE("parse TOML string")
{
	auto r1 = toml::parseFile("TestData\\utf8.toml");
	if (!r1.valid()) FAIL(r1.errorReason);

	auto str = "[TestData]\nstring = \"Florian Mücke\"\nint = 300\ndouble = 3.1415\narray = [1,2,3,4,5]\n[TestData2]\nstring = \"Ipponboard\"";
	std::stringstream ss(str);
	auto r2 = toml::parse(ss);
	if (!r2.valid()) FAIL(r2.errorReason);

	REQUIRE(r1.value == r2.value);

	auto testData = r2.value.find("TestData");
	REQUIRE(testData);
	REQUIRE(testData->valid());

	auto str1 = testData->find("string");
	REQUIRE(str1);
	REQUIRE(str1->valid());
	REQUIRE(str1->as<std::string>() == "Florian Mücke");

	auto testData2 = r2.value.find("TestData2");
	REQUIRE(testData2);
	REQUIRE(testData2->valid());
	auto str2 = testData2->find("string");
	REQUIRE(str2);
	REQUIRE(str2->valid());
	REQUIRE(str2->as<std::string>() == "Ipponboard");
}
