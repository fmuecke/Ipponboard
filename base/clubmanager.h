#ifndef BASE__CLUBMANAGER_H_
#define BASE__CLUBMANAGER_H_

#include <QString>
#include <vector>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

// forwards
class QListWidgetItem;

namespace Ipponboard
{
	struct Club
	{
		Club()
			: name("")
			, logoFile("")
		{}

		Club(QString const& id, QString const& logo)
			: name(id)
			, logoFile(logo)
		{}

		QString name;
		QString logoFile;

		QString ToString() const { return name; }
	};
}

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive& ar, Ipponboard::Club& c, const unsigned int /*version*/)
{
	ar & BOOST_SERIALIZATION_NVP(c.name);
	ar & BOOST_SERIALIZATION_NVP(c.logoFile);
}

} // namespace serialization
} // namespace boost

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

	bool GetClub( int index, Ipponboard::Club& club ) const;
	const QString GetLogo( const QString& clubName ) const;

	// overwrites an existing club!
	void AddClub( const Ipponboard::Club& club );
	void UpdateClub( unsigned int index, const Ipponboard::Club& club );
	void RemoveClub( unsigned int index );
	int ClubCount() const { return m_Clubs.size(); }

private:
	void LoadClubs_();
	void SaveClubs_();

	ClubList m_Clubs;
};

} // namespace Ipponboard

#endif  // BASE__CLUBMANAGER_H_
