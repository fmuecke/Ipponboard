#include "../core/TournamentMode.h"
#include "../core/TournamentMode.cpp"

#include <QString>
#include <iostream>

using namespace Ipponboard;

struct IpponboardTest
{

static bool parse_group(QSettings& config, QString const& group, QString& errorMsg)
{
    TournamentMode tm;

    config.beginGroup(group);
    bool readSuccess = TournamentMode::parse_current_group(config, tm, errorMsg);

    if (!readSuccess)
    {
#ifdef _DEBUG
        std::cout << "--> " << errorMsg.toStdString() << std::endl;
#endif
    }

    config.endGroup();
    return readSuccess;
}

};

TEST_CASE("Test_parse_current_group")
{
	QSettings config("TournamentModes-test.ini", QSettings::IniFormat, nullptr);
	QStringList groups;
	groups
			<< "basic"
			<< "with_weights_doubled"
			<< "with_time_overrides"
			<< "template_not_found"
			<< "no_title"
			<< "no_weights"
			<< "no_rounds"
			<< "no_template"
			<< "no_fight_time";

    REQUIRE(groups.count() == config.childGroups().count());

	QString errorMsg;
    REQUIRE(IpponboardTest::parse_group(config, "basic", errorMsg));
    REQUIRE(IpponboardTest::parse_group(config, "with_weights_doubled", errorMsg));
    REQUIRE(IpponboardTest::parse_group(config, "with_time_overrides", errorMsg));
    REQUIRE(!IpponboardTest::parse_group(config, "template_not_found", errorMsg));
    REQUIRE(!IpponboardTest::parse_group(config, "no_title", errorMsg));
    REQUIRE(!IpponboardTest::parse_group(config, "no_weights", errorMsg));
    REQUIRE(!IpponboardTest::parse_group(config, "no_rounds", errorMsg));
    REQUIRE(!IpponboardTest::parse_group(config, "no_template", errorMsg));
    REQUIRE(!IpponboardTest::parse_group(config, "no_fight_time", errorMsg));

	//QCOMPARE(str.toUpper(), QString("HELLO"));
}

