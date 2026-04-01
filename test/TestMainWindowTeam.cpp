// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../base/MainWindowTeam.h"
#include "TestQtApp.h"

#include <QStandardPaths>
#include <QString>
#include <catch2/catch_test_macros.hpp>

struct MainWindowTeamTest
{
    static QString qualifyTemplateReference(QString const& templateReference)
    {
        return MainWindowTeam::qualify_template_reference(templateReference);
    }
};

TEST_CASE("[MainWindowTeam] Template references preserve explicit paths")
{
    QStandardPaths::setTestModeEnabled(true);
    auto& app = ensure_qt_app();
    app.setApplicationName(QStringLiteral("IpponboardTestApp"));
    app.setApplicationVersion(QStringLiteral("0.1-test"));

    REQUIRE(MainWindowTeamTest::qualifyTemplateReference(QStringLiteral("list_output_bay.html")) ==
            QStringLiteral("templates/list_output_bay.html"));
    REQUIRE(MainWindowTeamTest::qualifyTemplateReference(QStringLiteral("../custom/output.html")) ==
            QStringLiteral("../custom/output.html"));
    REQUIRE(MainWindowTeamTest::qualifyTemplateReference(QStringLiteral(
                ":/templates/resource.html")) == QStringLiteral(":/templates/resource.html"));
}
