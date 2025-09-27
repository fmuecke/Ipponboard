// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#ifndef BASE__FIGHTCATEGORY_H_
#define BASE__FIGHTCATEGORY_H_

#include <QString>
#include <QStringList>
#include <vector>
#include <string>

namespace Ipponboard
{

class FightCategory
{
  public:
	// Note: class uses std::string instead of QString for
	// better serialization support
	typedef std::vector<std::string> StringList;

	FightCategory() {}

	explicit FightCategory(QString const& new_name)
	    : name(new_name.toStdString()), round_time_secs(0), golden_score_time_secs(0), weights()
	{}

	inline bool operator==(std::string const& n) const { return name == n; }
	inline bool operator==(QString const& n) const { return name == n.toStdString(); }

	QString ToString() const { return QString::fromStdString(name); }
	void Rename(QString const& newName) { name.assign(newName.toStdString()); }

	int GetRoundTime() const { return round_time_secs; }
	int GetGoldenScoreTime() const { return golden_score_time_secs; }
	QString GetRoundTimeStr() const;
	QString GetGoldenScoreTimeStr() const;
	QString GetWeights() const { return QString::fromStdString(weights); }
	void SetRoundTime(int secs) { round_time_secs = secs; }
	void SetRoundTime(QString const& str);
	void SetGoldenScoreTime(int secs) { golden_score_time_secs = secs; }
	void SetGoldenScoreTime(QString const& str);
	void SetWeights(QString const& s) { weights.assign(s.toStdString()); }

	QStringList GetWeightsList() const;

  private:
	std::string name;
	int round_time_secs{ 0 };
	int golden_score_time_secs{ 0 };
	std::string weights;
};

typedef std::vector<FightCategory> FightCategoryList;

} // namespace Ipponboard

#endif // BASE__FIGHTCATEGORY_H_
