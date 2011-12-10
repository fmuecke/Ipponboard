TEMPLATE = subdirs
SUBDIRS = gamepad SingleTournament TeamTournament 
SingleTournament.depends gamepad
TeamTournament.depends gamepad