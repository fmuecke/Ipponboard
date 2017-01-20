#include "UpdateChecker.h"
#include "versioninfo.h"

#include <QtGui/QApplication>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QXmlQuery>
#include <QUrl>
#include <QDesktopServices>

UpdateChecker::UpdateChecker()
{
}

QString GetXmlValue(QXmlQuery query, QString xPath)
{
    QString value;
    query.setQuery(xPath);
    if (query.isValid())
    {
        query.evaluateTo(&value);
        return value.trimmed();
    }

    return value;
}

bool UpdateChecker::CheckForNewerVersion()
{
    QXmlQuery query;
    try
    {
        query.setFocus(QUrl("http://ipponboard.koe-judo.de/files/current_version.xml"));
    }
    catch (...)
    {
        return false;
    }

    auto version = GetXmlValue(query, "Ipponboard/Version/text()");
    if (!version.isEmpty())
    {
        QStringList onlineVer = version.split('.');
        QStringList buildVer = QString(VersionInfo::VersionStr).split('.');

        while (onlineVer.length() < 3) onlineVer.append("0");
        while (buildVer.length() < 3) buildVer.append("0");

        bool isOldVersion = false;
        if ((onlineVer[0].toInt() > buildVer[0].toInt()))
        {
            isOldVersion = true;
        }
        else if (onlineVer[0].toInt() == buildVer[0].toInt())
        {
            if (onlineVer[1].toInt() > buildVer[1].toInt())
            {
                isOldVersion = true;
            }
            else if ((onlineVer[1].toInt() == buildVer[1].toInt()))
            {
                if ((onlineVer[2].toInt() > buildVer[2].toInt()))
                {
                    isOldVersion = true;
                }
            }
        }

        if (isOldVersion)
        {
            auto lang = QCoreApplication::tr("en");
            lang = lang == "de" ? "de" : "en";
            auto changes = GetXmlValue(query, "Ipponboard/Changes[@lang='" + lang + "']/text()");
            if (!changes.isEmpty())
            {
                changes = QString(": <em>%1</em>").arg(changes);
            }

            QString msg = QString("<p>%1 %2</p>")
                    .arg(
                        QCoreApplication::tr("Version %1 available").arg(
                            QString("<b>%1</b>").arg(version)))
                    .arg(changes);

            auto downloadUrl = GetXmlValue(query, "Ipponboard/DownloadUrl/text()");
            auto infoUrl = GetXmlValue(query, "Ipponboard/InfoUrl/text()");

            msg += QString("<p>%1</p>")
                    .arg(QCoreApplication::tr("Do you want to download it or visit the project homepage?"));

            auto result = QMessageBox::information(
                        nullptr,
                        QCoreApplication::tr("Ipponboard - New Version Available"),
                        msg,
                        QCoreApplication::tr("Download"),
                        QCoreApplication::tr("Visit Homepage"),
                        QCoreApplication::tr("Cancel"),
                        0, 2);

            if (result == 0)
            {
                return QDesktopServices::openUrl(QUrl(downloadUrl));
            }
            else if (result == 1)
            {
                if (!QDesktopServices::openUrl(QUrl(infoUrl)))
                {
                    return QDesktopServices::openUrl(QUrl("http://www.ipponboard.info/"));
                }

                return true;
            }
        }
    }

    return false;
}
