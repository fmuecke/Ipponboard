#include "FightCategoryManager.h"
#include <QObject>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <QMessageBox>
#include "../util/path_helpers.h"
#include "../util/qt_helpers.hpp"
#include "../util/json.hpp"

using namespace Ipponboard;

const char* const FightCategoryMgr::str_fileName = "categories.json";

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
	const std::string filePath(fm::GetSettingsFilePath(str_fileName));

	try
	{
		auto jsonCategories = fm::Json::ReadFile(filePath.c_str());

		for (fm::Json::Value const & jsonCat : jsonCategories)
		{
			FightCategory cat(fm::qt::from_utf8_str(jsonCat["name"].asString()));
			cat.SetRoundTime(jsonCat["round_time_secs"].asInt());
			cat.SetGoldenScoreTime(jsonCat["golden_score_time_secs"].asInt());
			cat.SetWeights(fm::qt::from_utf8_str(jsonCat["weights"].asString()));

			AddCategory(cat);
		}
	}
	catch (fm::Json::Exception const& e)
	{
		QMessageBox::critical(0,
							  QString(QObject::tr("Error")),
							  QString(QObject::tr("Unable to read fight categories:\n%1\n\nRestoring defaults.").arg(
										  QString::fromStdString(e.what()))));

		load_default_categories();
	}
	catch (std::exception const& e)
	{
		QMessageBox::critical(0,
							  QString(QObject::tr("Error")),
							  QString(QObject::tr("Unable to parse fight categories:\n%1\n\nRestoring defaults.").arg(
										  QString::fromStdString(e.what()))));

		load_default_categories();
	}
}

//---------------------------------------------------------
void FightCategoryMgr::save_categories()
//---------------------------------------------------------
{
	const std::string filePath(fm::GetSettingsFilePath(str_fileName));
    const std::string& jsonString = ConvertCategoriesToString_WITH_GUI_ERROR();
	fm::Json::WriteFile(filePath.c_str(), jsonString);
}

//---------------------------------------------------------
bool FightCategoryMgr::CategoriesFromString(std::string const& s)
//---------------------------------------------------------
{
	try
	{
		auto jsonCategories = fm::Json::ReadString(s);

		for (fm::Json::Value const & jsonCat : jsonCategories)
		{
			FightCategory cat(fm::qt::from_utf8_str(jsonCat["name"].asString()));
			cat.SetRoundTime(jsonCat["round_time_secs"].asInt());
			cat.SetGoldenScoreTime(jsonCat["golden_score_time_secs"].asInt());
			cat.SetWeights(fm::qt::from_utf8_str(jsonCat["weights"].asString()));

			AddCategory(cat);
		}
	}
	catch (fm::Json::Exception const& e)
	{
		QMessageBox::critical(0,
							  QString(QObject::tr("Error")),
							  QString(QObject::tr("Unable to read fight categories:\n%1\n\nRestoring defaults.").arg(
										  QString::fromStdString(e.what()))));

		return false;
	}
	catch (std::exception const& e)
	{
		QMessageBox::critical(0,
							  QString(QObject::tr("Error")),
							  QString(QObject::tr("Unable to parse fight categories:\n%1\n\nRestoring defaults.").arg(
										  QString::fromStdString(e.what()))));

		return false;
	}

	return true;
}

std::string FightCategoryMgr::categories_to_str()
{
    fm::Json::Value jsonCategories;

    for (FightCategory const & cat : m_Categories)
    {
        fm::Json::Value jsonCat;
        jsonCat["name"] = fm::qt::to_utf8_str(cat.ToString());
        jsonCat["round_time_secs"] = cat.GetRoundTime();
        jsonCat["golden_score_time_secs"] = cat.GetGoldenScoreTime();
        jsonCat["weights"] = fm::qt::to_utf8_str(cat.GetWeights());

        jsonCategories.append(jsonCat);
    }

    return jsonCategories.toStyledString();
}

//---------------------------------------------------------
std::string FightCategoryMgr::ConvertCategoriesToString_WITH_GUI_ERROR()
//---------------------------------------------------------
{
	try
	{
        return categories_to_str();
	}
	catch (fm::Json::Exception const& e)
	{
		QMessageBox::critical(0,
							  QString(QObject::tr("Error")),
							  QString(QObject::tr("Unable to write fight categories:\n%1").arg(
										  QString::fromStdString(e.what()))));
	}
	catch (std::exception const& e)
	{
		QMessageBox::critical(0,
							  QString(QObject::tr("Error")),
							  QString(QObject::tr("Unable to convert fight categories:\n%1").arg(
										  QString::fromStdString(e.what()))));
	}

	return std::string();
}

//--------------------------------------------------------
void FightCategoryMgr::load_default_categories()
//--------------------------------------------------------
{
	m_Categories.clear();

	FightCategory t("M");
	t.SetWeights("-60;-66;-73;-81;-90;-100;+100");
	t.SetRoundTime(4 * 60);
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
	t.SetRoundTime(4 * 60);
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
