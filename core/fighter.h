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

class Fighter
{
public:
    Fighter(QString const& firstname,
            QString const& lastname,
            QString const& weightclass,
            QString const& club);

    QString m_firstName;
    QString m_lastName;
    QString m_weightClass;
    QString m_club;
    //QString nation;
};
}

#endif // FIGHTER_H
