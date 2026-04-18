// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "FightCategoryManager.h"

#include "../util/path_helpers.h"
#include "FightCategoryParser.h"

#include <QDebug>
#include <QFile>
#include <QMessageBox>
#include <QObject>
#include <algorithm>
#include <stdexcept>

using namespace Ipponboard;

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
    //		[name](CompetitionClass t)->bool { return t.name == name; }

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
    if (t.ToString().isEmpty())
        return;

    FightCategoryList::iterator iter =
        std::find(m_Categories.begin(), m_Categories.end(), t.ToString());

    if (iter == m_Categories.end())
        Q_ASSERT(!"Critical: weight class not in list!");

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

    if (iter == m_Categories.end())
        Q_ASSERT(!"Critical: weight class not in list!");

    m_Categories.erase(iter);
}

//---------------------------------------------------------
void FightCategoryMgr::LoadCategories()
//---------------------------------------------------------
{
    auto configFile = fm::ResolveConfigFileForRead(str_configFileName);

    qInfo() << "Reading categories from config:" << configFile;
    if (QFile::exists(configFile))
    {
        m_Categories = FightCategoryParser::ParseIniFile(configFile);
    }
    else
    {
        qWarning() << "Error loading categories:" << configFile;
    }
}

//---------------------------------------------------------
void FightCategoryMgr::SaveCategories()
//---------------------------------------------------------
{
    qInfo() << "Saving categories to:" << str_configFileName;
    auto filePath{ fm::GetConfigFilePath(str_configFileName) };
    FightCategoryParser::ToIniFile(filePath, m_Categories);
}
