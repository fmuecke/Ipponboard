#include <QtGui/QApplication>
#include <QTranslator>
#include <QMessageBox>
#include <QSettings>
#include <QFile>
#include "../base/mainwindow.h"
#include "../widgets/countdown.h"
#include "../widgets/splashscreen.h"
#include "../base/versioninfo.h"
#include "../util/helpers.h"


int DelayUser()
{
    Countdown dlg(25);
    return dlg.exec();
}

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setApplicationVersion(VersionInfo::VersionStr);
    QCoreApplication::setOrganizationName("Florian Mücke");
    QCoreApplication::setOrganizationDomain("ipponboard.info");
    QCoreApplication::setApplicationName("Ipponboard (Basic Edition)");

    // read language code
    const QString ini(
        QString::fromStdString(fmu::GetSettingsFilePath(str_ini_name)));

    QSettings settings(ini, QSettings::IniFormat, &a);
    settings.beginGroup(str_tag_Main);
    const QString langStr = settings.value(str_tag_Language, "en").toString();
    settings.endGroup();

    QTranslator translator;

    if (langStr != QString("en"))
    {
        const QString& langPath =
            QCoreApplication::applicationDirPath();// + Qtring("/lang");

        const QString langFile =
            QString("lang/ipponboard_") + langStr;

        if (translator.load(langFile, langPath))
        {
            a.installTranslator(&translator);
        }
        else
        {
            QMessageBox::critical(nullptr,
                                  QCoreApplication::applicationName(),
                                  "Unable to read language file: " + langFile +
                                  "\nThe default language is being used.");
        }
    }


    QFile f(langStr == "de" ? ":/text/text/License_de.html" : ":/text/text/License_en.html");
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
    splashData.date = QDate(2012, 6, 30);
    SplashScreen splash(splashData);

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

    if (days_left < 30)
    {
        QMessageBox::warning(0,
                             QCoreApplication::tr("Warning"),
                             QCoreApplication::tr(
                                 "This version will stop to work in less than 30 days!\n\n"
                                 "Please visit the project homepage - there should be a newer version available."));
    }

    MainWindow w;
    w.setWindowTitle(QCoreApplication::applicationName() + " v" +
                     QCoreApplication::applicationVersion());
    w.show();

    return a.exec();
}
