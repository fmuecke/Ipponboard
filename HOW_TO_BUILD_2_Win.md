# Prerequisites for Windows build

Ipponboard requires the following libraries and tools to get built: 
- [Microsoft Visual Studio C++] (last used: VS 2022 a.k.a VC17.8.3). Do not use BuildTools, because it is not much smaller and the IDE is missing! 
- [Qt library](https://www.qt.io/) (last used: 4.8.7; 5.x not yet supported) --> TODO: migrate to QT5
- [Boost C++ Libraries](http://www.boost.org/) (last used: 1.82.0) --> TODO: remove Boost dependencies
- [Inno Setup](https://jrsoftware.org/isinfo.php) (last used: 6.2.2)
- optional: pandoc to build the manual (last used: 3.1.5)
- optional: use jom instead of nmake to compile parallel with multiple cores (https://download.qt.io/official_releases/jom/) (last used 1.1.4) 

&nbsp;
## Configure Visual Studio Build Tools

You must ensure that you use the same compiler version that was used to build the Qt libraries to build Ipponboard to avoid runtime problems.

&nbsp;
## Checkout and setup sources
1. `git clone https://gitlab.com/r_bernhard/Ipponboard.git`
2. Create versioninfo.h: 
    - cd Ipponboard\base   
    - edit in *_create_versioninfo.cmd* the variable values of *VER1*, *VER2*, *VER3* 
        (e.g. major release/minor release/feature release).
        *TAG* contains the git tag we´re planning for this version. 
        *VER4* contains the build number of the product (do not edit this variable)      
    - **execute *_create_versioninfo.cmd* all the time before building a release!**

# Compile
## Using __deprecated__build_qmake_qt4-x86.cmd (will not supported anymore)
1. Execute `__deprecated__build_qmake_qt4-x86.cmd`
    - The first run of *__deprecated__build_qmake_qt4-x86.cmd* will create a file to configure the paths to the above libraries
    - Please configure paths in `_env_cfg-x86.bat` first!

2. Modify those according to your environment. After that you may try building by executing `__deprecated_build_qmake_qt4-x86.cmd` again    
```
Select build mode:  
    (1) initial: create makefiles   
    (2) build --> needs jom     
    (3) rebuild     
    (4) clean   
    (5) run     
    (6) build doc --> needs pandoc  
    (7) setup --> needs Inno Setup  
    (8) all     
    (q) quit        
```
&nbsp;
## Using vcproj (will not supported anymore)
Create a Visual Studio project and solution file. You can use this for compiling or develop the source code in Visual Studio

    1. make_vcproj.cmd  
    2. nmake
    
Note:  This generates only a solution compatible with VStudio 2015. Use CMake to create a more up-to-date solution)

## Using Visual Studio 2022 Command Prompt

Start the Developer Command Prompt, e.g. by starting a custom batch file `vcvars64.bat` for 64bit or `vcvars` for 32bit 
   ```batch
   @echo off
   title Visual Studio Command Shell
   set MAKE_BUILD_TYPE=Release
   %comspec% /k "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
   ```
   
&nbsp;
## Using QtCreator
1. Download and install QtCreator   
2. Configure the Qt environment     
3. Open *Ipponboard.pro* or the *CMakefileList.txt* with QtCreator  
4. Add *BOOST_DIR* to the environment variables of the project  

&nbsp;
## Configuring CDB Debugger
1. [Download Windows 10 SDK](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/debugger-download-tools) and be sure to install the *Debugging Tools*
2. Insert path in the QtCreator debugger options (tab *CDB paths*): e.g. `C:\Program Files (x86)\Windows Kits\10\Debuggers\x86`
3. Restart QtCreator

&nbsp;
## Using cmake: Recommended build solution !!!
Via CMakelist we can do
1. Compile sources
    - Inclusion of CMakelists.txt in the Ipponboard source directory --> cmake_qt4 for QT4-compile and cmake_qt5 for QT5 (TODO), that contain the CMakeLists.txt
    - QT4-32: cmake -S "..\\cmake_qt4" -B "..\\_build_cmake_qt4" -G "Visual Studio 17 2022" -A Win32 -DCMAKE_BUILD_TYPE=Release|Debug
    - QT4-64: cmake -S "..\\cmake_qt4" -B "..\\_build_cmake_qt4" -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release|Debug
    - QT5-64: cmake -S "..\\cmake_qt5" -B "..\\_build_cmake_qt4" -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release|Debug
    - Build Targets:
        - Release Build: cmake --build _build_cmake --config Release --target Ipponboard 
        - Debug Build: cmake --build ..\\_build_cmake --config Debug --target Ipponboard_Debug
        - (IpponboardTest: cmake --build ..\\_build_cmake --target IpponboardTest)
        - (GamepadDemo: cmake --build ..\\_build_cmake --target GamepadDemo)
        - Make Setup: cmake --build ..\\_build_cmake --target make-setup
2. create resources, e.g. language files, icons
3. create documentation
4. create installer

Execute the file `build_cmake_qt4-x86.cmd` or `build_cmake_qt4-x64.cmd` and you will find a Setup-File of Ipponboard in the bin-folder.
