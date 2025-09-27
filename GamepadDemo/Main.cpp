// Copyright 2018 Florian Muecke. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE.txt file.

#include "GamepadDemo.h"

#include <QApplication>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);

    GamepadDemo w;
    w.show();
    return a.exec();
}
