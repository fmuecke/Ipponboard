#ifndef WEIGHTCLASS_H
#define WEIGHTCLASS_H

#include <QString>
#include <QStringList>
#include <vector>
#include <string>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

namespace Ipponboard{

class WeightClass
{
public:
	// Note: class uses std::string instead of QString for
	// better serialization support
	typedef std::vector<std::string> StringList;

	WeightClass()
	{}

	explicit WeightClass( QString const& new_name )
		: name(new_name.toStdString())
		, round_time_secs(0)
		, golden_score_time_secs(0)
		, weights()
	{
	}

	inline bool operator==( std::string const& n ) const
	{
		return name == n;
	}
	inline bool operator==( QString const& n ) const
	{
		return name == n.toStdString();
	}

	void RemoveWeight(QString const& weight);

	const QString ToString() const { return QString::fromStdString(name); }
	const QStringList GetWeightList() const;

	int GetRoundTime() const { return round_time_secs; }
	int GetGoldenScoreTime() const { return golden_score_time_secs; }
	void SetRoundTime(int secs) { round_time_secs = secs; }
	void SetGoldenScoreTime(int secs) { golden_score_time_secs = secs; }
	void AddWeight(std::string const& s) { weights.push_back(s); }

	void MoveWeightUp( int index )
	{
		std::swap( weights.at(index), weights.at(index-1) );
	}

private:
	friend class boost::serialization::access;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(round_time_secs);
		ar & BOOST_SERIALIZATION_NVP(golden_score_time_secs);
		ar & BOOST_SERIALIZATION_NVP(name);
		ar & BOOST_SERIALIZATION_NVP(weights);
	}

	int round_time_secs;
	int golden_score_time_secs;
	std::string name;
	StringList weights;
};

} // namespace Ipponboard

#endif // WEIGHTCLASS_H
