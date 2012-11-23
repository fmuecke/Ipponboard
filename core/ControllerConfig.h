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
		, button_osaekomi_toketa_blue(-1)
		, button_osaekomi_toketa_white(-1)
		, button_reset_hold_blue(-1)
		, button_reset_hold_white(-1)
		, button_hansokumake_blue(-1)
		, button_hansokumake_white(-1)
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
		, key_osaekomi_toketa_blue(-1)
		, key_osaekomi_toketa_white(-1)
		, key_reset_hold_blue(-1)
		, key_reset_hold_white(-1)
		, key_hansokumake_blue(-1)
		, key_hansokumake_white(-1)
	{}

	int button_hajime_mate;
	int button_reset;
	int button_reset_2;
	int button_next;
	int button_prev;
	int button_pause;
	int button_osaekomi_toketa_blue;
	int button_osaekomi_toketa_white;
	int button_reset_hold_blue;
	int button_reset_hold_white;
	int button_hansokumake_blue;
	int button_hansokumake_white;

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
	int key_osaekomi_toketa_blue;
	int key_osaekomi_toketa_white;
	int key_reset_hold_blue;
	int key_reset_hold_white;
	int key_hansokumake_blue;
	int key_hansokumake_white;
};

} // namespace ipponboard

#endif  // BASE__CONTROLLERCONFIG_H_
