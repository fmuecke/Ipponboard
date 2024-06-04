// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef UPDATER_H
#define UPDATER_H

#include <QString>

class QXmlQuery;

class UpdateChecker
{
public:

	UpdateChecker();

	static bool CheckForNewerVersion();

	struct current_version
	{
		QString version;
		QString infoUrl;
		QString downloadUrl;
		QString changes_de;
		QString changes_en;
	};

	static QXmlQuery get_version_document_DEPRECATED(QString url);
	static QString get_version_document(QString url);
	static current_version parse_version_document_DEPRECATED(QXmlQuery document);
	static current_version parse_version_document(QString jsonDoc);
	static QString format_version_message(current_version onlineVersion, QString lang);

	static const QString VersionDocumentUrl_DEPRECATED;
	static const QString VersionDocumentUrl;
	static const QString ProjectReleasesUrl;
};

#endif // UPDATER_H
