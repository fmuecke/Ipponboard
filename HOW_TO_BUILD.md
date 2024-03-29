# Prerequisites

Ipponboard requires the following libraries and tools to get built: 
- [Microsoft Visual Studio C++](https://aka.ms/buildtools) (last used: VS 2015/2017 a.k.a VC14)
- [Qt library](https://www.qt.io/) (last used: 4.8.7; 5.x not yet supported)
- [Boost C++ Libraries](http://www.boost.org/) (last used: 1.59)
- [Inno Setup](https://jrsoftware.org/isinfo.php) (last used: 5.0)
- [jom](https://download.qt.io/official_releases/jom/) (also included in QCreator)
- optional: pandoc to build the manual

The first run of `build.cmd` will create a file to configure the paths to the above libraries
    > build.cmd
	Please configure paths in "env_cfg.bat" first!
    Press any key to continue . . .

Modify those according to your environment. After that you may try building ;)...

    > build.cmd
	Current config:

      QMAKESPEC : win32-msvc2015
      QTDIR     : c:\devtools\qt\qt-4.8.7-vc14\bin
      BOOST_DIR : c:\devtools\boost_1_59_0
    
    Select build mode:
    
      (1) clean
      (2) build
      (3) rebuild
      (4) setup
      (5) run
      (6) build doc
      (9) all
      (q) quit

## Configure Visual Studio Build Tools

You must ensure that you use the same compiler version that was used to build the Qt libraries to build Ipponboard to avoid runtime problems.

### MSVC v140 using Visual Studio 2019

1. Make sure the build tools are installed.
2. Start the VC140 Developer Command Prompt
   e.g. by starting a custom batch file `vc14.cmd` that calls `vcvarsall.bat` or specify the `-vcvars_ver=14.0` parameter manually:
   ```batch
   @echo off
   title Visual Studio Command Shell
   %comspec% /k "c:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" x86 -vcvars_ver=14.0
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
