// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef UPDATER_H
#define UPDATER_H

#include <QString>

class OnlineVersionChecker
{
public:

	OnlineVersionChecker();

	enum class State
	{
		Empty = 0,
		UpToDate,
		NewerAvailable,
		NewerThanOnlineAvailable
	};

	struct OnlineVersion
	{
		QString version;
		QString infoUrl;
		QString downloadUrl;
		QString changes_de;
		QString changes_en;
		State state;
	};

	static OnlineVersion CheckOnlineVersion();

	static QString get_version_document(QString url);
	static OnlineVersion parse_version_document(QString jsonDoc);

	static const QString VersionDocumentUrl;
	static const QString ProjectReleasesUrl;
};

#endif // UPDATER_H
