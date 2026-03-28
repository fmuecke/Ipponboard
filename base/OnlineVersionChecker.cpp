// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "OnlineVersionChecker.h"

#include "VersionComparer.h"
#include "versioninfo.h"

//#include <QCoreApplication>
//#include <QMessageBox>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QScopedPointer>
#include <QString>
#include <QStringList>

const QString OnlineVersionChecker::VersionDocumentUrl =
    "https://api.github.com/repos/fmuecke/Ipponboard/releases/latest";
const QString OnlineVersionChecker::ProjectReleasesUrl =
    "https://github.com/fmuecke/Ipponboard/releases";

namespace
{

[[nodiscard]] QString selectPreferredWin32AssetUrl(const QJsonArray& assets)
{
    QString archiveUrl;

    for (const auto& assetValue : assets)
    {
        if (!assetValue.isObject())
        {
            continue;
        }

        const auto asset = assetValue.toObject();
        const auto name = asset.value(QStringLiteral("name")).toString();
        const auto url = asset.value(QStringLiteral("browser_download_url")).toString();
        const auto contentType = asset.value(QStringLiteral("content_type")).toString();

        if (url.isEmpty())
        {
            continue;
        }

        const auto lowerName = name.toLower();
        const bool isExe = lowerName.endsWith(QStringLiteral(".exe")) ||
                           contentType == QStringLiteral("application/x-msdownload");
        const bool isWinArchive = lowerName.contains(QStringLiteral("win32")) &&
                                  lowerName.endsWith(QStringLiteral(".7z"));

        if (isExe)
        {
            return url;
        }

        if (isWinArchive && archiveUrl.isEmpty())
        {
            archiveUrl = url;
        }
    }

    return archiveUrl;
}

} // namespace

OnlineVersionChecker::OnlineVersionChecker() {}

namespace
{
QNetworkAccessManager& sharedNetworkAccessManager()
{
    static QNetworkAccessManager* manager = []
    {
        auto* instance = new QNetworkAccessManager();
        instance->setAutoDeleteReplies(false);
        return instance;
    }();
    return *manager;
}

OnlineVersionChecker::OnlineVersion FinalizeOnlineVersion(QString jsonDocument)
{
    OnlineVersionChecker::OnlineVersion onlineVersion;
    if (jsonDocument.isEmpty())
    {
        return onlineVersion;
    }

    onlineVersion = OnlineVersionChecker::parse_version_document(jsonDocument);
    if (onlineVersion.version.isEmpty())
    {
        return onlineVersion;
    }

    qDebug() << "online version string:" << onlineVersion.version
             << "( current:" << QString(VersionInfo::VersionStr) << ")";

    //#if _DEBUG
    //    qDebug() << "in debug there is always a newer version!";
    //    if (true)
    //#else
    if (VersionComparer::IsVersionLess(VersionInfo::VersionStr,
                                       onlineVersion.version.toStdString()))
    //#endif
    {
        qInfo() << "newer version" << onlineVersion.version << "available";
        onlineVersion.state = OnlineVersionChecker::State::NewerAvailable;
    }
    else if (VersionComparer::IsVersionLess(onlineVersion.version.toStdString(),
                                            VersionInfo::VersionStr))
    {
        qInfo() << "current version" << VersionInfo::VersionStr << "is newer than online version"
                << onlineVersion.version;
        onlineVersion.state = OnlineVersionChecker::State::NewerThanOnlineAvailable;
    }
    else
    {
        qInfo() << "no newer version available";
        onlineVersion.state = OnlineVersionChecker::State::UpToDate;
    }

    return onlineVersion;
}
} // namespace

QString GetJsonValue(QJsonObject obj, QString path)
{
    QStringList pathList = path.split(".");
    QString value;

    for (QString element : pathList)
    {
        QString key = element;
        if (element.contains('[')) // object or array
        {
            key = element.left(element.indexOf('['));
        }

        if (obj.contains(key))
        {
            QJsonValue jsonValue = obj.value(key);
            if (jsonValue.isObject())
            {
                obj = jsonValue.toObject();
            }
            else if (jsonValue.isArray())
            {
                QJsonArray jsonArray = jsonValue.toArray();
                QString arrayIndex = element.mid(element.indexOf('[') + 1,
                                                 element.indexOf(']') - element.indexOf('[') - 1);
                int index = arrayIndex.toInt();
                if (index >= 0 && index < jsonArray.size())
                {
                    obj = jsonArray.at(index).toObject();
                }
                else
                {
                    qDebug() << "Invalid array access:" << element;
                    break;
                }
            }
            else if (jsonValue.isString())
            {
                value = jsonValue.toString();
            }
            else if (jsonValue.isDouble())
            {
                // TODO: check if this is correct
                value = QString::number(jsonValue.toDouble());
            }
            else
            {
                qDebug() << "Invalid type:" << element;
                break;
            }
        }
        else
        {
            qDebug() << "Key not found:" << element;
            break;
        }
    }

    return value;
}

QString OnlineVersionChecker::get_version_document(QString url)
{
    // if path does not start with http, assume it is a local file. This is for testing purposes
    if (!url.startsWith("http"))
    {
        QFile file(url);
        if (file.open(QIODevice::ReadOnly))
        {
            QTextStream in(&file);
            return in.readAll();
        }
        else
        {
            qDebug() << "Error opening JSON document:" << file.errorString();
        }
    }

    QNetworkAccessManager& manager = sharedNetworkAccessManager();
    QNetworkRequest request(url);

    QNetworkReply* const rawReply = manager.get(request);
    QScopedPointer<QNetworkReply> reply(rawReply);
    // Detach from manager so scoped pointer owns lifetime and avoids double deletes.
    rawReply->setParent(nullptr);

    QEventLoop loop;
    QObject::connect(rawReply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QString jsonDocument;
    if (reply->error() == QNetworkReply::NoError)
    {
        jsonDocument = reply->readAll();
    }
    else
    {
        qDebug() << "Error retrieving JSON document:" << reply->errorString();
    }

    return jsonDocument;
}

OnlineVersionChecker::OnlineVersion OnlineVersionChecker::parse_version_document(QString jsonDoc)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonDoc.toUtf8());
    QJsonObject json = doc.object();

    OnlineVersion versionInfo;
    versionInfo.version = GetJsonValue(json, "name");
    QString releasePage = GetJsonValue(json, "html_url");
    if (releasePage.isEmpty())
    {
        releasePage = ProjectReleasesUrl;
    }

    versionInfo.infoUrl = releasePage;

#if defined(Q_OS_WIN)
    versionInfo.downloadUrl = releasePage;

    if (json.contains("assets"))
    {
        const auto assets = json.value("assets").toArray();
        const auto preferredAsset = selectPreferredWin32AssetUrl(assets);
        if (!preferredAsset.isEmpty())
        {
            versionInfo.downloadUrl = preferredAsset;
        }
    }
#else
    versionInfo.downloadUrl = releasePage;
#endif

    versionInfo.changes_en = GetJsonValue(json, "body");
    versionInfo.changes_de = versionInfo.changes_en;
    return versionInfo;
}

OnlineVersionChecker::OnlineVersion OnlineVersionChecker::CheckOnlineVersion()
{
    qInfo() << "Checking for updated version...";

    QElapsedTimer timer;
    timer.start();

    const auto onlineVersion = FinalizeOnlineVersion(get_version_document(VersionDocumentUrl));
    if (onlineVersion.version.isEmpty())
    {
        qDebug() << "checking took:" << timer.elapsed() << "ms (empty document)";
        return onlineVersion;
    }

    qDebug() << "checking took " << timer.elapsed() << "ms";
    return onlineVersion;
}

void OnlineVersionChecker::CheckOnlineVersionAsync(QObject* context, VersionCheckCallback callback)
{
    if (context == nullptr || !callback)
    {
        return;
    }

    qInfo() << "Checking for updated version...";

    QNetworkAccessManager& manager = sharedNetworkAccessManager();
    QNetworkRequest request(VersionDocumentUrl);
    QNetworkReply* const reply = manager.get(request);

    QObject::connect(reply,
                     &QNetworkReply::finished,
                     context,
                     [reply, callback]() mutable
                     {
                         QString jsonDocument;
                         if (reply->error() == QNetworkReply::NoError)
                         {
                             jsonDocument = reply->readAll();
                         }
                         else
                         {
                             qDebug() << "Error retrieving JSON document:" << reply->errorString();
                         }

                         reply->deleteLater();
                         callback(FinalizeOnlineVersion(jsonDocument));
                     });
}
