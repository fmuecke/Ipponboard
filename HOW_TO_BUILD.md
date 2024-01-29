# Prerequisites

Ipponboard requires the following libraries and tools to get built: 
- [Microsoft Visual Studio C++] (last used: VS 2022 a.k.a VC17.8.3). Do not use BuildTools, because it is not much smaller and the IDE is missing! 
- [Qt library](https://www.qt.io/) (last used: 4.8.7; 5.x not yet supported) --> TODO: migrate to QT5
- [Boost C++ Libraries](http://www.boost.org/) (last used: 1.82.0) --> TODO: remove Boost dependencies
- [Inno Setup](https://jrsoftware.org/isinfo.php) (last used: 6.2.2)
- optional: pandoc to build the manual (last used: 3.1.5)
- optional: use jom instead of nmake to compile parallel with multiple cores (https://download.qt.io/official_releases/jom/) (last used 1.1.4)

## Build QT4 for your developement platform
As there are no longer any official download sources for QT4 libraries that can be used on current operating systems (e.g. Windows 10), 
these must be generated from the sources for the target system. 

Attention, problems compiling the original code! The MSVC compiler from version 14 compiles c++11 standard, but there is incompatible 
code in QT4. You either need mingw/gcc to reduce the standard or you can use source code that has already been adapted for 
this (e.g. https://github.com/scharsig/Qt or https://github.com/raikantasahu/Qt)
1. unpack the sources to: c:\dev\src\qt\qt-src-4.8.7
2. delete the directory c:\dev\src\qt\qt-src-4.8.7\src\3rdparty\javascriptcore\JavaScriptCore\os-win32
3. start vcvars64.bat for 64bit target compilation or vcvars32.bat for 32bit
4. run configure. Create debug and release dlls with the msvc platform corresponding to the installed VStudio
	for 32bit: configure -debug-and-release -prefix c:\dev\inst\qt\qt-4.8.7-x86-msvc2017 -opensource -confirm-license -nomake examples -nomake tests -shared -ltcg -no-qt3support -platform win32-msvc2017
	for 64bit: configure -debug-and-release -prefix c:\dev\inst\qt\qt-4.8.7-x64-msvc2017 -opensource -confirm-license -nomake examples -nomake tests -shared -ltcg -no-qt3support -platform win32-msvc2017
5. run nmake for QT:
	a: nmake
	b: nmake install	

Note: The default directory in QT is defined by "configure -prefix <path>" and created by "nmake install". 
This also implicitly defines the QT INCLUDE- and LIB-Directory. To ensure that no such path dependency has to be included in the Ipponboard, it is recommended that the QT installation is executed as described here. 
To reconfigure the path, run "nmake confclean", change the setting and rerun nmake.

##  checkout and setup sources
1. git clone https://gitlab.com/r_bernhard/Ipponboard.git
2. Create versioninfo.h:
	a: cd Ipponboard\base
	b: edit in "create_versioninfo.cmd" the variable values of VER1, VER2, VER3 (e.g. major release/minor release/feature release).
		TAG contains the git tag we´re planning for this version. 
		VER4 contains the build number of the product (do not edit this variable)
	c: execute "create_versioninfo.cmd" all the time before building a release!

## compile
### Using build_qmake_qt4-x86.cmd
1. The first run of `build_qmake_qt4-x86.cmd` will create a file to configure the paths to the above libraries
	- build_qmake_qt4-x86.cmd
	- Please configure paths in "env_cfg-x86.bat" first!
	-  Attention: Whenever you change a path in "build_qmake_qt4-x86.cmd", you must delete the file "env_cfg-x86.bat" and rerun "build_qmake_qt4-x86.cmd"!

2. Modify those according to your environment. After that you may try building ;)...
- execute "build_qmake_qt4-x86.cmd" again
-->	Select build mode:  
      (1) clean
	  (2) initial: create makefiles
      (3) build --> needs jom
      (4) rebuild
      (5) setup --> needs Inno Setup
      (6) run
      (7) build doc --> needs pandoc
      (8) all
      (q) quit

	try "clean"
	try "build"
	try "run"
	try "rebuild"
	try "build doc"
	try "setup"
	execute "all"
	quit

Note: In copy_files.cmd we only deploy 64bit redistributables --> TODO: make it variable

### Using vcproj
Create a VStudio project and solution file. You can use this for compiling or develop the source code in Visual Studio 
	1. make_vcproj.cmd
	2. nmake

Note:  This generates only a solution compatible with VStudio 2015. Use CMake to create a more up-to-date solution)

### Using cmake: Recommended build solution
Via CMakelist we can do
	1. compile sources
		- Inclusion of CMakelists.txt in the Ipponboard source directory --> cmake_qt4 for QT4-compile and cnake_qt5 for QT5, that contain the CMakeLists.txt
		- QT4-32: cmake -S "cmake_qt4" -B "./_build_cmake" -G "Visual Studio 17 2022" -A Win32 
		- QT4-64: cmake -S "cmake_qt4" -B "./_build_cmake" -G "Visual Studio 17 2022" -A x64
		- QT5-64: cmake -S "cmake_qt5" -B "./_build_cmake" -G "Visual Studio 17 2022" -A x64  
		- Build Targets:
			- Release Build: cmake --build _build_cmake --config Release --target Ipponboard
			- Debug Build: cmake --build _build_cmake --config Debug --target Ipponboard_Debug
			- (IpponboardTest: cmake --build _build_cmake --target IpponboardTest)
			- (GamepadDemo: cmake --build _build_cmake --target GamepadDemo)
			- Make Setup: cmake --build _build_cmake --target make-setup	2. create resources, e.g. language files, icons (TODO)
	2. create documentation
	3. create installer
	Execute the file "build_cmake_qt4-x64.cmd" and you will find a Setup-File of Ipponboard in the bin-folder.

## Configure Visual Studio Build Tools

You must ensure that you use the same compiler version that was used to build the Qt libraries to build Ipponboard to avoid runtime problems.

### Using Visual Studio 2022

Start the Developer Command Prompt, e.g. by starting a custom batch file `vcvars64.bat` for 64bit or `vcvars` for 32bit 
   ```batch
   @echo off
   title Visual Studio Command Shell
   set MAKE_BUILD_TYPE=Release
   %comspec% /k "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
   ```
   
## Using *QtCreator* to build Ipponboard

1. Download and install [QCreator](https://www.qt.io/product/development-tools)
2. Configure the Qt environment
3. Open `Ipponboard.pro` with QtCreator
4. add `BOOST_DIR` to the environment variables of the project

### Configuring CDB Debugger

1. [Download Windows 10 SDK](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/debugger-download-tools) and be sure to install the *Debugging Tools*
2. Insert path in the QtCreator debugger options (tab *CDB paths*): e.g. `C:\Program Files (x86)\Windows Kits\10\Debuggers\x86`
3. Restart QtCreator
