// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "MainWindow.h"
#include "MainWindowTeam.h"
//#include "../core/TournamentMode.h"
#include "../util/path_helpers.h"
#include "SplashScreen.h"
#include "versioninfo.h"

#include <QApplication>
#include <QFile>
#include <QLocale>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QSettings>
#include <QTranslator>
//#include <QtextCodec>
#include <QCommandLineParser>
#include <QDebug>
#include <QTextStream>

#ifndef NO_ERROR
#define NO_ERROR 0
#endif
#ifndef ERROR_INVALID_PARAMETER
#define ERROR_INVALID_PARAMETER 1
#endif

namespace
{
QString logFilePath()
{
    return fm::GetAppConfigFilePath(QCoreApplication::applicationName() + ".log");
}
} // namespace

void LangNotFound(const QString& fileName)
{
    QMessageBox::critical(nullptr,
                          QCoreApplication::applicationName(),
                          "Unable to read language file: " + fileName +
                              "\nThe default language is being used.");
}

void SetTranslation(QApplication& app, QTranslator& translator, QString const& langStr)
{
    Q_UNUSED(app);

    if (langStr == QString("en"))
    {
        return; // default
    }

    if (langStr == QString("de") || langStr == QString("nl"))
    {
        const QString& langPath = QCoreApplication::applicationDirPath() + QString("/lang");

        if (translator.load(langStr, langPath))
        {
            app.installTranslator(&translator);
        }
        else
        {
            LangNotFound(langStr);
        }
    }
}

int ShowSplashScreen()
{
    /*
Ipponboard – the judo scoreboard by judoka for judoka

Judo is part of our lives. That is why Ipponboard was created by judoka for judoka.
It supports timekeepers, coaches, competitors, and the audience — clearly readable and easy to use.
Today Ipponboard is used by clubs and organizers around the world.

If you like Ipponboard, please support its development:

• Feedback and suggestions
• Report issues
• A donation to support the project

More information on GitHub.
*/

    auto title = QCoreApplication::tr("the 🥋 Judo scoreboard");
    auto story = QCoreApplication::tr(
        "Developed by judoka for judoka.<br/>"
        "Clear and easy to use — for timekeepers, coaches, competitors, and the audience.");
    auto callForSupprt = QCoreApplication::tr("Support the project:");
    auto howToSupport = QCoreApplication::tr("Feedback · Report issues · Donations");
    auto github = QCoreApplication::tr("More on %1.")
                      .arg("<a href=\"https://github.com/fmuecke/Ipponboard\">GitHub</a>");

    auto text = QString("<html><body style=\"margin: 10;text-align: left;\"><p><br/><big><span "
                        "style=\"color:#336699;font-weight:bold\"><em>Ipponboard</em></"
                        "span> - <b>%1</b></big></p>")
                    .arg(title);
    text += QString("<p>%1</p>").arg(story);
    text += QString("<p><b>%1</b><br/>%2</p>").arg(callForSupprt, howToSupport);
    text += QString("<p>%1</p></body></html>").arg(github);

    SplashScreen::Data splashData;
    splashData.text = text;
    splashData.info = QCoreApplication::applicationName() + " v" +
                      QCoreApplication::applicationVersion() + "\n" + "Build: " + VersionInfo::Date;
    SplashScreen splash(splashData);
    //splash.SetImageStyleSheet("image: url(:/res/images/logo_team.png);");
    //splash.resize(480, 410);

    auto dlgResult = splash.exec();

    if (QDialog::Rejected == dlgResult)
    {
        return 0;
    }

    return dlgResult;
}

void CustomMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    Q_UNUSED(context)
    QString logLevel;
    switch (type)
    {
    case QtDebugMsg:
        logLevel = "DBUG";
        break;
    case QtInfoMsg:
        logLevel = "INFO";
        break;
    case QtWarningMsg:
        logLevel = "WARN";
        break;
    case QtCriticalMsg:
        logLevel = "CRIT";
        break;
    case QtFatalMsg:
        logLevel = "FATL";
        break;
    }

    QString logMsg = QString("%1 %2 %3")
                         .arg(QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss.zzz"))
                         .arg(logLevel)
                         .arg(msg);

    QFile logFile(logFilePath());
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append))
    {
        QTextStream out(&logFile);
        out << logMsg << Qt::endl;
    }
}

int main(int argc, char* argv[])
{
#if defined(Q_OS_LINUX)
    // Force XCB only when no platform plugin was chosen already. Wayland or other
    // backends can be provided via the environment; respect caller choice.
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM"))
    {
        qputenv("QT_QPA_PLATFORM", QByteArrayLiteral("xcb"));
    }
#endif
    QApplication a(argc, argv);

    QCoreApplication::setApplicationVersion(VersionInfo::VersionStr);
    QCoreApplication::setOrganizationName(QString());
    QCoreApplication::setOrganizationDomain(QString());
    QCoreApplication::setApplicationName("Ipponboard");

    // Open the log file and truncate existing content
    QFile logFile(logFilePath());
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        logFile.close(); // Close file after truncating
    }

    qInstallMessageHandler(CustomMessageHandler);

    qInfo() << QCoreApplication::applicationName() << QCoreApplication::applicationVersion();

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption modeOption(
        QStringList() << "mode", "Selects starting mode: single|team|ask.", "mode", "ask");
    parser.addOption(modeOption);
    parser.process(a);
    QString mode = parser.value(modeOption);
    QStringList validModes{ "single", "team", "ask" };
    if (!validModes.contains(mode))
    {
        parser.showHelp(ERROR_INVALID_PARAMETER);
    }

    // read language and theme settings
    QString langStr = QLocale::system().name();
    langStr.truncate(langStr.lastIndexOf('_'));
    auto theme = Qt::ColorScheme::Unknown;

    auto settingsFile = fm::ResolveConfigFileForRead(MainWindowBase::GetConfigFileName());
    qInfo() << "Reading settings from: " << settingsFile;

    if (!QFile::exists(settingsFile))
    {
        qWarning() << "Unable to load config:" << settingsFile;
    }

    QSettings settings(settingsFile, QSettings::IniFormat, &a);
    settings.beginGroup(str_tag_Main);

    if (settings.contains(str_tag_Language))
    {
        langStr = settings.value(str_tag_Language).toString();
    }

    if (settings.contains(str_tag_Theme))
    {
        auto val = settings.value(str_tag_Theme).toInt();
        if (val == 1 || val == 2)
        {
            theme = static_cast<Qt::ColorScheme>(val);
        }
    }

    settings.endGroup();

    a.styleHints()->setColorScheme(theme);

    QTranslator translator; // Note: this object needs to remain in scope.
    SetTranslation(a, translator, langStr);

    int dlgResult{ 0 };

    if (mode == "ask")
    {
        dlgResult = ShowSplashScreen();
    }
    else if (mode == "single")
    {
        dlgResult = QDialog::Accepted;
    }
    else if (mode == "team")
    {
        dlgResult = QDialog::Accepted + 1;
    }
    if (dlgResult == 0)
    {
        return NO_ERROR;
    }

    std::unique_ptr<MainWindowBase> pMainWnd{ nullptr };

    if (dlgResult == QDialog::Accepted + 1)
    {
        pMainWnd = std::make_unique<MainWindowTeam>();
    }
    else
    {
        pMainWnd = std::make_unique<MainWindow>();
    }

    try
    {
        pMainWnd->Init();
    }
    catch (std::exception const&)
    {
        return NO_ERROR;
    }

    pMainWnd->show();

    return a.exec();
}
