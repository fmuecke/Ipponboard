TEMPLATE = subdirs
SUBDIRS += core gamepad SingleTournament TeamTournament
SingleTournament.depends = core gamepad
TeamTournament.depends = core gamepad