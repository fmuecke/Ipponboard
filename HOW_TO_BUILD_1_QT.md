# Build QT4 for Windows
As there are no longer any official download sources for QT4 libraries that can be used on current operating systems (e.g. Windows 10), 
these must be generated from the sources for the target system.  
Attention, there are problems compiling the original code! The MSVC compiler from version 14 compiles c++11 standard, but there is incompatible 
code in QT4. You either need mingw/gcc to reduce the standard or you can use source code that has already been adapted for 
this (e.g. https://github.com/scharsig/Qt or https://github.com/raikantasahu/Qt).
1. Unpack the sources to: *c:\dev\src\qt\qt-src-4.8.7*
2. Delete the directory *c:\dev\src\qt\qt-src-4.8.7\src\3rdparty\javascriptcore\JavaScriptCore\os-win32*
3. Start `vcvars64.bat` for 64bit target compilation or `vcvars32.bat` for 32bit  
4. Run configure, create debug and release dlls with the msvc platform corresponding to the installed Visual Studio 
            
    for 32bit: `configure -debug-and-release -prefix c:\dev\inst\qt\qt-4.8.7-x86-msvc2017 -opensource -confirm-license -nomake examples -nomake tests -shared -ltcg -no-qt3support -platform win32-msvc2017`
    
    for 64bit: `configure -debug-and-release -prefix c:\dev\inst\qt\qt-4.8.7-x64-msvc2017 -opensource -confirm-license -nomake examples -nomake tests -shared -ltcg -no-qt3support -platform win32-msvc2017`

5. run nmake for QT:

    a: `nmake`  
    b: `nmake install`

Note: The default directory in QT is defined by *configure -prefix \<path\>* and created by *nmake install*. 
This also implicitly defines the QT INCLUDE- and LIB-Directory. To ensure that no such path dependency 
has to be included in the Ipponboard, it is recommended that the QT installation is executed as described here. 
To reconfigure the path, run `nmake confclean`, change the setting and rerun nmake.

&nbsp;
# Build QT4 for Linux
## Debian based
`sudo apt-get install libfontconfig1-dev libfreetype6-dev libx11-dev libxcursor-dev libxext-dev libxfixes-dev libxft-dev libxi-dev libxrandr-dev libxrender-dev`

## Redhat based
`sudo yum -y install fontconfig-devel freetype-devel libX11-devel libXcursor-devel libXext-devel libXfixes-devel libXft-devel libXi-devel libXrandr-devel libXrender-devel`

# Build QT4 for your development platform
1. Download `qt-everywhere-opensource-src-4.8.7.tar.gz` from https://download.qt.io/archive/qt/4.8/4.8.7/ 
2. Unpack the sources to: ~/dev/src/qt/qt-src-4.8.7 and change into the created directory
3. Run configure, create debug and release dlls with gcc platform   
    
    for 32bit: `./configure -debug-and-release -prefix ~/dev/inst/qt/qt-4.8.7-x32-gcc -opensource -confirm-license -nomake examples -nomake tests -nomake demos -no-openssl -shared -no-qt3support -platform linux-g++-32`

    for 64bit: `./configure -debug-and-release -prefix ~/dev/inst/qt/qt-4.8.7-x64-gcc -opensource -confirm-license -nomake examples -nomake tests -nomake demos -no-openssl -shared -no-qt3support -platform linux-g++-64`
4. Patching the errors: 
    
    a: dialogs/qprintdialog_unix.cpp:281:19: error: ‘class Ui::QPrintPropertiesWidget’ has no member named ‘cupsPropertiesPage’
  281 |     delete widget.cupsPropertiesPage; --> edit the file src/corelib/global/qglobal.h and line 2500 replace the break by a continue  
    b: ./3rdparty/javascriptcore/JavaScriptCore/wtf/TypeTraits.h:173:69: error: ‘std::tr1’ has not been declared --> Add in mkspecs/linux-g++-64/qmake.conf: QMAKE_CXXFLAGS = $$QMAKE_CFLAGS -std=gnu++98   
    
    c: messagemodel.cpp:186:61: error: ordered comparison of pointer with integer zero (‘MessageItem*’ and ‘int’) --> fix messagemodel.cpp:186 to: if (c->findMessage(m->text(), m->comment()))
5. compile and install:         
    
    a: `gmake`    
    b: `gmake install`

Note: The default directory in QT is defined by *configure -prefix \<path\>* and created by *make install*. 
This also implicitly defines the QT INCLUDE- and LIB-Directory. To ensure that no such path dependency has to be included in the Ipponboard, it is recommended that the QT installation is executed as described here.
To reconfigure the path, run `gmake confclean`, change the setting and rerun gmake.
