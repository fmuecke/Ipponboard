// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "OnlineVersionChecker.h"

#include "VersionComparer.h"
#include "versioninfo.h"

//#include <QCoreApplication>
//#include <QMessageBox>
#include <QDebug>
#include <QElapsedTimer>
#include <QEventLoop>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>
#include <QStringList>

const QString OnlineVersionChecker::VersionDocumentUrl =
    "https://api.github.com/repos/fmuecke/Ipponboard/releases/latest";
const QString OnlineVersionChecker::ProjectReleasesUrl =
    "https://github.com/fmuecke/Ipponboard/releases";

OnlineVersionChecker::OnlineVersionChecker() {}

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

    QNetworkAccessManager manager;
    QNetworkRequest request(url);
    QNetworkReply* reply = manager.get(request);

    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
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

    reply->deleteLater();
    return jsonDocument;
}

OnlineVersionChecker::OnlineVersion OnlineVersionChecker::parse_version_document(QString jsonDoc)
{
    QJsonDocument doc = QJsonDocument::fromJson(jsonDoc.toUtf8());
    QJsonObject json = doc.object();

    OnlineVersion versionInfo;
    versionInfo.version = GetJsonValue(json, "name");
    versionInfo.infoUrl = "https://github.com/fmuecke/Ipponboard/releases/latest";
    versionInfo.downloadUrl = GetJsonValue(json, "assets[0].browser_download_url");
    versionInfo.changes_en = GetJsonValue(json, "body");
    versionInfo.changes_de = versionInfo.changes_en;
    return versionInfo;
}

OnlineVersionChecker::OnlineVersion OnlineVersionChecker::CheckOnlineVersion()
{
    qInfo() << "Checking for updated version...";

    QElapsedTimer timer;
    timer.start();

    auto doc = get_version_document(VersionDocumentUrl);
    if (doc.isEmpty())
    {
        qDebug() << "checking took:" << timer.elapsed() << "ms (empty document)";
        return OnlineVersion();
    }

    auto onlineVersion = parse_version_document(doc);
    if (!onlineVersion.version.isEmpty())
    {
        qDebug() << "online version string:" << onlineVersion.version
                 << "( current:" << QString(VersionInfo::VersionStr) << ")";
        qDebug() << "checking took " << timer.elapsed() << "ms";

        if (VersionComparer::IsVersionLess(VersionInfo::VersionStr,
                                           onlineVersion.version.toStdString()))
        {
            qInfo() << "newer version" << onlineVersion.version << "available";
            onlineVersion.state = State::NewerAvailable;
        }
        else
        {
            if (VersionComparer::IsVersionLess(onlineVersion.version.toStdString(),
                                               VersionInfo::VersionStr))
            {
                qInfo() << "current version" << VersionInfo::VersionStr
                        << "is newer than online version" << onlineVersion.version;
                onlineVersion.state = State::NewerThanOnlineAvailable;
            }
            else
            {
                qInfo() << "no newer version available";
                onlineVersion.state = State::UpToDate;
            }
        }
    }

    return onlineVersion;
}