// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef CORE__IVIEW_H_
#define CORE__IVIEW_H_

#include <QString>
#include "Enums.h"

namespace Ipponboard
{
class IController;

class IView
{
  public:
	virtual void UpdateView() = 0;
	virtual void Reset() = 0;
	virtual void SetShowInfoHeader(bool show) = 0;
};

} // namespace Ipponboard
#endif // CORE__IVIEW_H_
