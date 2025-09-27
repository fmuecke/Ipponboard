// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "FightCategoryManager.h"

#include <QObject>
#include <QMessageBox>
#include <QFile>
#include <QDebug>

#include "../util/path_helpers.h"
#include "FightCategoryParser.h"

#include <stdexcept>
#include <algorithm>

using namespace Ipponboard;

const char* const FightCategoryMgr::str_legacyConfigFileName = "categories.json"; // used prior to version 1.10
const char* const FightCategoryMgr::str_configFileName = "categories.config";

//---------------------------------------------------------
FightCategoryMgr::FightCategoryMgr() : m_Categories()
//---------------------------------------------------------
{
	LoadCategories();
}

//---------------------------------------------------------
FightCategoryMgr::~FightCategoryMgr()
//---------------------------------------------------------
{
	SaveCategories();
}

//---------------------------------------------------------
bool FightCategoryMgr::GetCategory(int index, Ipponboard::FightCategory& t) const
//---------------------------------------------------------
{
	try
	{
		t = m_Categories.at(static_cast<size_t>(index));
	}
	catch (std::out_of_range&)
	{
		return false;
	}

	return true;
}

//---------------------------------------------------------
bool FightCategoryMgr::GetCategory(QString const& name, Ipponboard::FightCategory& c) const
//---------------------------------------------------------
{
	FightCategoryList::const_iterator iter = std::find(m_Categories.begin(), m_Categories.end(), name);

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

	return std::find(m_Categories.begin(), m_Categories.end(), name) != m_Categories.end();
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
	if (t.ToString().isEmpty()) return;

	FightCategoryList::iterator iter = std::find(m_Categories.begin(), m_Categories.end(), t.ToString());

	if (iter == m_Categories.end()) Q_ASSERT(!"Critical: weight class not in list!");

	*iter = t;
}

//---------------------------------------------------------
void FightCategoryMgr::RenameCategory(QString const& oldName, QString const& newName)
//---------------------------------------------------------
{
	FightCategoryList::iterator iter = std::find(m_Categories.begin(), m_Categories.end(), oldName);

	Q_ASSERT(iter != m_Categories.end() && "Critical: weight class not in list!");
	Q_ASSERT(std::find(m_Categories.begin(), m_Categories.end(), newName) == m_Categories.end() &&
	         "Critical: new name already in list!");

	iter->Rename(newName);
}

//---------------------------------------------------------
void FightCategoryMgr::RemoveCategory(QString const& name)
//---------------------------------------------------------
{
	FightCategoryList::iterator iter = std::find(m_Categories.begin(), m_Categories.end(), name);

	if (iter == m_Categories.end()) Q_ASSERT(!"Critical: weight class not in list!");

	m_Categories.erase(iter);
}

//---------------------------------------------------------
void FightCategoryMgr::LoadCategories()
//---------------------------------------------------------
{
	auto configFile{ fm::GetSettingsFilePath(str_configFileName) };
	auto legacyFile{ fm::GetSettingsFilePath(str_legacyConfigFileName) };

	try
	{
		if (QFile::exists(configFile))
		{
			qInfo() << "Reading categories from config:" << configFile;
			m_Categories = FightCategoryParser::ParseIniFile(configFile);
		}
		else
		{
			if (!QFile::exists(legacyFile))
			{
				qInfo() << "Loading default categories";
				load_default_categories();
			}
			else
			{
				qInfo() << "Reading categories from legacy config:" << legacyFile;
				m_Categories = FightCategoryParser::ParseJsonFile(legacyFile);
			}
		}
	}
	catch (std::exception const& e)
	{
		qWarning() << "Error loading categories, restoring defaults";
		QMessageBox::critical(nullptr,
		                      QString(QObject::tr("Error")),
		                      QString(QObject::tr("Unable to load fight categories:\n%1\n\nRestoring defaults.")
		                                  .arg(QString::fromStdString(e.what()))));

		load_default_categories();
	}
}

//---------------------------------------------------------
void FightCategoryMgr::SaveCategories()
//---------------------------------------------------------
{
	qInfo() << "Saving categories to:" << str_configFileName;
	auto filePath{ fm::GetSettingsFilePath(str_configFileName) };
	FightCategoryParser::ToIniFile(filePath, m_Categories);
}

//--------------------------------------------------------
void FightCategoryMgr::load_default_categories()
//--------------------------------------------------------
{
	m_Categories.clear();

	FightCategory t("M");
	t.SetWeights("-60kg;-66kg;-73kg;-81kg;-90kg;-100kg;+100kg");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(0);
	AddCategory(t);

	t = FightCategory("MU21");
	t.SetWeights("-60kg;-66kg;-73kg;-81kg;-90kg;-100kg;+100kg");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(0);
	AddCategory(t);

	t = FightCategory("MU18");
	t.SetWeights("-46kg;-50kg;-55kg;-60kg;-66kg;-73kg;-81kg;-90kg;+90kg");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(0);
	AddCategory(t);

	t = FightCategory("MU15");
	t.SetWeights("-34kg;-37kg;-40kg;-43kg;-46kg;-50kg;-55kg;-60kg;-66kg;+66kg");
	t.SetRoundTime(3 * 60);
	t.SetGoldenScoreTime(3 * 60);
	AddCategory(t);

	t = FightCategory("MU13");
	t.SetWeights("-28kg;-31kg;-34kg;-37kg;-40kg;-43kg;-46kg;-50kg;-55kg;+55kg");
	t.SetRoundTime(3 * 60);
	t.SetGoldenScoreTime(0);
	AddCategory(t);

	t = FightCategory("F");
	t.SetWeights("-48kg;-52kg;-57kg;-63kg;-70kg;-78kg;+78kg");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(0);
	AddCategory(t);

	t = FightCategory("FU21");
	t.SetWeights("-48kg;-52kg;-57kg;-63kg;-70kg;-78kg;+78kg");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(0);
	AddCategory(t);

	t = FightCategory("FU18");
	t.SetWeights("-40kg;-44kg;-48kg;-52kg;-57kg;-63kg;-70kg;-78kg;+78kg");
	t.SetRoundTime(4 * 60);
	t.SetGoldenScoreTime(0);
	AddCategory(t);

	t = FightCategory("FU15");
	t.SetWeights("-33kg;-36kg;-40kg;-44kg;-48kg;-52kg;-57kg;-63kg;+63kg");
	t.SetRoundTime(3 * 60);
	t.SetGoldenScoreTime(3 * 60);
	AddCategory(t);

	t = FightCategory("FU13");
	t.SetWeights("-27kg;-30kg;-33kg;-36kg;-40kg;-44kg;-48kg;-52kg;-57kg;+57kg");
	t.SetRoundTime(3 * 60);
	t.SetGoldenScoreTime(0);
	AddCategory(t);
}
