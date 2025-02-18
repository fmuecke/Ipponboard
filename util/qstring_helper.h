// Copyright 2021 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef QSTRING_HELPER_H
#define QSTRING_HELPER_H

#include <QString>

namespace fm
{
namespace
{
	inline QString NormalizeSpaces(QString const& s)
		{
            QString ret = s.trimmed();

			// trim multiple spaces
			while (ret.contains("  ")) ret.replace("  ", " ");
			return ret;
		}
	}
}
#endif // QSTRING_HELPER_H
