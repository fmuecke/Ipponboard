TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += \
	core \
	gamepad \
	base \
	#VersionSelector \
	GamepadDemo \
	test
base.depends = core gamepad
