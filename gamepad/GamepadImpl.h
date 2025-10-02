// Copyright 2025 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#include "JessTechPadAdapter.h"

#if defined(_WIN32)
#include "GamepadWinImpl.h"
#else
#include "GamepadLinuxImpl.h"
#endif