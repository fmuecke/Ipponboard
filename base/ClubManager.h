#ifndef BASE__CLUBMANAGER_H_
#define BASE__CLUBMANAGER_H_

#include <QString>
#include <vector>

// forwards
class QListWidgetItem;

namespace Ipponboard
{
struct Club
{
	Club()
		: name()
		, logoFile()
		, address()
	{}

	Club(QString const& id, QString const& logo)
		: name(id)
		, logoFile(logo)
		, address()
	{}

	QString name;
	QString logoFile;
	QString address;

	inline bool operator<(const Club& rhs) const
	{
		return this->ToString() < rhs.ToString();
	}

	inline QString ToString() const { return name; }
};
}

namespace Ipponboard
{

class ClubManager
{
	//Q_OBJECT
public:
	typedef std::vector<Ipponboard::Club> ClubList;
	static const char* const str_filename_club_definitions;

	ClubManager();
	virtual ~ClubManager();

	bool GetClub(int index, Ipponboard::Club& club) const;
	QString GetLogo(const QString& clubName) const;
	QString GetAddress(const QString& clubName) const;

	// overwrites an existing club!
	void AddClub(const Ipponboard::Club& club);
	void UpdateClub(unsigned int index, const Ipponboard::Club& club);
	void RemoveClub(unsigned int index);
	void SortClubs();
	int ClubCount() const { return m_Clubs.size(); }

private:
	void LoadClubs_();
	void SaveClubs_();

	ClubList m_Clubs;
};

} // namespace Ipponboard

#endif  // BASE__CLUBMANAGER_H_
