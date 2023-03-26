// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../util/SimpleCsvFile.hpp"

namespace csv = fm::SimpleCsvFile;

TEST_CASE("empty csv data is invalid")
{
	QByteArray data{ "" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ";", result, errorMsg) == false);
	REQUIRE(errorMsg == "no data");
}

TEST_CASE("empty separator is invalid")
{
	QByteArray data{ "asdf" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, "", result, errorMsg) == false);
	REQUIRE(errorMsg == "invalid separator");
}

TEST_CASE("single element is valid data")
{
	QByteArray data{ "foobar" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == true);
	REQUIRE(errorMsg.isEmpty() == true);
	REQUIRE(result.size() == 1);
	REQUIRE(result[0].length() == 1);
	REQUIRE(result[0].at(0) == "foobar");
}

TEST_CASE("just a separator equals two values")
{
	QByteArray data{ "," };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == true);
	REQUIRE(errorMsg.isEmpty() == true);
	REQUIRE(result.size() == 1);
	REQUIRE(result[0].length() == 2);
	REQUIRE(result[0].at(0) == "");
	REQUIRE(result[0].at(1) == "");
}

TEST_CASE("trailing separator separates values")
{
	QByteArray data{ "asdf," };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == true);
	REQUIRE(errorMsg.isEmpty() == true);
	REQUIRE(result.size() == 1);
	REQUIRE(result[0].length() == 2);
	REQUIRE(result[0].at(0) == "asdf");
	REQUIRE(result[0].at(1) == "");
}

TEST_CASE("leading separator separates values")
{
	QByteArray data{ ",asdf" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == true);
	REQUIRE(errorMsg.isEmpty() == true);
	REQUIRE(result.size() == 1);
	REQUIRE(result[0].length() == 2);
	REQUIRE(result[0].at(0) == "");
	REQUIRE(result[0].at(1) == "asdf");
}

TEST_CASE("reading two values on separate lines")
{
	QByteArray data{ "one\ntwo" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == true);
	REQUIRE(errorMsg.isEmpty() == true);
	REQUIRE(result.size() == 2);
	REQUIRE(result[0].length() == 1);
	REQUIRE(result[1].length() == 1);
	REQUIRE(result[0].at(0) == "one");
	REQUIRE(result[1].at(0) == "two");
}

TEST_CASE("fewer values as the first line is invalid")
{
	QByteArray data{ "one,two\nonly one" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == false);
	REQUIRE(errorMsg == "line 2 has fewer values as required by the first line");
	REQUIRE(result.size() == 0);
}

TEST_CASE("more values as the first line is invalid")
{
	QByteArray data{ "one,two\none,two,three" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == false);
	REQUIRE(errorMsg == "line 2 has more values as required by the first line");
	REQUIRE(result.size() == 0);
}

TEST_CASE("10 lines with 10 values")
{
	QByteArray data{ ",,,,,,,,,\n,,,,,,,,,\n,,,,,,,,,\n,,,,,,,,,\n,,,,,,,,,\n,,,,,,,,,\n,,,,,,,,,\n,,,,,,,,,\n,,,,,,,,,\n,,,,,,,,," };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == true);
	REQUIRE(errorMsg == "");
	REQUIRE(result.size() == 10);
	for (auto line : result)
	{
		REQUIRE(line.length() == 10);
	}
}

TEST_CASE("single newline at the end is ignored")
{
	QByteArray data{ "abc,def\none,two\n" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == true);
	REQUIRE(errorMsg == "");
	REQUIRE(result.size() == 2);
}

TEST_CASE("multiple newlines at the end are invalid")
{
	QByteArray data{ "abc,def\none,two\n\n" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == false);
	REQUIRE(errorMsg != "");
	REQUIRE(result.empty());
}