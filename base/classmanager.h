#ifndef WEIGHTCLASS_MANAGER_H_INCLUDED
#define WEIGHTCLASS_MANAGER_H_INCLUDED

#include <QString>
#include <QTime>
#include <QStringList>
#include <vector>
#include <string>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "weightclass.h"

// forwards
class QListWidgetItem;

namespace Ipponboard {

class WeightClassManager
{
	//Q_OBJECT
public:
	static const char* const str_fileName;
	static const char* const str_defaultClass;

	typedef std::vector<WeightClass> WeightClassList;

	WeightClassManager();
	virtual ~WeightClassManager();

	bool GetClass( int index, WeightClass& t ) const;
	bool GetClass( QString const& name, WeightClass& t ) const;

	// overwrites an existing club!
	bool HasClass( QString const& name ) const;
	void AddClass( WeightClass const& t );
	void AddClass( QString const& name );
	void UpdateClass( WeightClass const& t );
	void RemoveClass( QString const& name );
	int ClassCount() const { return m_Classes.size(); }
	bool ClassesFromString( std::string const& s );
	std::string const ClassesToString();

private:
	void LoadClasses_();
	void SaveClasses_();
	void LoadDefaultClasses_();

	WeightClassList m_Classes;
};

} // namespace Ipponboard

#endif // WEIGHTCLASS_MANAGER_H_INCLUDED
