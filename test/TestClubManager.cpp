// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../base/Club.h"
#include "../base/ClubManager.h"
#include "../util/path_helpers.h"
#include "TestQtApp.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMessageLogContext>
#include <QStandardPaths>
#include <catch2/catch_test_macros.hpp>

namespace
{
// used to suppress qInfo/qDebug/QWarn messages during test
class ScopedQtMessageFilter
{
  public:
    ScopedQtMessageFilter()
    {
        s_previousHandler = qInstallMessageHandler(
            [](QtMsgType type, const QMessageLogContext& context, const QString& msg)
            {
                if (msg.contains(QStringLiteral("Reading clubs from config:")) ||
                    msg.contains(QStringLiteral("Unable to load clubs:")))
                {
                    return;
                }

                if (s_previousHandler != nullptr)
                {
                    s_previousHandler(type, context, msg);
                }
                else
                {
                    qt_message_output(type, context, msg);
                }
            });
    }

    ~ScopedQtMessageFilter() { qInstallMessageHandler(s_previousHandler); }

    ScopedQtMessageFilter(ScopedQtMessageFilter const&) = delete;
    ScopedQtMessageFilter& operator=(ScopedQtMessageFilter const&) = delete;

  private:
    static inline QtMessageHandler s_previousHandler = nullptr;
};

void prepareQtApp()
{
    QStandardPaths::setTestModeEnabled(true);
    auto& app = ensure_qt_app();
    app.setApplicationName(QStringLiteral("IpponboardTestApp"));
    app.setApplicationVersion(QStringLiteral("0.1-test"));
}
} // namespace

TEST_CASE("[ClubManager] Relative logos resolve from config dir before user config exists")
{
    prepareQtApp();

    const auto configFilePath =
        fm::GetConfigFilePath(Ipponboard::ClubManager::str_clubs_settingsFile);
    const auto logoRelativePath = QStringLiteral("clubs/test-user-logo.png");
    const auto logoAbsolutePath = QDir(fm::GetConfigDir()).filePath(logoRelativePath);

    QFile::remove(configFilePath);
    QFile::remove(logoAbsolutePath);
    REQUIRE(QDir().mkpath(QFileInfo(logoAbsolutePath).path()));

    QFile configFile(configFilePath);
    REQUIRE(configFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    configFile.close();

    QFile logoFile(logoAbsolutePath);
    REQUIRE(logoFile.open(QIODevice::WriteOnly | QIODevice::Truncate));
    logoFile.write("logo");
    logoFile.close();

    {
        ScopedQtMessageFilter messageFilter;
        Ipponboard::ClubManager clubManager;
        clubManager.AddClub(Ipponboard::Club(QStringLiteral("Test Club"), logoRelativePath));

        REQUIRE(clubManager.GetLogo(QStringLiteral("Test Club")) ==
                QDir::toNativeSeparators(logoAbsolutePath));
    }

    QFile::remove(configFilePath);
    QFile::remove(logoAbsolutePath);
}
