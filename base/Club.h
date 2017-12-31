#ifndef BASE__CLUB_H_
#define BASE__CLUB_H_

#include <QString>
#include <vector>

namespace Ipponboard
{
struct Club
{
	Club()
		: name()
		, logoFile()
		, address()
	{}

	Club(QString const& id, QString const& logo)
		: name(id)
		, logoFile(logo)
		, address()
	{}

	QString name;
	QString logoFile;
	QString address;

	inline bool operator<(const Club& rhs) const
	{
		return this->ToString() < rhs.ToString();
	}

	inline QString ToString() const { return name; }
};

using ClubList = std::vector<Club>;

} // namespace Ipponboard

#endif  // BASE__CLUB_H_
