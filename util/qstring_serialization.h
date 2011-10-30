#ifndef UTIL__QSTRING_SERIALIZATION_H_
#define UTIL__QSTRING_SERIALIZATION_H_

#include <QString>
#include <string>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/serialization/string.hpp>

namespace boost {
namespace serialization {


template<class Archive>
inline void save( Archive& ar, const QString& s, const unsigned int /*version*/ )
{
	using boost::serialization::make_nvp;
	std::string str = s.toStdString();
	ar << make_nvp("value", str);
}

template<class Archive>
inline void load( Archive& ar, QString& s, const unsigned int /*version*/ )
{
	using boost::serialization::make_nvp;

	std::string stdStr;
	ar >> make_nvp("value", stdStr);
	s = QString::fromStdString(stdStr);
}

template<class Archive>
inline void serialize( Archive& ar, QString& s, const unsigned int file_version )
{
	boost::serialization::split_free(ar, s, file_version);
}

} // namespace serialization
} // namespace boost

#endif  // UTIL__QSTRING_SERIALIZATION_H_
