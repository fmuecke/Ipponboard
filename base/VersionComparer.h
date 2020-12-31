// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef VERSIONCOMPARER_H
#define VERSIONCOMPARER_H

#include <string>

class VersionComparer
{
public:
	static bool IsVersionLess(std::string const& version1, std::string const& version2);
};

#endif // VERSIONCOMPARER_H
