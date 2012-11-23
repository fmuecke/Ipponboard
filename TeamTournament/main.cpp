#include <QtGui/QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include "../base/mainwindow.h"
#include "../widgets/countdown.h"
#include "../widgets/splashscreen.h"
#include "../base/versioninfo.h"
#include "../util/path_helpers.h"


int DelayUser()
{
    Countdown dlg(25);
    return dlg.exec();
}

void LangNotFound(const QString& fileName)
{
    QMessageBox::critical(nullptr,
                          QCoreApplication::applicationName(),
                          "Unable to read language file: " + fileName +
                          "\nThe default language is being used.");
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationVersion(VersionInfo::VersionStr);
    QCoreApplication::setOrganizationName("Florian Mücke");
    QCoreApplication::setOrganizationDomain("ipponboard.info");
    QCoreApplication::setApplicationName("Ipponboard (Team Edition)");

    MainWindow mainWnd;
    mainWnd.setWindowTitle(
                QCoreApplication::applicationName() + " v" +
                QCoreApplication::applicationVersion());

    // read language code
    QString langStr = QLocale::system().name();
    langStr.truncate(langStr.lastIndexOf('_'));

    const QString ini(QString::fromStdString(
                          fmu::GetSettingsFilePath(mainWnd.GetConfigFileName())));
    QSettings settings(ini, QSettings::IniFormat, &a);
    settings.beginGroup(str_tag_Main);

    if (settings.contains(str_tag_Language))
        langStr = settings.value(str_tag_Language).toString();

    settings.endGroup();

    QTranslator translator;  // Note: this object needs to stay.
    QTranslator coreTranslator; // Note: this object needs to stay.
    if (langStr == QString("de"))
    {
        const QString& langPath =
            QCoreApplication::applicationDirPath() + QString("/lang");

        if (translator.load("Ipponboard_team_de", langPath))
            a.installTranslator(&translator);
        else
            LangNotFound("Ipponboard_team_de");

        if (coreTranslator.load("core_de", langPath))
            a.installTranslator(&coreTranslator);
        else
            LangNotFound("core_de");
    }


    //QFile f(langStr == "de" ? ":/text/text/License_team_de.html" : ":/text/text/License_team_en.html");
    QFile f(":/text/text/License_team_de.html");
    f.open(QIODevice::ReadOnly | QIODevice::Text);
    const QByteArray data = f.readAll();
    f.close();
    const QString text = QTextCodec::codecForHtml(data)->toUnicode(data);

    SplashScreen::Data splashData;
    splashData.text = text;
    splashData.info = QCoreApplication::applicationName()
                      + " v" + QCoreApplication::applicationVersion()
                      + "\n"
                      + "Build: " + VersionInfo::Date;
    splashData.date = QDate(2012, 12, 31);
    SplashScreen splash(splashData);
    splash.SetImageStyleSheet("image: url(:/res/images/logo_team.png);");
    splash.resize(480, 410);

    if (QDialog::Accepted != splash.exec())
    return 0;

    const int days_left = QDate::currentDate().daysTo(splashData.date);

    if (days_left <= 0)
    {
        QMessageBox::critical(0,
                              QCoreApplication::tr("Warning"),
                              QCoreApplication::tr(
                                  "This version is no longer valid!\n\n"
                                  "You need to visit the project homepage for a (free) update."));

        if (QDialog::Accepted != DelayUser())
            return 0;
    }
    else if (days_left < 30)
    {
        QMessageBox::warning(0,
                             QCoreApplication::tr("Warning"),
                             QCoreApplication::tr(
                                 "This version will stop to work in less than 30 days!\n\n"
                                 "Please visit the project homepage - there should be a newer version available."));
    }

    //w.setWindowTitle(QCoreApplication::applicationName() + " v" +
    //				 QCoreApplication::applicationVersion() +
    //" ***Spezialversion DJK ITSV Grosshadern***");
    //				 " ***Spezialversion DJK Ingolstadt***");

    mainWnd.Init();
	mainWnd.show();

    return a.exec();
}
