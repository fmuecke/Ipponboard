#ifndef BASE__FIGHTCATEGORYMANAGER_H_
#define BASE__FIGHTCATEGORYMANAGER_H_

#include <QString>
#include <QTime>
#include <QStringList>
#include <vector>
#include <string>
#include <memory>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>

#include "../core/fightcategory.h"

// forwards
class QListWidgetItem;

namespace Ipponboard
{

class FightCategoryMgr
{
	//Q_OBJECT
public:
	static const char* const str_fileName;

	FightCategoryMgr();
	virtual ~FightCategoryMgr();

    typedef std::tr1::shared_ptr<FightCategoryMgr> Ptr;

	bool GetCategory(int index, FightCategory& t) const;
	bool GetCategory(QString const& name, FightCategory& t) const;

	bool HasCategory(QString const& name) const;
	void AddCategory(FightCategory const& t);
	void AddCategory(QString const& name);
	void UpdateCategory(FightCategory const& t);
	void UpdateCategory(QString const& oldName, FightCategory const& t);
	void RenameCategory(QString const& oldName, QString const& newName);
	void RemoveCategory(QString const& name);
	void MoveCategoryUp(QString const& name);
	void MoveCategoryDown(QString const& name);
	int CategoryCount() const { return m_Categories.size(); }

	bool CategoriesFromString(std::string const& s);
	std::string const CategoriesToString();

private:
	void load_categories();
	void save_categories();
	void load_default_categories();

	FightCategoryList m_Categories;
};

} // namespace Ipponboard

#endif  // BASE__FIGHTCATEGORYMANAGER_H_
