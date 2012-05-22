// Copyright 2010-2012 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#include "fighter.h"

using namespace Ipponboard;

Fighter::Fighter(QString const& firstname,
                 QString const& lastname,
                 QString const& weightclass,
                 QString const& club)
    : m_firstName(firstname)
    , m_lastName(lastname)
    , m_weightClass(weightclass)
    , m_club(club)
{
}
