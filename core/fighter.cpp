// Copyright 2010-2012 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#include "fighter.h"

using namespace Ipponboard;

Fighter::Fighter(QString const& firstName,
                 QString const& lastName,
                 QString const& weightClass,
                 QString const& club)
    : first_name(firstName)
    , last_name(lastName)
    , weight_class(weightClass)
    , club(club)
{
}
