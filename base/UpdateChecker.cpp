// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "UpdateChecker.h"
#include "versioninfo.h"
#include "VersionComparer.h"

#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QXmlQuery>
#include <QUrl>
#include <QDesktopServices>
#include <QElapsedTimer>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

const QString UpdateChecker::VersionDocumentUrl_DEPRECATED = "https://ipponboard.koe-judo.de/files/current_version.xml";
const QString UpdateChecker::VersionDocumentUrl = "https://api.github.com/repos/fmuecke/Ipponboard/releases/latest";
const QString UpdateChecker::ProjectReleasesUrl = "https://github.com/fmuecke/Ipponboard/releases";

UpdateChecker::UpdateChecker()
{
}

QString GetXmlValue_DEPRECATED(QXmlQuery query, QString xPath)
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
				QString arrayIndex = element.mid(element.indexOf('[') + 1, element.indexOf(']') - element.indexOf('[') - 1);
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

QXmlQuery UpdateChecker::get_version_document_DEPRECATED(QString url)
{
	QXmlQuery query;
	query.setFocus(QUrl(url));
	return query;
}

QString UpdateChecker::get_version_document(QString url)
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

UpdateChecker::current_version UpdateChecker::parse_version_document_DEPRECATED(QXmlQuery query)
{
	UpdateChecker::current_version versionInfo;
	versionInfo.version = GetXmlValue_DEPRECATED(query, "Ipponboard/Version/text()");
	versionInfo.infoUrl = ProjectReleasesUrl;
	versionInfo.downloadUrl = GetXmlValue_DEPRECATED(query, "Ipponboard/DownloadUrl/text()");
	versionInfo.changes_de = GetXmlValue_DEPRECATED(query, "Ipponboard/Changes[@lang='de']/text()");
	versionInfo.changes_en = GetXmlValue_DEPRECATED(query, "Ipponboard/Changes[@lang='en']/text()");
	return versionInfo;
}

UpdateChecker::current_version UpdateChecker::parse_version_document(QString jsonDoc)
{
	QJsonDocument doc = QJsonDocument::fromJson(jsonDoc.toUtf8());
	QJsonObject json = doc.object();

	UpdateChecker::current_version versionInfo;
	versionInfo.version = GetJsonValue(json, "name");
	versionInfo.infoUrl = "https://github.com/fmuecke/Ipponboard/releases/latest";
	versionInfo.downloadUrl = GetJsonValue(json, "assets[0].browser_download_url");
	versionInfo.changes_en = GetJsonValue(json, "body");
	versionInfo.changes_de = versionInfo.changes_en;
	return versionInfo;
}

QString UpdateChecker::format_version_message(current_version onlineVersion, QString lang)
{
	QString changes = QString(": <br><br><tt>%1</tt><br>").arg(lang == "de" ? onlineVersion.changes_de : onlineVersion.changes_en);
	changes.replace("\n", "<br>");

	QString msg = QString("<p>%1 %2</p>")
				  .arg(QCoreApplication::tr("Version %1 available").arg(QString("<b>%1</b>").arg(onlineVersion.version)))
				  .arg(changes);

	msg += QString("<p>%1</p>").arg(QCoreApplication::tr("Do you want to download it or visit the project homepage?"));

	return msg;
}

bool UpdateChecker::CheckForNewerVersion()
{
    qInfo() << "Checking for updated version...";
	current_version onlineVersion{ 0 };

	QElapsedTimer timer;
	timer.start();		
	try
	{
		//onlineVersion = parse_version_document_DEPRECATED(get_version_document_DEPRECATED(VersionDocumentUrl_DEPRECATED));
		onlineVersion = parse_version_document(get_version_document(VersionDocumentUrl));
	}
	catch (...)
	{
		qDebug() << "checking took:" << timer.elapsed() << "ms";
		return false;
	}

    if (!onlineVersion.version.isEmpty())
	{
        qDebug() << "online version string:" << onlineVersion.version << "( current:" << QString(VersionInfo::VersionStr) << ")";
		qDebug() << "checking took " << timer.elapsed() << "ms";

		if (VersionComparer::IsVersionLess(VersionInfo::VersionStr, onlineVersion.version.toStdString()))
		{
            qInfo() << "newer version" << onlineVersion.version << "available";

			auto result = QMessageBox::information(
							  nullptr,
							  QCoreApplication::tr("Ipponboard - New Version Available"),
							  format_version_message(onlineVersion, QCoreApplication::tr("en")),
							  QCoreApplication::tr("Download"),
							  QCoreApplication::tr("Visit Homepage"),
							  QCoreApplication::tr("Cancel"),
							  0, 2);

			if (result == 0) // download
			{
				return QDesktopServices::openUrl(QUrl(onlineVersion.downloadUrl));
			}
			else if (result == 1) // visit homepage
			{
				return QDesktopServices::openUrl(QUrl(onlineVersion.infoUrl));
			}
		}
        else
        {
            if (VersionComparer::IsVersionLess(onlineVersion.version.toStdString(), VersionInfo::VersionStr))
            {
                qInfo() << "current version" << VersionInfo::VersionStr << "is newer than online version" << onlineVersion.version;
            }
            else
            {
                qInfo() << "no newer version available";
            }
        }
	}

    return false;
}
