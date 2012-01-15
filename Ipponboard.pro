TEMPLATE = subdirs
SUBDIRS += core gamepad SingleTournament TeamTournament VersionSelector
SingleTournament.depends = core gamepad
TeamTournament.depends = core gamepad