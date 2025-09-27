// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "VersionComparer.h"

#include <QString>
#include <QStringList>

VersionComparer::SemVer VersionComparer::SemVer::FromString(std::string const& version)
{
	SemVer semver;
	QStringList parts = QString(version.c_str()).split('-'); // split prerelease from version

	QStringList versionParts = parts[0].split('.'); // split version strings

	// ensure we have at least 3 parts
	while (versionParts.count() < 3) versionParts.append("0");
	semver.major = versionParts[0].startsWith("v") ? versionParts[0].mid(1).toInt() : versionParts[0].toInt();
	semver.minor = versionParts[1].toInt();
	semver.patch = versionParts[2].toInt();

	if (parts.count() > 1) { semver.prerelease = parts[1].toStdString(); }

	return semver;
}

bool VersionComparer::IsVersionLess(std::string const& lhs, std::string const& rhs)
{
	//return semver::Version(version1) < semver::Version(version2);

	if (lhs == rhs) return false;
	if (lhs.empty() || rhs.empty()) return false;

	SemVer lhsSemVer = SemVer::FromString(lhs);
	SemVer rhsSemVer = SemVer::FromString(rhs);

	if (lhsSemVer.major < rhsSemVer.major) { return true; }
	else if (lhsSemVer.major == rhsSemVer.major)
	{
		if (lhsSemVer.minor < rhsSemVer.minor) { return true; }
		else if (lhsSemVer.minor == rhsSemVer.minor)
		{
			if (lhsSemVer.patch < rhsSemVer.patch) { return true; }
			else if (lhsSemVer.patch == rhsSemVer.patch)
			{
				if (lhsSemVer.prerelease.empty()) { return false; }
				else if (rhsSemVer.prerelease.empty()) { return true; }
				else { return lhsSemVer.prerelease < rhsSemVer.prerelease; }
			}
		}
	}

	return false;
}
