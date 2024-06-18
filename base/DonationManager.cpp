// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "DonationManager.h"

#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include "../util/array_helpers.h"
#include <random>

char const* const DonationManager::DonationUrl = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=K4A2XEMB9PPGJ";

DonationManager::DonationManager()
{
}

QString DonationManager::GetDonationLabel()
{
	static const char* const DonationLabels[] =
	{
		QT_TRANSLATE_NOOP("DonationLabel", "Contribute"),
		QT_TRANSLATE_NOOP("DonationLabel", "Ipponboard rocks!"),
		QT_TRANSLATE_NOOP("DonationLabel", "It really saves my day"),
		QT_TRANSLATE_NOOP("DonationLabel", "Donate a beer (or two)"),
		QT_TRANSLATE_NOOP("DonationLabel", "Buy me a coffee"),
		QT_TRANSLATE_NOOP("DonationLabel", "Buy me a pizza"),
		QT_TRANSLATE_NOOP("DonationLabel", "Best score board ever!"),
		QT_TRANSLATE_NOOP("DonationLabel", "I really love Ipponboard!"),
		QT_TRANSLATE_NOOP("DonationLabel", "Support future development"),
		QT_TRANSLATE_NOOP("DonationLabel", "Ipponboard is awesome!"),
		QT_TRANSLATE_NOOP("DonationLabel", "Show me appreciation and donate a buck"),
		QT_TRANSLATE_NOOP("DonationLabel", "I want more features!"),
		QT_TRANSLATE_NOOP("DonationLabel", "Please fix this bug..."),
		QT_TRANSLATE_NOOP("DonationLabel", "Please keep Ipponboard alive!"),
		QT_TRANSLATE_NOOP("DonationLabel", "Thanks for investing your time!"), // yes,
		QT_TRANSLATE_NOOP("DonationLabel", "Thanks for investing your time!")
	};

	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> distribution(0, fm::array_size(DonationLabels) - 1);

	return QCoreApplication::instance()->translate("DonationLabel", DonationLabels[distribution(generator)]);
}

void DonationManager::OpenUrl()
{
	QDesktopServices::openUrl(QUrl(DonationUrl));
}
