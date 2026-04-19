// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#pragma once

#include "../core/ContestCategory.h"

#include <QString>
#include <QStringList>
#include <QTime>
#include <memory>
#include <string>

// forwards
class QListWidgetItem;

namespace Ipponboard
{

class ContestCategoryMgr
{
    //Q_OBJECT
  public:
    static const char* const str_legacyConfigFileName;
    static const char* const str_configFileName;

    ContestCategoryMgr();
    virtual ~ContestCategoryMgr();

    typedef std::shared_ptr<ContestCategoryMgr> Ptr;

    void LoadCategories();
    void SaveCategories();

    bool GetCategory(int index, ContestCategory& t) const;
    bool GetCategory(QString const& name, ContestCategory& t) const;

    bool HasCategory(QString const& name) const;
    void AddCategory(ContestCategory const& t);
    void AddCategory(QString const& name);
    void UpdateCategory(ContestCategory const& t);
    void UpdateCategory(QString const& oldName, ContestCategory const& t);
    void RenameCategory(QString const& oldName, QString const& newName);
    void RemoveCategory(QString const& name);
    int CategoryCount() const { return m_Categories.size(); }

  private:
    CategoryList m_Categories;
};

} // namespace Ipponboard
