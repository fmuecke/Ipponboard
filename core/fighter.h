// Copyright 2010-2013 Florian Muecke. All rights reserved.
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
            QString const& club,
            QString const& weight,
            QString const& category);

    inline bool operator==(Fighter const& other) const
    {
        return this == &other ||
                (first_name == other.first_name &&
                last_name == other.last_name &&
                club == other.club &&
                weight_class == other.weight_class &&
                category == other.category);
    }

    QString first_name;
    QString last_name;
    QString club;
    QString weight_class;
    QString category;
    //QString nation;
};

}  // namespace Ipponboard

#endif // FIGHTER_H
