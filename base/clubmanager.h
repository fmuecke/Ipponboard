#ifndef CLUBMANAGER_H
#define CLUBMANAGER_H

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
		QString name;
		QString city;
		QString homepage;
		QString address;
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
	ar & BOOST_SERIALIZATION_NVP(c.city);
	ar & BOOST_SERIALIZATION_NVP(c.homepage);
	ar & BOOST_SERIALIZATION_NVP(c.address);
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

	ClubManager();
	virtual ~ClubManager();

	bool GetClub( int index, Ipponboard::Club& club ) const;
	const QString GetLogo( const QString& clubName ) const;

	// overwrites an existing club!
	void AddClub( const Ipponboard::Club& club );
	void UpdateClub( int index, const Ipponboard::Club& club );
	void RemoveClub( int index );
	int ClubCount() const { return m_Clubs.size(); }

private:
	void LoadClubs_();
	void SaveClubs_();

	ClubList m_Clubs;
};

} // namespace Ipponboard

#endif // CLUBMANAGER_H
