#include "VersionComparer.h"
//#include "../util/semver/semantic_version.h"
//#include "../util/semver/semantic_version_v2.cpp"

#include <QString>
#include <QStringList>

bool VersionComparer::IsVersionLess(std::string const& version1, std::string const& version2)
{
    //return semver::Version(version1) < semver::Version(version2);


    QStringList onlineVer = QString(version2.c_str()).split('.');
    QStringList buildVer = QString(version1.c_str()).split('.');

    while (onlineVer.length() < 3) onlineVer.append("0");
    while (buildVer.length() < 3) buildVer.append("0");

    if ((onlineVer[0].toInt() > buildVer[0].toInt()))
    {
        return true;
    }
    else if (onlineVer[0].toInt() == buildVer[0].toInt())
    {
        if (onlineVer[1].toInt() > buildVer[1].toInt())
        {
            return true;
        }
        else if ((onlineVer[1].toInt() == buildVer[1].toInt()))
        {
            auto patchVer1 = buildVer[2].split('-');
            auto patchVer2 = onlineVer[2].split('-');

            if (patchVer1[0].toInt() < patchVer2[0].toInt())
            {
                return true;
            }
            if (patchVer1[0].toInt() > patchVer2[0].toInt())
            {
                return false;
            }

            auto preRelease1 = patchVer1.count() > 1 ? patchVer1[1] : QString();
            auto preRelease2 = patchVer2.count() > 1 ? patchVer2[1] : QString();

            if (!preRelease1.isEmpty())
            {
                if (preRelease2.isEmpty())
                {
                    return true;
                }
                else
                {
                    return preRelease1 < preRelease2;  // alphanum comparison
                }
            }
        }
    }

    return false;
}

