#ifndef UPDATER_H
#define UPDATER_H

class QString;

class UpdateChecker
{
public:
	UpdateChecker();

	static bool CheckForNewerVersion();
};

#endif // UPDATER_H
