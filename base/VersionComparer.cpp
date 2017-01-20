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

    bool isOldVersion = false;
    if ((onlineVer[0].toInt() > buildVer[0].toInt()))
    {
        isOldVersion = true;
    }
    else if (onlineVer[0].toInt() == buildVer[0].toInt())
    {
        if (onlineVer[1].toInt() > buildVer[1].toInt())
        {
            isOldVersion = true;
        }
        else if ((onlineVer[1].toInt() == buildVer[1].toInt()))
        {
            if ((onlineVer[2].toInt() > buildVer[2].toInt()))
            {
                isOldVersion = true;
            }
        }
    }

    return isOldVersion;
}

