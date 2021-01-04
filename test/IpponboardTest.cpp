// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#define CATCH_CONFIG_MAIN
#include "../util/catch2/catch.hpp"

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
