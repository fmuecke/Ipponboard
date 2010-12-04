#include "classmanager.h"
#include <QObject>

#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/serialization/vector.hpp>
#include <algorithm>
#include <QMessageBox>

using namespace Ipponboard;

const char* const WeightClassManager::str_fileName = "classes.xml";
const char* const WeightClassManager::str_defaultClass = "default";

//---------------------------------------------------------
WeightClassManager::WeightClassManager()
	: m_Classes()
//---------------------------------------------------------
{
	LoadClasses_();
}

//---------------------------------------------------------
WeightClassManager::~WeightClassManager()
//---------------------------------------------------------
{
	SaveClasses_();
}

//---------------------------------------------------------
bool WeightClassManager::GetClass( int index,
									   Ipponboard::WeightClass& t ) const
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
bool WeightClassManager::GetClass( QString const& name,
									   Ipponboard::WeightClass& c ) const
//---------------------------------------------------------
{
	WeightClassList::const_iterator iter =
		std::find(m_Classes.begin(), m_Classes.end(), name );

	if( iter != m_Classes.end() )
	{
		c = *iter;
		return true;
	}
	return false;
}


//---------------------------------------------------------
bool WeightClassManager::HasClass( QString const& name ) const
//---------------------------------------------------------
{
	// We could use the new cpp0x lambda expression for that
	// or use operator overloading...
	//
	// return std::find_if( m_Classes.begin(), m_Classes.end(),
	//		[name](TournamentClass t)->bool { return t.name == name; }

	return std::find( m_Classes.begin(), m_Classes.end(), name )
			!= m_Classes.end();
}


//---------------------------------------------------------
void WeightClassManager::AddClass( Ipponboard::WeightClass const& t )
//---------------------------------------------------------
{
	m_Classes.push_back(t);
}


//---------------------------------------------------------
void WeightClassManager::AddClass( QString const& name )
//---------------------------------------------------------
{
	WeightClass t(name);
	AddClass(t);
}

//---------------------------------------------------------
void WeightClassManager::UpdateClass( const Ipponboard::WeightClass& t )
//---------------------------------------------------------
{
	if( t.ToString().isEmpty() )
		return;

	WeightClassList::iterator iter =
		std::find(m_Classes.begin(), m_Classes.end(), t.ToString());

	if( iter == m_Classes.end() )
		Q_ASSERT(!"Critical: tournament class not in list!");

	*iter = t;
}

//---------------------------------------------------------
void WeightClassManager::RemoveClass( QString const& name )
//---------------------------------------------------------
{
	WeightClassList::iterator iter =
		std::find(m_Classes.begin(), m_Classes.end(), name);

	if( iter == m_Classes.end() )
		Q_ASSERT(!"Critical: tournament class not in list!");

	m_Classes.erase( iter );
}

//---------------------------------------------------------
void WeightClassManager::LoadClasses_()
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
void WeightClassManager::SaveClasses_()
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
			QString(QObject::tr("Unable to save ") + str_fileName + "!"));
	}
	ofs.close();
}

//---------------------------------------------------------
bool WeightClassManager::ClassesFromString( std::string const& s )
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
std::string const WeightClassManager::ClassesToString()
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
void WeightClassManager::LoadDefaultClasses_()
//--------------------------------------------------------
{
	m_Classes.clear();

	WeightClass t(str_defaultClass);
	t.AddWeight("-66");
	t.AddWeight("-73");
	t.AddWeight("-81");
	t.AddWeight("-90");
	t.AddWeight("-100");
	t.AddWeight("+100");
	t.SetRoundTime(5*60);
	t.SetGoldenScoreTime(3*60);
	AddClass(t);

	t = WeightClass("MU14");
	t.AddWeight("-31");
	t.AddWeight("-34");
	t.AddWeight("-37");
	t.AddWeight("-40");
	t.AddWeight("-43");
	t.AddWeight("-46");
	t.AddWeight("-50");
	t.AddWeight("-55");
	t.AddWeight("-60");
	t.AddWeight("+60");
	t.SetRoundTime(3*60);
	t.SetGoldenScoreTime(90);
	AddClass(t);

	t = WeightClass("FU14");
	t.AddWeight("-30");
	t.AddWeight("-33");
	t.AddWeight("-36");
	t.AddWeight("-40");
	t.AddWeight("-44");
	t.AddWeight("-48");
	t.AddWeight("-52");
	t.AddWeight("-57");
	t.AddWeight("-63");
	t.AddWeight("+63");
	t.SetRoundTime(3*60);
	t.SetGoldenScoreTime(90);
	AddClass(t);

	t = WeightClass("MU17");
	t.AddWeight("-43");
	t.AddWeight("-46");
	t.AddWeight("-50");
	t.AddWeight("-55");
	t.AddWeight("-60");
	t.AddWeight("-66");
	t.AddWeight("-73");
	t.AddWeight("-81");
	t.AddWeight("-90");
	t.AddWeight("+90");
	t.SetRoundTime(4*60);
	t.SetGoldenScoreTime(2*60);
	AddClass(t);

	t = WeightClass("FU17");
	t.AddWeight("-40");
	t.AddWeight("-44");
	t.AddWeight("-48");
	t.AddWeight("-52");
	t.AddWeight("-57");
	t.AddWeight("-63");
	t.AddWeight("-70");
	t.AddWeight("-78");
	t.AddWeight("+78");
	t.SetRoundTime(4*60);
	t.SetGoldenScoreTime(2*60);
	AddClass(t);

	t = WeightClass("MU20");
	t.AddWeight("-55");
	t.AddWeight("-60");
	t.AddWeight("-66");
	t.AddWeight("-73");
	t.AddWeight("-81");
	t.AddWeight("-90");
	t.AddWeight("-100");
	t.AddWeight("+100");
	t.SetRoundTime(4*60);
	t.SetGoldenScoreTime(2*60);
	AddClass(t);

	t = WeightClass("FU20");
	t.AddWeight("-44");
	t.AddWeight("-48");
	t.AddWeight("-52");
	t.AddWeight("-57");
	t.AddWeight("-63");
	t.AddWeight("-70");
	t.AddWeight("-78");
	t.AddWeight("+78");
	t.SetRoundTime(4*60);
	t.SetGoldenScoreTime(2*60);
	AddClass(t);

	t = WeightClass("M");
	t.AddWeight("-60");
	t.AddWeight("-66");
	t.AddWeight("-73");
	t.AddWeight("-81");
	t.AddWeight("-90");
	t.AddWeight("-100");
	t.AddWeight("+100");
	t.SetRoundTime(5*60);
	t.SetGoldenScoreTime(3*60);
	AddClass(t);

	t = WeightClass("F");
	t.AddWeight("-48");
	t.AddWeight("-52");
	t.AddWeight("-57");
	t.AddWeight("-63");
	t.AddWeight("-70");
	t.AddWeight("-78");
	t.AddWeight("+78");
	t.SetRoundTime(5*60);
	t.SetGoldenScoreTime(3*60);
	AddClass(t);
}
