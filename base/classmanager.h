#ifndef CLASSMANAGER_H_INCLUDED
#define CLASSMANAGER_H_INCLUDED

#include <QString>
#include <QTime>
#include <QStringList>
#include <vector>
#include <string>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

// forwards
class QListWidgetItem;

namespace Ipponboard
{
	struct TournamentClass
	{
		typedef std::vector<QString> StringList;

		void Init( QString const& new_name )
		{
			name = new_name;
			round_time_in_seconds = 0;
			golden_score_time_in_seconds = 0;
			weight_classes.clear();
		}

		QString name;
		int round_time_in_seconds;
		int golden_score_time_in_seconds;
		StringList weight_classes;

		inline bool operator==( QString const& n ) const
		{
			return name == n;
		}
		void RemoveWeight(QString const& weight)
		{
			StringList::const_iterator iter =
				std::find(weight_classes.begin(), weight_classes.end(),
					weight);
			if( iter != weight_classes.end() )
				weight_classes.erase(iter);
		}

		QString ToString() const { return name; }
	};
}

namespace boost {
namespace serialization {

template<class Archive>
void serialize(Archive& ar, Ipponboard::TournamentClass& c,
			   const unsigned int /*version*/)
{
	ar & BOOST_SERIALIZATION_NVP(c.name);
	ar & BOOST_SERIALIZATION_NVP(c.round_time_in_seconds);
	ar & BOOST_SERIALIZATION_NVP(c.golden_score_time_in_seconds);
	ar & BOOST_SERIALIZATION_NVP(c.weight_classes);
}

} // namespace serialization
} // namespace boost

namespace Ipponboard
{

class TournamentClassManager
{
	//Q_OBJECT
public:
	static const char* const str_fileName;
	static const char* const str_defaultClass;

	typedef std::vector<Ipponboard::TournamentClass> TournamentClassList;

	TournamentClassManager();
	virtual ~TournamentClassManager();

	bool GetClass( int index, Ipponboard::TournamentClass& t ) const;
	bool GetClass( QString const& name, Ipponboard::TournamentClass& t ) const;

	// overwrites an existing club!
	bool HasClass( QString const& name ) const;
	void AddClass( Ipponboard::TournamentClass const& t );
	void AddClass( QString const& name );
	void UpdateClass( Ipponboard::TournamentClass const& t );
	void RemoveClass( QString const& name );
	int ClassCount() const { return m_Classes.size(); }
	bool ClassesFromString( std::string const& s );
	std::string const ClassesToString();

private:
	void LoadClasses_();
	void SaveClasses_();
	void LoadDefaultClasses_();

	TournamentClassList m_Classes;
};

} // namespace Ipponboard

#endif // CLASSMANAGER_H_INCLUDED
