// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "Athlete.h"

using namespace Ipponboard;

Athlete::Athlete(QString const& firstName, QString const& lastName)
    : first_name(firstName), last_name(lastName), club(), weight(), category()
{
}
