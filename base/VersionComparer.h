#ifndef VERSIONCOMPARER_H
#define VERSIONCOMPARER_H

#include <string>

class VersionComparer
{
public:
    static bool IsVersionLess(std::string const& version1, std::string const& version2);
};

#endif // VERSIONCOMPARER_H
