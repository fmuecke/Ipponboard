// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef UTIL__DIALOGRESULT_H_
#define UTIL__DIALOGRESULT_H_

namespace fm
{

template <typename T>
class DialogResult
{
public:
	explicit DialogResult(T const& data) : m_dialogData(data) {}
	virtual ~DialogResult() {}

	virtual T Result() const { return m_dialogData; }

protected:
	T m_dialogData;
};

}

#endif  // UTIL__DIALOGRESULT_H_
