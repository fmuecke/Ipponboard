// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UTIL__HELPERS_H_
#define UTIL__HELPERS_H_

namespace fm
{

template <typename T> inline bool IsOptionSet(T data, T option)
{
    return (data & option) == option;
}

} // namespace fm

#endif // UTIL__HELPERS_H_
