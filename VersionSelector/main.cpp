// Copyright 2010-2013 Florian Muecke. All rights reserved.
// http://www.ipponboard.info (ipponboardinfo at googlemail dot com)
//
// THIS FILE IS PART OF THE IPPONBOARD PROJECT.
// IT MAY NOT BE DISTRIBUTED TO OR SHARED WITH THE PUBLIC IN ANY FORM!
//
// $Id$

#include <QtGui/QApplication>
#include "maindlg.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainDlg w;
    w.show();
    
    return a.exec();
}
