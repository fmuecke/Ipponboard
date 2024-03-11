// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/debug.h"
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

UpdateChecker::UpdateChecker()
{
    TRACE(2, "UpdateChecker::UpdateChecker()");
}

QString GetXmlValue(QXmlQuery query, QString xPath)
{
    TRACE(2, "GetXmlValue()");
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
    TRACE(2, "UpdateChecker::CheckForNewerVersion()");
    QXmlQuery query;

	try
	{
        query.setFocus(QUrl("http://ipponboard.koe-judo.de/files/current_version.xml")); // TODO: Wenn offline, dauert die Abfrage zu lange und die Anwendung startet nicht
	}
	catch (...)
	{
		return false;
	}

	auto version = GetXmlValue(query, "Ipponboard/Version/text()");

	if (!version.isEmpty())
	{
		if (VersionComparer::IsVersionLess(VersionInfo::VersionStr, version.toStdString()))
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
					return QDesktopServices::openUrl(QUrl("https://ipponboard.koe-judo.de"));
				}

				return true;
			}
		}
	}

	return false;
}
