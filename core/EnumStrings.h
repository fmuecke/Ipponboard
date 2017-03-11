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
    case eOption_AutoAdjustPoints:
        return "AutoAdjustPoints";

    case eOption_AllSubscoresCount:
        return "AllSubscoresCount";

    default:
        return "";
    }
}

}
#endif // CORE_ENUMSTRINGS_H
