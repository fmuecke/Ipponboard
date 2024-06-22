// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../util/SimpleCsvFile.hpp"

namespace csv = fm::SimpleCsvFile;

TEST_CASE("ParseData: empty csv data is invalid")
{
	QByteArray data{ "" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ";", result, errorMsg) == false);
	REQUIRE(errorMsg == "no data");
}

TEST_CASE("ParseData: empty separator is invalid")
{
	QByteArray data{ "asdf" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, "", result, errorMsg) == false);
	REQUIRE(errorMsg == "invalid separator");
}

TEST_CASE("ParseData: single element is valid data")
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

TEST_CASE("ParseData: just a separator equals two values")
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

TEST_CASE("ParseData: trailing separator separates values")
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

TEST_CASE("ParseData: leading separator separates values")
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

TEST_CASE("ParseData: reading two values on separate lines")
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

TEST_CASE("ParseData: fewer values as the first line is invalid")
{
	QByteArray data{ "one,two\nonly one" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == false);
	REQUIRE(errorMsg == "line 2 has fewer values as required by the first line");
	REQUIRE(result.size() == 0);
}

TEST_CASE("ParseData: more values as the first line is invalid")
{
	QByteArray data{ "one,two\none,two,three" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == false);
	REQUIRE(errorMsg == "line 2 has more values as required by the first line");
	REQUIRE(result.size() == 0);
}

TEST_CASE("ParseData: 10 lines with 10 values")
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

TEST_CASE("ParseData: single newline at the end is ignored")
{
	QByteArray data{ "abc,def\none,two\n" };
	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == true);
	REQUIRE(errorMsg == "");
	REQUIRE(result.size() == 2);
	REQUIRE(result[1].at(0) == "one");
}

TEST_CASE("ParseData: UTF8-BOM is ignored")
{
	QByteArray data;
	data += static_cast<char>(0xEF);
	data += static_cast<char>(0xBB);
	data += static_cast<char>(0xBF);
	data += "abc, def\none, two\n";

	std::vector<QStringList> result;
	QString errorMsg;
	REQUIRE(csv::ParseData(data, ",", result, errorMsg) == true);
	REQUIRE(errorMsg == "");
	REQUIRE(result.size() == 2);
	REQUIRE(result[1].at(0) == "one");
}

TEST_CASE("[SimpleCsvFile] EncodeValues: encode in UTF-8")
{
	std::vector<QStringList> data{ QStringList{ "Mücke" } };

	QByteArray resultData;
	QString errorMsg;
	REQUIRE(csv::EncodeValues(data, ",", resultData, errorMsg) == true);
	REQUIRE(errorMsg == "");
	auto expected = QByteArray::fromStdString("Mücke\n");
	REQUIRE(resultData == expected);
}

TEST_CASE("[SimpleCsvFile] EncodeValues: no values is invalid")
{
	QByteArray resultData;
	std::vector<QStringList> values;
	QString errorMsg;
	REQUIRE(csv::EncodeValues(values, ",", resultData, errorMsg) == false);
	REQUIRE(errorMsg == "no values");
}

TEST_CASE("[SimpleCsvFile] EncodeValues: empty value is invalid")
{
	QByteArray resultData;
	std::vector<QStringList> values;
	values.push_back(QStringList());
	QString errorMsg;
	REQUIRE(csv::EncodeValues(values, ",", resultData, errorMsg) == false);
	REQUIRE(errorMsg == "no values");
}

TEST_CASE("[SimpleCsvFile] EncodeValues: value containing empty value is invalid")
{
	QByteArray resultData;
	std::vector<QStringList> values;
	values.push_back(QStringList{""});
	QString errorMsg;
	REQUIRE(csv::EncodeValues(values, ",", resultData, errorMsg) == false);
	REQUIRE(errorMsg == "no values");
}

TEST_CASE("[SimpleCsvFile] EncodeValues: single value")
{
	QByteArray resultData;
	std::vector<QStringList> values{ QStringList{ "one" } };
	QString errorMsg;
	REQUIRE(csv::EncodeValues(values, ",", resultData, errorMsg) == true);
	REQUIRE(errorMsg == "");
	QByteArray const expectedData{ "one\n" };
	REQUIRE(expectedData == resultData);
}

TEST_CASE("[SimpleCsvFile] EncodeValues: items must have same number of values")
{
	QStringList firstItem{ "one" };
	QStringList secondItem{ "one" };
	secondItem.append("two");
	std::vector<QStringList> moreValues{ firstItem, secondItem };

	QByteArray resultData;
	QString errorMsg;
	REQUIRE(csv::EncodeValues(moreValues, ",", resultData, errorMsg) == false);
	REQUIRE(errorMsg == "item at index 1 has more values as required by the first item");

	std::vector<QStringList> fewerValues{ secondItem, firstItem };
	REQUIRE(csv::EncodeValues(fewerValues, ",", resultData, errorMsg) == false);
	REQUIRE(errorMsg == "item at index 1 has fewer values as required by the first item");
}

TEST_CASE("[SimpleCsvFile] EncodeValues: valid data case")
{
	QByteArray const expectedData{ "abc,def\none,two\n" };
	
	QByteArray resultData;
	std::vector<QStringList> values;
	QString errorMsg;
	REQUIRE(csv::ParseData(expectedData, ",", values, errorMsg) == true);
	REQUIRE(csv::EncodeValues(values, ",", resultData, errorMsg) == true);
	REQUIRE(errorMsg == "");
	REQUIRE(expectedData.length() == resultData.length());
	REQUIRE(expectedData == resultData);
}

TEST_CASE("[SimpleCsvFile] values containing separator are not allowed")
{
	std::vector<QStringList> values{ QStringList{"a,s"} };

	QByteArray resultData;
	QString errorMsg;
	REQUIRE(csv::EncodeValues(values, ",", resultData, errorMsg) == false);
	REQUIRE(errorMsg == "not supported: item at index 0 contains separator");

	errorMsg.clear();
	REQUIRE(csv::EncodeValues(values, ";", resultData, errorMsg) == true);
	REQUIRE(errorMsg == "");
}

TEST_CASE("[SimpleCsvFile] Write: writes utf-8 byte order mark (important for Excel)")
{
	QString const fileName{ "test.csv" };
	std::vector<QStringList> data{ QStringList{ "one" } };
	QString errorMsg;
	REQUIRE(csv::Write(fileName, data, ";", errorMsg) == true);
	REQUIRE(errorMsg.isEmpty() == true);

	QFile file(fileName);
	REQUIRE(file.open(QIODevice::ReadOnly));
	QByteArray const bom{ file.read(3) };
	file.close();
	REQUIRE((bom[0] == '\xEF' && bom[1] == '\xBB' && bom[2] == '\xBF'));
}
