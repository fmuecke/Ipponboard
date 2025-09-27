// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__CONTROLLERCONFIG_H_
#define BASE__CONTROLLERCONFIG_H_

namespace Ipponboard
{

struct ControllerConfig
{
	ControllerConfig()
		: button_hajime_mate(-1)
		, button_reset(-1)
		, button_reset_2(-1)
		, button_next(-1)
		, button_prev(-1)
		, button_pause(-1)
		, button_osaekomi_toketa_first(-1)
		, button_osaekomi_toketa_second(-1)
		, button_reset_hold_first(-1)
		, button_reset_hold_second(-1)
		, button_hansokumake_first(-1)
		, button_hansokumake_second(-1)
		, axis_inverted_X(false)
		, axis_inverted_Y(false)
		, axis_inverted_R(false)
		, axis_inverted_Z(false)
		, key_hajime_mate(-1)
		, key_reset(-1)
		, key_reset_2(-1)
		, key_next(-1)
		, key_prev(-1)
		, key_pause(-1)
		, key_osaekomi_toketa_first(-1)
		, key_osaekomi_toketa_second(-1)
		, key_reset_hold_first(-1)
		, key_reset_hold_second(-1)
		, key_hansokumake_first(-1)
		, key_hansokumake_second(-1)
	{}

	int button_hajime_mate;
	int button_reset;
	int button_reset_2;
	int button_next;
	int button_prev;
	int button_pause;
	int button_osaekomi_toketa_first;
	int button_osaekomi_toketa_second;
	int button_reset_hold_first;
	int button_reset_hold_second;
	int button_hansokumake_first;
	int button_hansokumake_second;

	bool axis_inverted_X;
	bool axis_inverted_Y;
	bool axis_inverted_R;
	bool axis_inverted_Z;

	int key_hajime_mate;
	int key_reset;
	int key_reset_2;
	int key_next;
	int key_prev;
	int key_pause;
	int key_osaekomi_toketa_first;
	int key_osaekomi_toketa_second;
	int key_reset_hold_first;
	int key_reset_hold_second;
	int key_hansokumake_first;
	int key_hansokumake_second;
};

} // namespace ipponboard

#endif  // BASE__CONTROLLERCONFIG_H_
