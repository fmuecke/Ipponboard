// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/debug.h"
#include "FightCategoryManager.h"
#include <QObject>

#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <QMessageBox>
#include <QFile>
#include "../util/path_helpers.h"
#include "FightCategoryParser.h"

using namespace Ipponboard;

const char* const FightCategoryMgr::str_fileName = "categories.json";
const char* const FightCategoryMgr::str_configFileName = "categories.config";

//---------------------------------------------------------
FightCategoryMgr::FightCategoryMgr()
	: m_Categories()
//---------------------------------------------------------
{
    TRACE(2, "FightCategoryMgr::FightCategoryMgr()");
    load_categories();
}

//---------------------------------------------------------
FightCategoryMgr::~FightCategoryMgr()
//---------------------------------------------------------
{
    TRACE(2, "FightCategoryMgr::~FightCategoryMgr()");
    save_categories();
}

//---------------------------------------------------------
bool FightCategoryMgr::GetCategory(int index,
								   Ipponboard::FightCategory& t) const
//---------------------------------------------------------
{
    TRACE(2, "FightCategoryMgr::GetCategory()");
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
    TRACE(2, "FightCategoryMgr::GetCategory(name=%s)", name.toUtf8().data());

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
    TRACE(2, "FightCategoryMgr::HasCategory(name=%s)", name.toUtf8().data());
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
    TRACE(2, "FightCategoryMgr::AddCategory()");
    m_Categories.push_back(t);
}


//---------------------------------------------------------
void FightCategoryMgr::AddCategory(QString const& name)
//---------------------------------------------------------
{
    TRACE(2, "FightCategoryMgr::AddCategory(name=%s)", name.toUtf8().data());
    FightCategory t(name);
	AddCategory(t);
}

//---------------------------------------------------------
void FightCategoryMgr::UpdateCategory(const Ipponboard::FightCategory& t)
//---------------------------------------------------------
{
    TRACE(2, "FightCategoryMgr::UpdateCategory()");
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
    TRACE(2, "FightCategoryMgr::RenameCategory(oldName=%s, newName=%s)", oldName.toUtf8().data(), newName.toUtf8().data());
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
    TRACE(2, "FightCategoryMgr::RemoveCategory(name=%s)", name.toUtf8().data());
    FightCategoryList::iterator iter =
		std::find(m_Categories.begin(), m_Categories.end(), name);

	if (iter == m_Categories.end())
		Q_ASSERT(!"Critical: weight class not in list!");

	m_Categories.erase(iter);
}

//---------------------------------------------------------
void FightCategoryMgr::load_categories()
//---------------------------------------------------------
{
    TRACE(2, "FightCategoryMgr::load_categories()");
    auto configFile {fm::GetSettingsFilePath(str_configFileName)};
	auto legacyFile {fm::GetSettingsFilePath(str_fileName)};

	try
	{
		if (QFile::exists(configFile.c_str()))
		{
			m_Categories = FightCategoryParser::ParseIniFile(configFile.c_str());
		}
		else
		{
			if (!QFile::exists(legacyFile.c_str()))
			{
				load_default_categories();
			}
			else
			{
				m_Categories = FightCategoryParser::ParseJsonFile(legacyFile.c_str());
			}
		}
	}
	catch (std::exception const& e)
	{
		QMessageBox::critical(0,
							  QString(QObject::tr("Error")),
							  QString(QObject::tr("Unable to load fight categories:\n%1\n\nRestoring defaults.").arg(
										  QString::fromStdString(e.what()))));

		load_default_categories();
	}
}

//---------------------------------------------------------
void FightCategoryMgr::save_categories()
//---------------------------------------------------------
{
    TRACE(2, "FightCategoryMgr::save_categories()");
    std::string filePath {fm::GetSettingsFilePath(str_configFileName)};
	FightCategoryParser::ToIniFile(filePath.c_str(), m_Categories);
}

//---------------------------------------------------------
bool FightCategoryMgr::CategoriesFromString(std::string const& s)
//---------------------------------------------------------
{
    TRACE(2, "FightCategoryMgr::CategoriesFromString(s=%s)", s);
    try
	{
		auto cats = FightCategoryParser::ParseJsonString(s);

		for (auto const & cat : cats)
		{
			AddCategory(cat);
		}
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

//---------------------------------------------------------
std::string FightCategoryMgr::ConvertCategoriesToString_WITH_GUI_ERROR()
//---------------------------------------------------------
{
    TRACE(2, "FightCategoryMgr::ConvertCategoriesToString_WITH_GUI_ERROR()");
    try
	{
		return FightCategoryParser::ToJsonString(m_Categories);
	}
	catch (std::exception const& e)
	{
		QMessageBox::critical(0,
							  QString(QObject::tr("Error")),
							  QString(QObject::tr("Unable to write fight categories:\n%1").arg(
										  QString::fromStdString(e.what()))));
	}

	return std::string();
}

//--------------------------------------------------------
void FightCategoryMgr::load_default_categories()
//--------------------------------------------------------
{
    TRACE(2, "FightCategoryMgr::load_default_categories()");
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
