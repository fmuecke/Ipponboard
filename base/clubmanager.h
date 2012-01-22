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

BOOST_CLASS_VERSION(Ipponboard::Club, 1);

namespace boost
{
namespace serialization
{

template<class Archive>
void serialize(Archive& ar, Ipponboard::Club& c, const unsigned int version)
{
    ar& BOOST_SERIALIZATION_NVP(c.name);
    ar& BOOST_SERIALIZATION_NVP(c.logoFile);

    if (version > 0)
        ar& BOOST_SERIALIZATION_NVP(c.address);
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

    bool GetClub(int index, Ipponboard::Club& club) const;
    const QString GetLogo(const QString& clubName) const;
    const QString GetAddress(const QString& clubName) const;

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
