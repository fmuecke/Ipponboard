TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += \
	core \
	gamepad \
	SingleTournament \
	TeamTournament \
	#VersionSelector \
        GamePadDemo \
        test
SingleTournament.depends = core gamepad
TeamTournament.depends = core gamepad
