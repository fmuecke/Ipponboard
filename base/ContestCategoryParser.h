// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef FIGHTCATEGORYPARSER_H
#define FIGHTCATEGORYPARSER_H

#include "../core/ContestCategory.h"

#include <string>

class ContestCategoryParser
{
  public:
    ContestCategoryParser();

    // Qt settings format
    static Ipponboard::CategoryList ParseIniFile(const QString& file);
    static void ToIniFile(const QString& file, Ipponboard::CategoryList const& categories);
};

#endif // FIGHTCATEGORYPARSER_H
