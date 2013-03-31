#include "../core/TournamentMode.h"

#include <QtTest/QtTest>
#include <iostream>

using namespace Ipponboard;

class TestTournamentMode : public QObject
{
    Q_OBJECT
	
private:
	bool parse_group(QSettings& config, QString const& group, QString& errorMsg);

private slots:
    void Test_parse_current_group();
};

bool TestTournamentMode::parse_group(QSettings& config, QString const& group, QString& errorMsg)
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

void TestTournamentMode::Test_parse_current_group()
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

    QVERIFY(groups.count() == config.childGroups().count());

	QString errorMsg;
	QVERIFY(parse_group(config, "basic", errorMsg));
	QVERIFY(parse_group(config, "with_weights_doubled", errorMsg));
	QVERIFY(parse_group(config, "with_time_overrides", errorMsg));
    QVERIFY(!parse_group(config, "template_not_found", errorMsg));
    QVERIFY(!parse_group(config, "no_title", errorMsg));
    QVERIFY(!parse_group(config, "no_weights", errorMsg));
    QVERIFY(!parse_group(config, "no_rounds", errorMsg));
    QVERIFY(!parse_group(config, "no_template", errorMsg));
    QVERIFY(!parse_group(config, "no_fight_time", errorMsg));

    //QCOMPARE(str.toUpper(), QString("HELLO"));
}

QTEST_MAIN(TestTournamentMode)
#include "TestTournamentMode.moc"
