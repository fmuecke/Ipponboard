#include "classmanager.h"
#include <QObject>

#include <iostream>
#include <fstream>
#include <sstream>
#include "../util/qstring_serialization.h"
#include <boost/serialization/vector.hpp>
#include <algorithm>


using namespace Ipponboard;

const char* const TournamentClassManager::str_fileName = "classes.xml";
const char* const TournamentClassManager::str_defaultClass = "default";

//---------------------------------------------------------
TournamentClassManager::TournamentClassManager()
	: m_Classes()
//---------------------------------------------------------
{
	LoadClasses_();
}

//---------------------------------------------------------
TournamentClassManager::~TournamentClassManager()
//---------------------------------------------------------
{
	SaveClasses_();
}

//---------------------------------------------------------
bool TournamentClassManager::GetClass( int index,
									   Ipponboard::TournamentClass& t ) const
//---------------------------------------------------------
{
	try
	{
		t = m_Classes.at(index);
	}
	catch(std::out_of_range& )
	{
		return false;
	}
	return true;
}

//---------------------------------------------------------
bool TournamentClassManager::GetClass( QString const& name,
									   Ipponboard::TournamentClass& c ) const
//---------------------------------------------------------
{
	TournamentClassList::const_iterator iter =
		std::find(m_Classes.begin(), m_Classes.end(), name );
	if( iter == m_Classes.end() )
	{
		return false;
	}
	c = *iter;
	return true;
}


//---------------------------------------------------------
bool TournamentClassManager::HasClass( QString const& name ) const
//---------------------------------------------------------
{
	// We could use the new cpp0x lambda expression for that
	// or use operator overloading...
	//
	// return std::find_if( m_Classes.begin(), m_Classes.end(),
	//		[name](TournamentClass t)->bool { return t.name == name; }

	return std::find( m_Classes.begin(), m_Classes.end(),
					  name) != m_Classes.end();
}


//---------------------------------------------------------
void TournamentClassManager::AddClass( Ipponboard::TournamentClass const& t )
//---------------------------------------------------------
{
	m_Classes.push_back(t);
}


//---------------------------------------------------------
void TournamentClassManager::AddClass( QString const& name )
//---------------------------------------------------------
{
	TournamentClass t = {0};
	t.Init(name);
	AddClass(t);
}

//---------------------------------------------------------
void TournamentClassManager::UpdateClass( const Ipponboard::TournamentClass& t )
//---------------------------------------------------------
{
	if( t.name.isEmpty() )
		return;

	TournamentClassList::iterator iter =
		std::find(m_Classes.begin(), m_Classes.end(), t.name);

	if( iter == m_Classes.end() )
		Q_ASSERT(!"Critical: tournament class not in list!");

	*iter = t;
}

//---------------------------------------------------------
void TournamentClassManager::RemoveClass( QString const& name )
//---------------------------------------------------------
{
	TournamentClassList::iterator iter =
		std::find(m_Classes.begin(), m_Classes.end(), name);

	if( iter == m_Classes.end() )
		Q_ASSERT(!"Critical: tournament class not in list!");

	m_Classes.erase( iter );
}

//---------------------------------------------------------
void TournamentClassManager::LoadClasses_()
//---------------------------------------------------------
{
	// open the archive
	std::ifstream ifs(str_fileName);
	if( ifs.good() )
	{
		try
		{
			boost::archive::xml_iarchive ia(ifs);
			// restore the classes from the xml archive
			ia >> BOOST_SERIALIZATION_NVP(m_Classes);
		}
		catch( std::exception& )
		{
			QMessageBox::critical(0, QString(QObject::tr("Error")),
				QString(QObject::tr("Unable to parse ") + str_fileName + "!"));

			LoadDefaultClasses_();
		}
	}
	else
	{
		LoadDefaultClasses_();
	}
	ifs.close();

	if( ClassCount() == 0 )
		LoadDefaultClasses_();
}

//---------------------------------------------------------
void TournamentClassManager::SaveClasses_()
//---------------------------------------------------------
{
	// make an archive
	std::ofstream ofs(str_fileName);
	if( ofs.good() )
	{
		boost::archive::xml_oarchive oa(ofs);
		oa << BOOST_SERIALIZATION_NVP(m_Classes);
	}
	else
	{
		QMessageBox::critical(0, QString(QObject::tr("Error")),
							  QString(QObject::tr("Unable to save ") +
					str_fileName + "!"));
	}
	ofs.close();
}

//---------------------------------------------------------
bool TournamentClassManager::ClassesFromString( std::string const& s )
//---------------------------------------------------------
{
	// open the archive
	std::stringstream ss;
	ss.str(s);
	if( ss.good() )
	{
		try
		{
			boost::archive::xml_iarchive ia(ss);
			// restore the classes from the xml archive
			ia >> BOOST_SERIALIZATION_NVP(m_Classes);
			return true;
		}
		catch( std::exception& )
		{
			return false;
		}
	}
	return false;
}


//---------------------------------------------------------
std::string const TournamentClassManager::ClassesToString()
//---------------------------------------------------------
{
	// make an archive
	std::stringstream ss;
	if( ss.good() )
	{
		boost::archive::xml_oarchive oa(ss);
		oa << BOOST_SERIALIZATION_NVP(m_Classes);
	}
	else
	{
		return std::string("");
	}
	return ss.str();
}

//--------------------------------------------------------
void TournamentClassManager::LoadDefaultClasses_()
//--------------------------------------------------------
{
	m_Classes.clear();

	TournamentClass t = {0};
	t.Init(str_defaultClass);
	t.weight_classes.push_back("-66 kg");
	t.weight_classes.push_back("-73 kg");
	t.weight_classes.push_back("-81 kg");
	t.weight_classes.push_back("-90 kg");
	t.weight_classes.push_back("-100 kg");
	t.weight_classes.push_back("+100 kg");
	t.round_time_in_seconds = 5*60;
	t.golden_score_time_in_seconds = 3*60;
	AddClass(t);

	t.Init("MU14");
	t.weight_classes.push_back("-31 kg");
	t.weight_classes.push_back("-34 kg");
	t.weight_classes.push_back("-37 kg");
	t.weight_classes.push_back("-40 kg");
	t.weight_classes.push_back("-43 kg");
	t.weight_classes.push_back("-46 kg");
	t.weight_classes.push_back("-50 kg");
	t.weight_classes.push_back("-55 kg");
	t.weight_classes.push_back("-60 kg");
	t.weight_classes.push_back("+60 kg");
	t.round_time_in_seconds = 3*60;
	t.golden_score_time_in_seconds = 90;
	AddClass(t);

	t.Init("FU14");
	t.weight_classes.push_back("-30 kg");
	t.weight_classes.push_back("-33 kg");
	t.weight_classes.push_back("-36 kg");
	t.weight_classes.push_back("-40 kg");
	t.weight_classes.push_back("-44 kg");
	t.weight_classes.push_back("-48 kg");
	t.weight_classes.push_back("-52 kg");
	t.weight_classes.push_back("-57 kg");
	t.weight_classes.push_back("-63 kg");
	t.weight_classes.push_back("+63 kg");
	t.round_time_in_seconds = 3*60;
	t.golden_score_time_in_seconds = 90;
	AddClass(t);

	t.Init("MU17");
	t.weight_classes.push_back("-43 kg");
	t.weight_classes.push_back("-46 kg");
	t.weight_classes.push_back("-50 kg");
	t.weight_classes.push_back("-55 kg");
	t.weight_classes.push_back("-60 kg");
	t.weight_classes.push_back("-66 kg");
	t.weight_classes.push_back("-73 kg");
	t.weight_classes.push_back("-81 kg");
	t.weight_classes.push_back("-90 kg");
	t.weight_classes.push_back("+90 kg");
	t.round_time_in_seconds = 4*60;
	t.golden_score_time_in_seconds = 2*60;
	AddClass(t);

	t.Init("FU17");
	t.weight_classes.push_back("-40 kg");
	t.weight_classes.push_back("-44 kg");
	t.weight_classes.push_back("-48 kg");
	t.weight_classes.push_back("-52 kg");
	t.weight_classes.push_back("-57 kg");
	t.weight_classes.push_back("-63 kg");
	t.weight_classes.push_back("-70 kg");
	t.weight_classes.push_back("-78 kg");
	t.weight_classes.push_back("+78 kg");
	t.round_time_in_seconds = 4*60;
	t.golden_score_time_in_seconds = 2*60;
	AddClass(t);

	t.Init("MU20");
	t.weight_classes.push_back("-55 kg");
	t.weight_classes.push_back("-60 kg");
	t.weight_classes.push_back("-66 kg");
	t.weight_classes.push_back("-73 kg");
	t.weight_classes.push_back("-81 kg");
	t.weight_classes.push_back("-90 kg");
	t.weight_classes.push_back("-100 kg");
	t.weight_classes.push_back("+100 kg");
	t.round_time_in_seconds = 4*60;
	t.golden_score_time_in_seconds = 2*60;
	AddClass(t);

	t.Init("FU20");
	t.weight_classes.push_back("-44 kg");
	t.weight_classes.push_back("-48 kg");
	t.weight_classes.push_back("-52 kg");
	t.weight_classes.push_back("-57 kg");
	t.weight_classes.push_back("-63 kg");
	t.weight_classes.push_back("-70 kg");
	t.weight_classes.push_back("-78 kg");
	t.weight_classes.push_back("+78 kg");
	t.round_time_in_seconds = 4*60;
	t.golden_score_time_in_seconds = 2*60;
	AddClass(t);

	t.Init("M");
	t.weight_classes.push_back("-60 kg");
	t.weight_classes.push_back("-66 kg");
	t.weight_classes.push_back("-73 kg");
	t.weight_classes.push_back("-81 kg");
	t.weight_classes.push_back("-90 kg");
	t.weight_classes.push_back("-100 kg");
	t.weight_classes.push_back("+100 kg");
	t.round_time_in_seconds = 5*60;
	t.golden_score_time_in_seconds = 3*60;
	AddClass(t);

	t.Init("F");
	t.weight_classes.push_back("-48 kg");
	t.weight_classes.push_back("-52 kg");
	t.weight_classes.push_back("-57 kg");
	t.weight_classes.push_back("-63 kg");
	t.weight_classes.push_back("-70 kg");
	t.weight_classes.push_back("-78 kg");
	t.weight_classes.push_back("+78 kg");
	t.round_time_in_seconds = 5*60;
	t.golden_score_time_in_seconds = 3*60;
	AddClass(t);
}
