// Copyright 2020 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "../util/catch2/catch.hpp"
#include "../base/FighterManager.h"

#include "../base/FighterManager.cpp"

using Ipponboard::FighterManager;

TEST_CASE("CSV header format is @FIRSTNAME;@LASTNAME;@CLUB;@WEIGHT;@CATEGORY")
{
	REQUIRE(FighterManager::GetCsvHeaderFormat() == "@FIRSTNAME;@LASTNAME;@CLUB;@WEIGHT;@CATEGORY");
}

TEST_CASE("Add new fighter increases fighter count")
{
	FighterManager m;

	auto oldCount = m.m_fighters.size();
	auto f = m.AddNewFighter();
	auto newCount = m.m_fighters.size();
	REQUIRE(oldCount < newCount);
}

TEST_CASE("Addding 20 new fighter creates random non-duplicate fighter names")
{
	FighterManager m;

	auto oldCount = m.m_fighters.size();
	for (int i = 0; i<20; ++i)
	{
		m.AddNewFighter();
	}
	auto newCount = m.m_fighters.size();
	REQUIRE(oldCount + 20 == newCount);
}

TEST_CASE("LoadFighters creates dummy fighter, if none could be loaded")
{
	FighterManager m;

	REQUIRE(m.m_fighters.empty());

	QString errorMsg;
	m.LoadFighters("file does not exist.exe", errorMsg);

	REQUIRE(m.m_fighters.size() == 1);
	auto f = *m.m_fighters.begin();
	REQUIRE(f.club.isEmpty());
	REQUIRE(f.weight.isEmpty());
	REQUIRE(f.category.isEmpty());
	REQUIRE(f.first_name.toStdString() == "FIRST NAME");
	REQUIRE(f.last_name.toStdString() == "LAST NAME");
}

TEST_CASE("LoadFighters clears existing fighters")
{
	FighterManager m;

	m.AddNewFighter();
	REQUIRE(m.m_fighters.size() == 1);
	auto existingFighter = *m.m_fighters.begin();

	QString errorMsg;
	m.LoadFighters("file does not exist.exe", errorMsg);
	REQUIRE(m.m_fighters.size() == 1); // should be the dummy fighter
	auto dummyFighter = *m.m_fighters.begin();

	REQUIRE(dummyFighter != existingFighter);
}

TEST_CASE("Contains finds identical fighters")
{
	FighterManager m;
	Fighter f("hans", "dampf");
	f.weight = "1234";
	f.category = "AllStars";
	f.club = "FC Bayern";
	REQUIRE_FALSE(m.Contains(f));

	m.AddFighter(f);
	REQUIRE(m.Contains(f));
}

TEST_CASE("Contains finds fighter if database has empty weight")
{
	FighterManager m;
	Fighter f("hans", "dampf");
	f.weight = "";
	f.category = "AllStars";
	f.club = "FC Bayern";
	m.AddFighter(f);
	REQUIRE(m.Contains(f));

	f.weight = "123";
	REQUIRE(m.Contains(f));
}

TEST_CASE("Contains finds fighter if fighter has empty weight")
{
	FighterManager m;
	Fighter f("hans", "dampf");
	f.weight = "1234";
	f.category = "AllStars";
	f.club = "FC Bayern";
	m.AddFighter(f);
	REQUIRE(m.Contains(f));

	f.weight = "";
	REQUIRE(m.Contains(f));
}

TEST_CASE("Contains finds fighter if database has empty category")
{
	FighterManager m;
	Fighter f("hans", "dampf");
	f.weight = "1234";
	f.category = "";
	f.club = "FC Bayern";
	m.AddFighter(f);
	REQUIRE(m.Contains(f));

	f.category = "AllStars";
	REQUIRE(m.Contains(f));
}

TEST_CASE("Contains finds fighter if fighter has empty category")
{
	FighterManager m;
	Fighter f("hans", "dampf");
	f.weight = "1234";
	f.category = "AllStars";
	f.club = "FC Bayern";
	m.AddFighter(f);
	REQUIRE(m.Contains(f));

	f.category = "";
	REQUIRE(m.Contains(f));
}
