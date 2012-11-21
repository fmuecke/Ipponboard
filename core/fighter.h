// Copyright 2010-2012 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#ifndef FIGHTER_H
#define FIGHTER_H

#include <QString>

namespace Ipponboard
{

struct Fighter
{
public:
    Fighter(QString const& firstName,
            QString const& lastName,
            QString const& weightClass,
            QString const& club);

    QString first_name;
    QString last_name;
    QString weight_class;
    QString club;
    //QString nation;
};

}  // namespace Ipponboard

#endif // FIGHTER_H
