#include "clubmanager.h"
#include <QMessageBox>

#include <iostream>
#include <fstream>
#include "../util/qstring_serialization.h"
#include <boost/serialization/vector.hpp>
#include <algorithm>


using namespace Ipponboard;

//---------------------------------------------------------
ClubManager::ClubManager()
	: m_Clubs()
//---------------------------------------------------------
{
	LoadClubs_();
}

//---------------------------------------------------------
ClubManager::~ClubManager()
//---------------------------------------------------------
{
	SaveClubs_();
}

//---------------------------------------------------------
bool ClubManager::GetClub( int index, Ipponboard::Club& club ) const
//---------------------------------------------------------
{
	try
	{
		club = m_Clubs.at(index);
	}
	catch(std::out_of_range& )
	{
		return false;
	}
	return true;
}

//---------------------------------------------------------
const QString ClubManager::GetLogo( const QString& clubName ) const
//---------------------------------------------------------
{
	for( unsigned i(0); i< m_Clubs.size(); ++i )
	{
		if( clubName == m_Clubs.at(i).name )
			return m_Clubs.at(i).logoFile;
	}
	return QString();
}

//---------------------------------------------------------
void ClubManager::AddClub( const Ipponboard::Club& club )
//---------------------------------------------------------
{
	m_Clubs.push_back(club);
}

//---------------------------------------------------------
void ClubManager::UpdateClub( int index, const Ipponboard::Club& club )
//---------------------------------------------------------
{
	m_Clubs.at(index) = club;
}

//---------------------------------------------------------
void ClubManager::RemoveClub( int index )
//---------------------------------------------------------
{
	Q_ASSERT(index<m_Clubs.size() && index >= 0);

	m_Clubs.erase( m_Clubs.begin() + index );
}

//---------------------------------------------------------
void ClubManager::LoadClubs_()
//---------------------------------------------------------
{
	// open the archive
	std::ifstream ifs("clubs.xml");
	if( ifs.good() )
	{
		try
		{
			boost::archive::xml_iarchive ia(ifs);
			// restore the clubs from the xml archive
			ia >> BOOST_SERIALIZATION_NVP(m_Clubs);
		}
		catch( std::exception& )
		{
			QMessageBox::critical(0, QString("Error"), QString("Unable to parse clubs.xml!"));
		}
	}
	ifs.close();
}

//---------------------------------------------------------
void ClubManager::SaveClubs_()
//---------------------------------------------------------
{
	// make an archive
	std::ofstream ofs("clubs.xml");
	if( ofs.good() )
	{
		boost::archive::xml_oarchive oa(ofs);
		oa << BOOST_SERIALIZATION_NVP(m_Clubs);
	}
	else
	{
		QMessageBox::critical(0, QString("Error"), QString("Unable to save clubs.xml!"));
	}
	ofs.close();
}
