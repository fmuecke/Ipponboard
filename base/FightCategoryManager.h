#ifndef BASE__FIGHTCATEGORYMANAGER_H_
#define BASE__FIGHTCATEGORYMANAGER_H_

#include "../core/FightCategory.h"

#include <QString>
#include <QTime>
#include <QStringList>
#include <vector>
#include <string>
#include <memory>

// forwards
class QListWidgetItem;

namespace Ipponboard
{

class FightCategoryMgr
{
	//Q_OBJECT
public:
	static const char* const str_fileName;
    static const char* const str_configFileName;

	FightCategoryMgr();
	virtual ~FightCategoryMgr();

	typedef std::shared_ptr<FightCategoryMgr> Ptr;

	bool GetCategory(int index, FightCategory& t) const;
	bool GetCategory(QString const& name, FightCategory& t) const;

	bool HasCategory(QString const& name) const;
	void AddCategory(FightCategory const& t);
	void AddCategory(QString const& name);
	void UpdateCategory(FightCategory const& t);
	void UpdateCategory(QString const& oldName, FightCategory const& t);
	void RenameCategory(QString const& oldName, QString const& newName);
	void RemoveCategory(QString const& name);
	int CategoryCount() const { return m_Categories.size(); }

	bool CategoriesFromString(std::string const& s);
    std::string ConvertCategoriesToString_WITH_GUI_ERROR();

private:
	void load_categories();
	void save_categories();
	void load_default_categories();

	FightCategoryList m_Categories;
};

} // namespace Ipponboard

#endif  // BASE__FIGHTCATEGORYMANAGER_H_
