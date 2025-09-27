// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef ARRAY_HELPERS_H
#define ARRAY_HELPERS_H

namespace fm
{
template <typename T, std::size_t N>
constexpr std::size_t array_size(T const(&)[N]) noexcept
{
	return N;
}
}
#endif // ARRAY_HELPERS_H
