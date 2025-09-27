// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef VERSIONCOMPARER_H
#define VERSIONCOMPARER_H

#include <string>

class VersionComparer
{
  public:
	struct SemVer
	{
		int major{ 0 };
		int minor{ 0 };
		int patch{ 0 };
		std::string prerelease;

		static SemVer FromString(std::string const& version);
		std::string ToString() const
		{
			if (prerelease.empty())
			{
				return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
			}
			return std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch) + "-" + prerelease;
		}
	};

	// returns true if lhs is less than rhs, supporting semver versioning
	// e.g. 1.0.0 < 1.0.1
	static bool IsVersionLess(std::string const& lhs, std::string const& rhs);
};

#endif // VERSIONCOMPARER_H
