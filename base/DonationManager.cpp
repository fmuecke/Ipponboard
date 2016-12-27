#include "DonationManager.h"

#include <random>
#include <QDesktopServices>
#include <QUrl>
#include <QApplication>
#include "../util/array_helpers.h"

char const * const DonationManager::DonationUrl = "https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=K4A2XEMB9PPGJ";

DonationManager::DonationManager()
{
}

QString DonationManager::GetDonationLabel()
{
    static const char* const DonationLabels[] =
    {
        QT_TRANSLATE_NOOP("DonationLabel", "I really like Ipponboard and want to contribute"),
        QT_TRANSLATE_NOOP("DonationLabel", "Ipponboard rocks! Please don't stop!"),
        QT_TRANSLATE_NOOP("DonationLabel", "Ipponboard really saves my day"),
        QT_TRANSLATE_NOOP("DonationLabel", "Let me donate a beer (or two)"),
        QT_TRANSLATE_NOOP("DonationLabel", "It really makes tournaments less work"),
        QT_TRANSLATE_NOOP("DonationLabel", "Best score board ever!"),
        QT_TRANSLATE_NOOP("DonationLabel", "I really love Ipponboard!"),
        QT_TRANSLATE_NOOP("DonationLabel", "Let me support future development"),
        QT_TRANSLATE_NOOP("DonationLabel", "I want to donate because Ipponboard is really great"),
        QT_TRANSLATE_NOOP("DonationLabel", "Let me show my appreciation and donate a buck"),
        QT_TRANSLATE_NOOP("DonationLabel", "There are still features missing..."),
        QT_TRANSLATE_NOOP("DonationLabel", "I believe there is still some development to do..."),
        QT_TRANSLATE_NOOP("DonationLabel", "Please fix my bug..."),
        QT_TRANSLATE_NOOP("DonationLabel", "Please keep Ipponboard alive!"),
        QT_TRANSLATE_NOOP("DonationLabel", "I need more features"),
        QT_TRANSLATE_NOOP("DonationLabel", "Thanks for investing your time!"), // yes,
        QT_TRANSLATE_NOOP("DonationLabel", "Thanks for investing your time!")
    };

    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, fm::array_size(DonationLabels));

    return QCoreApplication::instance()->translate("DonationLabel", DonationLabels[distribution(generator)]);
}

void DonationManager::OpenUrl()
{
    QDesktopServices::openUrl(QUrl(DonationUrl));
}
