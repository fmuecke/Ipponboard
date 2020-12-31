// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef UPDATER_H
#define UPDATER_H

class QString;

class UpdateChecker
{
public:
	UpdateChecker();

	static bool CheckForNewerVersion();
};

#endif // UPDATER_H
