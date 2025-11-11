// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "TestDataPaths.h"

#include <QByteArray>
#include <QFile>
#include <QJsonDocument>
#include <QJsonParseError>
#include <catch2/catch_test_macros.hpp>

namespace
{

[[nodiscard]] QJsonDocument readJsonDocument(const QString& relativePath)
{
    const auto absolutePath = Ipponboard::TestSupport::ResolveTestDataPath(relativePath);
    QFile file(absolutePath);
    REQUIRE(file.open(QIODevice::ReadOnly));

    QJsonParseError parseError{};
    const auto document = QJsonDocument::fromJson(file.readAll(), &parseError);
    REQUIRE(parseError.error == QJsonParseError::NoError);

    return document;
}

} // namespace

TEST_CASE("[json] read file with BOM")
{
    const auto bomDoc = readJsonDocument(QStringLiteral("utf8_with_bom.json"));
    const auto plainDoc = readJsonDocument(QStringLiteral("utf8.json"));

    REQUIRE(bomDoc.toJson(QJsonDocument::Indented) == plainDoc.toJson(QJsonDocument::Indented));
}

TEST_CASE("[json] parse string")
{
    const QByteArray jsonBytes("[{\"string\": \"Florian Mücke\", \"int\": 300, \"double\": 3.1415, "
                               "\"array\": [1,2,3,4,5]}]");

    QJsonParseError parseError{};
    const auto fromString = QJsonDocument::fromJson(jsonBytes, &parseError);
    REQUIRE(parseError.error == QJsonParseError::NoError);

    const auto fromFile = readJsonDocument(QStringLiteral("utf8.json"));

    REQUIRE(fromString.toJson(QJsonDocument::Indented) == fromFile.toJson(QJsonDocument::Indented));
}
