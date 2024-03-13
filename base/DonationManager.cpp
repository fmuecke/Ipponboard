// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/debug.h"
#include "DonationManager.h"

#include <random>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include "../util/array_helpers.h"

char const* const DonationManager::DonationUrl = ""; // "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=K4A2XEMB9PPGJ"; TODO

DonationManager::DonationManager()
{
    TRACE(2, "DonationManager::DonationManager()");
}

QString DonationManager::GetDonationLabel()
{
    TRACE(2, "DonationManager::GetDonationLabel()");
    static const char* const DonationLabels[] =
	{
		QT_TRANSLATE_NOOP("DonationLabel", "Contribute"),
		QT_TRANSLATE_NOOP("DonationLabel", "Ipponboard rocks! Please don't stop!"),
		QT_TRANSLATE_NOOP("DonationLabel", "It really saves my day"),
		QT_TRANSLATE_NOOP("DonationLabel", "Donate a beer (or two)"),
		QT_TRANSLATE_NOOP("DonationLabel", "Tournaments are less work now"),
		QT_TRANSLATE_NOOP("DonationLabel", "Best score board ever!"),
		QT_TRANSLATE_NOOP("DonationLabel", "I really love Ipponboard!"),
		QT_TRANSLATE_NOOP("DonationLabel", "Support future development"),
		QT_TRANSLATE_NOOP("DonationLabel", "Ipponboard is really great"),
		QT_TRANSLATE_NOOP("DonationLabel", "Show my appreciation and donate a buck"),
		QT_TRANSLATE_NOOP("DonationLabel", "There are still features missing..."),
		QT_TRANSLATE_NOOP("DonationLabel", "There is still some development to do..."),
		QT_TRANSLATE_NOOP("DonationLabel", "Please fix my bug..."),
		QT_TRANSLATE_NOOP("DonationLabel", "Please keep Ipponboard alive!"),
		QT_TRANSLATE_NOOP("DonationLabel", "I need more features"),
		QT_TRANSLATE_NOOP("DonationLabel", "Thanks for investing your time!"), // yes,
		QT_TRANSLATE_NOOP("DonationLabel", "Thanks for investing your time!")
	};

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> distribution(0, (int)fm::array_size(DonationLabels));

	return QCoreApplication::instance()->translate("DonationLabel", DonationLabels[distribution(generator)]);
}

void DonationManager::OpenUrl()
{
    TRACE(2, "DonationManager::OpenUrl()");
    QDesktopServices::openUrl(QUrl(DonationUrl));
}
