// Copyright 2010-2014 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "../../../../development/Catch/include/catch.hpp"
#include "../core/Score.cpp"
#include "../core/Fight.cpp"


#include "TestTournamentMode.hpp"
#include "TestJson.hpp"
#include "TestScore.hpp"
#include "TestFight.hpp"


/*
int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QTEST_DISABLE_KEYPAD_NAVIGATION
    std::vector<QObject*> testObjects;

    //
    // define tests
    //
//    TestJson tj;
//    testObjects.push_back(&tj);

    TestTournamentMode tm;
    testObjects.push_back(&tm);

    //
    // execute tests
    //
    for (auto testObj : testObjects)
    {
        auto result = QTest::qExec(testObj, argc, argv);
        if (result != 0)
        {
            return result;
        }
    }

    return 0;
}
//#include "TestJson.moc"
#include "TestTournamentMode.moc"

*/
