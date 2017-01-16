// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#ifndef CORE_ENUMSTRINGS_H
#define CORE_ENUMSTRINGS_H

#include "Enums.h"
#include <QString>

namespace Ipponboard
{

static char const* const EnumToString(EOption o)
{
    switch (o)
    {
    case eOption_AutoIncrementPoints:
        return "AutoIncrementPoints";

    case eOption_Use2013Rules:
        return "Use2013Rules";

    case eOption_AllSubscoresCount:
        return "AllSubscoresCount";

    default:
        return "";
    }
}

//static EOption StringToEnum(QString const& str)
//{
//    if (str == EnumToString(eOption_AutoIncrementPoints))
//    {
//        return eOption_AutoIncrementPoints;
//    }
//    else if (str == EnumToString(eOption_Use2013Rules))
//    {
//        return eOption_Use2013Rules;
//    }
//    else if (str == EnumToString(eOption_CountAllSubscores))
//    {
//        return eOption_CountAllSubscores;
//    }

//    return eOption_Invalid;
//}

}
#endif // CORE_ENUMSTRINGS_H
