// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef DONATIONMANAGER_H
#define DONATIONMANAGER_H

#include <QString>

class DonationManager
{
public:
	static char const* const DonationUrl;

	DonationManager();

	static QString GetDonationLabel();
	static void OpenUrl();
};

#endif // DONATIONMANAGER_H
