// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef CORE__IGOLDENSCOREVIEW_H_
#define CORE__IGOLDENSCOREVIEW_H_

#include <QString>
#include "Enums.h"

namespace Ipponboard
{
class IController;

class IGoldenScoreView
{
public:
	virtual void UpdateGoldenScoreView() = 0;
};

}
#endif  // CORE__IGOLDENSCOREVIEW_H_
