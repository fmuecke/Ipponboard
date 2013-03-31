#include "FightCategoryManager.h"
#include <QObject>

#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/serialization/vector.hpp>
#include <algorithm>
#include <QMessageBox>
#include "../util/path_helpers.h"

using namespace Ipponboard;

const char* const FightCategoryMgr::str_fileName = "categories.xml";

//---------------------------------------------------------
FightCategoryMgr::FightCategoryMgr()
	: m_Categories()
//---------------------------------------------------------
{
	load_categories();
}

//---------------------------------------------------------
FightCategoryMgr::~FightCategoryMgr()
//---------------------------------------------------------
{
	save_categories();
}

//---------------------------------------------------------
bool FightCategoryMgr::GetCategory(int index,
								   Ipponboard::FightCategory& t) const
//---------------------------------------------------------
{
	try
	{
		t = m_Categories.at(index);
	}
	catch (std::out_of_range&)
	{
		return false;
	}

	return true;
}

//---------------------------------------------------------
bool FightCategoryMgr::GetCategory(QString const& name,
								   Ipponboard::FightCategory& c) const
//---------------------------------------------------------
{
	FightCategoryList::const_iterator iter =
		std::find(m_Categories.begin(), m_Categories.end(), name);

	if (iter != m_Categories.end())
	{
		c = *iter;
		return true;
	}

	return false;
}


//---------------------------------------------------------
bool FightCategoryMgr::HasCategory(QString const& name) const
//---------------------------------------------------------
{
	// We could use the new cpp0x lambda expression for that
	// or use operator overloading...
	//
	// return std::find_if( m_Classes.begin(), m_Classes.end(),
	//		[name](TournamentClass t)->bool { return t.name == name; }

	return std::find(m_Categories.begin(), m_Categories.end(), name)
		   != m_Categories.end();
}


//---------------------------------------------------------
void FightCategoryMgr::AddCategory(Ipponboard::FightCategory const& t)
//---------------------------------------------------------
{
	m_Categories.push_back(t);
}


//---------------------------------------------------------
void FightCategoryMgr::AddCategory(QString const& name)
//---------------------------------------------------------
{
	FightCategory t(name);
	AddCategory(t);
}

//---------------------------------------------------------
void FightCategoryMgr::UpdateCategory(const Ipponboard::FightCategory& t)
//---------------------------------------------------------
{
	if (t.ToString().isEmpty())
		return;

	FightCategoryList::iterator iter =
		std::find(m_Categories.begin(), m_Categories.end(), t.ToString());

	if (iter == m_Categories.end())
		Q_ASSERT(!"Critical: weight class not in list!");

	*iter = t;
}

//---------------------------------------------------------
void FightCategoryMgr::RenameCategory(QString const& oldName,
									  QString const& newName)
//---------------------------------------------------------
{
	FightCategoryList::iterator iter =
		std::find(m_Categories.begin(), m_Categories.end(), oldName);

	Q_ASSERT(iter != m_Categories.end() && "Critical: weight class not in list!");
	Q_ASSERT(std::find(m_Categories.begin(), m_Categories.end(), newName) == m_Categories.end()
			 && "Critical: new name already in list!");

	iter->Rename(newName);
}



//---------------------------------------------------------
void FightCategoryMgr::RemoveCategory(QString const& name)
//---------------------------------------------------------
{
	FightCategoryList::iterator iter =
		std::find(m_Categories.begin(), m_Categories.end(), name);

	if (iter == m_Categories.end())
		Q_ASSERT(!"Critical: weight class not in list!");

	m_Categories.erase(iter);
}

//---------------------------------------------------------
void FightCategoryMgr::MoveCategoryUp(QString const& name)
//---------------------------------------------------------
{
	FightCategoryList::iterator iter =
		std::find(m_Categories.begin(), m_Categories.end(), name);

	if (iter == m_Categories.end())
		Q_ASSERT(!"Critical: weight class not in list!");

	if (iter != m_Categories.begin())
		std::swap(*iter, *(iter - 1));
}

//---------------------------------------------------------
void FightCategoryMgr::MoveCategoryDown(QString const& name)
//---------------------------------------------------------
{
	FightCategoryList::iterator iter =
		std::find(m_Categories.begin(), m_Categories.end(), name);

	if (iter == m_Categories.end())
		Q_ASSERT(!"Critical: weight class not in list!");

	if (iter + 1 != m_Categories.end())
		std::swap(*iter, *(iter + 1));
}

//---------------------------------------------------------
void FightCategoryMgr::load_categories()
//---------------------------------------------------------
{
	// open the archive

	const std::string filePath(
		fmu::GetSettingsFilePath(str_fileName));

	std::ifstream ifs(filePath.c_str());

	if (ifs.good())
	{
		try
		{
			boost::archive::xml_iarchive ia(ifs);
			// restore the classes from the xml archive
			ia >> BOOST_SERIALIZATION_NVP(m_Categories);
		}
		catch (std::exception&)
		{
			//QMessageBox::critical(0,
			//	QString(QObject::tr("Error")),
			//	QString(QObject::tr("Unable to parse %1!").arg(
			//			QString::fromStdString(filePath))));

			load_default_categories();
		}
	}
	else
	{
		load_default_categories();
	}

	ifs.close();

	//if( CategoryCount() == 0 )
	//load_default_categories();
}

//---------------------------------------------------------
void FightCategoryMgr::save_categories()
//---------------------------------------------------------
{
	// make an archive

	const std::string filePath(
		fmu::GetSettingsFilePath(str_fileName));

	std::ofstream ofs(filePath.c_str());

	if (ofs.good())
	{
		boost::archive::xml_oarchive oa(ofs);
		oa << BOOST_SERIALIZATION_NVP(m_Categories);
	}
	else
	{
		QMessageBox::critical(0,
							  QString(QObject::tr("Error")),
							  QString(QObject::tr("Unable to save %1!").arg(
										  QString::fromStdString(filePath))));
	}

	ofs.close();
}

//---------------------------------------------------------
bool FightCategoryMgr::CategoriesFromString(std::string const& s)
//---------------------------------------------------------
{
	// open the archive
	std::stringstream ss;
	ss.str(s);

	if (ss.good())
	{
		try
		{
			boost::archive::xml_iarchive ia(ss);
			// restore the classes from the xml archive
			ia >> BOOST_SERIALIZATION_NVP(m_Categories);
			return true;
		}
		catch (std::exception&)
		{
			return false;
		}
	}

	return false;
}


//---------------------------------------------------------
std::string FightCategoryMgr::CategoriesToString()
//---------------------------------------------------------
{
	// make an archive
	std::stringstream ss;

	if (ss.good())
	{
		boost::archive::xml_oarchive oa(ss);
		oa << BOOST_SERIALIZATION_NVP(m_Categories);
	}
	else
	{
		return std::string("");
	}

	return ss.str();
}

//--------------------------------------------------------
void FightCategoryMgr::load_default_categories()
//--------------------------------------------------------
{
	m_Categories.clear();

	FightCategory t("M");
	t.SetWeights("-60;-66;-73;-81;-90;-100;+100");
	t.SetRoundTime(5 * 60);
	t.SetGoldenScoreTime(3 * 60);
	AddCategory(t);

	t = FightCategory("MU20");
	t.SetWeights("-55;-60;-66;-73;-81;-90;-100;+100");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(2 * 60);
	AddCategory(t);

	t = FightCategory("MU19");
	t.SetWeights("-55;-60;-66;-73;-81;-90;-100;+100");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(2 * 60);
	AddCategory(t);

	t = FightCategory("MU17");
	t.SetWeights("-43;-46;-50;-55;-60;-66;-73;-81;-90;+90");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(2 * 60);
	AddCategory(t);

	t = FightCategory("MU16");
	t.SetWeights("-40;-43;-46;-50;-55;-60;-66;-73;-81;+81");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(2 * 60);
	AddCategory(t);

	t = FightCategory("MU14");
	t.SetWeights("-31;-34;-37;-40;-43;-46;-50;-55;-60;+60");
	t.SetRoundTime(3 * 60);
	t.SetGoldenScoreTime(90);
	AddCategory(t);

	t = FightCategory("F");
	t.SetWeights("-48;-52;-57;-63;-70;-78;+78");
	t.SetRoundTime(5 * 60);
	t.SetGoldenScoreTime(3 * 60);
	AddCategory(t);

	t = FightCategory("FU20");
	t.SetWeights("-44;-48;-52;-57;-63;-70;-78;+78");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(2 * 60);
	AddCategory(t);

	t = FightCategory("FU19");
	t.SetWeights("-44;-48;-52;-57;-63;-70;-78;+78");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(2 * 60);
	AddCategory(t);

	t = FightCategory("FU17");
	t.SetWeights("-40;-44;-48;-52;-57;-63;-70;-78;+78");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(2 * 60);
	AddCategory(t);

	t = FightCategory("FU16");
	t.SetWeights("-40;-44;-48;-52;-57;-63;-70;+70");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(2 * 60);
	AddCategory(t);

	t = FightCategory("FU14");
	t.SetWeights("-30;-33;-36;-40;-44;-48;-52;-57;-63;+63");
	t.SetRoundTime(3 * 60);
	t.SetGoldenScoreTime(90);
	AddCategory(t);
}
