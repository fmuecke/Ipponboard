#include "DonationManager.h"

#include <random>
#include <QDesktopServices>
#include <QUrl>
#include "../util/array_helpers.h"

static QString DonationLabels[] =
{
    "I really like Ipponboard and want to contribute",
    "Ipponboard rocks! Please don't stop!",
    "Ipponboard really saves my day",
    "Let me donate a beer (or two)",
    "It really makes tournaments less work",
    "Best score board ever!",
    "I really love Ipponboard!",
    "Let me support future development",
    "I want to donate because Ipponboard is really great",
    "Let me show my appreciation and donate a buck",
    "There are still features missing...",
    "I believe there is still some development to do...",
    "Please fix my bug...",
    "Please keep Ipponboard alive",
    "I need more features",
    "Thanks for investing your time!"
};

char const * const DonationManager::DonationUrl = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=K4A2XEMB9PPGJ";

DonationManager::DonationManager()
{
}

QString DonationManager::GetDonationLabel()
{
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, fm::array_size(DonationLabels));

    return DonationLabels[distribution(generator)];
}

void DonationManager::OpenUrl()
{
    QDesktopServices::openUrl(QUrl(DonationUrl));
}
