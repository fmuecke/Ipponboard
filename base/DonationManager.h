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
