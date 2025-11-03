# How to build Ipponboard

To build Ipponboard, there are currently two recommended development platforms:

Build system | Compiler | Target system
-- | -- | --
Windows 10/11 | [Microsoft Visual Studio C++](https://aka.ms/buildtools) (last used: VS 2017/2019/2022 a.k.a VC14) | Windows 7 and higher, 32 Bit
Linux (Ubuntu/WSL) | gcc 11 | Linux 64-Bit (experimental)

> Note: Support for Linux builds is still experimental and may lack some features (printing not 100% functional).

## Prerequisites

Ipponboard requires the following libraries and tools to be built: 

- [Qt framework](https://www.qt.io/) (required: 5.15.13 or newer; 6.x not yet supported)
  - versions < 5.15.13 can be used to compile, however not all tests are working properly because of missing fixes in the ssl library
- [CMake](https://cmake.org) (last used: 3.29.x)
- [Ninja](https://ninja-build.org/) build tool (last used: 1.11)
- [LLVM lld linker](https://lld.llvm.org/) (last used: 17.x) for faster linking on Linux
- [Git](https://git-scm.com/) (required for downloading third-party sources such as Catch2 during the CMake configure step)
- [Boost C++ Libraries](http://www.boost.org/) (last used: 1.81)
- [Pandoc](https://pandoc.org/) to build the HTML manual
- [Inno Setup](https://jrsoftware.org/isinfo.php) to create the setup on Windows (last used: 6.0)
- [LLVM clang-format](https://clang.llvm.org/docs/ClangFormat.html) (required: 14.x) for enforcing layout before builds/PRs

For compilation and configuration of the Qt framework, see the section [Building Qt](#building-qt).

## Run `build.ps1` resp. `build.sh`

_Note:_ Before executing the build script be sure to install all required dependencies like explained in [Building on Windows](#building-on-windows-1011) and [Building on Linux](#building-on-linuxubuntuwsl-).
Be sure to enable powershell script execution on windows via `Set-ExecutionPolicy -ExecutionPolicy Unrestricted` in an admin console.


The first run of `build.ps1` (`build.sh` on Linux) will create a file to configure the paths to the above libraries

```
> build.ps1
Please configure paths in "env_cfg.bat" first!
Press any key to continue . . .
```

Modify those according to your environment. After that, you may try building ;)... 

```
> build.ps1
Current config (debug):

     QTDIR     : c:\devtools\qt5\Qt5.15.13-x86-msvc2022
     BOOST_DIR : c:\devtools\boost_1_81_0
     ROOT_DIR  : c:\dev\_cpp\Ipponboard
     BUILD_DIR : c:\dev\_cpp\Ipponboard\_build\build-Ipponboard
     BIN_DIR   : c:\dev\_cpp\Ipponboard\_bin\Ipponboard-debug
     INNO_DIR  : c:\Program Files (x86)\Inno Setup 6

Select build mode:

     (1) clean ALL
     (2) create makefiles
     (3) tests only
     (4) build all
     (5) run Ipponboard
     (6) build doc
     (7) translate resources
     (8) build setup
     (9) clean build with setup (release)
     (s) switch debug/release
     (q) quit
```

> The first configure step will download Catch2 via CMake FetchContent. Ensure outbound network
> access (or provide a local mirror through the `FETCHCONTENT_SOURCE_DIR_Catch2` cache value) before running `build.sh` / `build.ps1`.


## Building on Windows 10/11

1. Install _Visual Studio_
2. Install `cmake` and make sure it's available via `%PATH%`
3. Install `ninja` and make sure it's available via `%PATH%`
4. [Build Qt5 framework](#building-qt5-on-windows-1011)
5. Install _Boost_
6. Install _Pandoc_ and make sure it's available via `%PATH%`
7. [Run `build.ps1`](#run-buildps1-resp-buildsh) from Windows PowerShell

### Building Qt5 on Windows 10/11

1. Install Python2 <br>
    `winget install python.python.2`

2. Get `qt-erverywhere-opensource-src-5.15.*.zip` from https://download.qt.io/official_releases/qt/5.15/

3. Extract archive

4. Configure

    1. Open x86 Native Tools Command Prompt for Visual Studio

    2. Add to environment path before running `configure.bat`: `set PATH=c:\Python27;%PATH%`

    3. `.\configure -prefix Qt5.15.13 -opensource -confirm-license -platform win32-msvc -debug-and-release -shared -silent -nomake examples -nomake tests -no-dbus -schannel -mp`

5. Compile & link <br>
    `jom`

6. Install in prefix path directory <br>
    `jom install`

----

## Building on Linux/Ubuntu/WSL 🐧


1. Install the `ninja-build` package from your distribution
2. Install the `lld` package (often `sudo apt install lld`)
3. [Build Qt5 framework](#building-qt5-on-ubuntu-wsl) or [use aqt to install Qt5 libraries](#installing-qt5-on-ubuntu-wsl-using-aqt)

4. Install _Boost_:
    1. Download the recent version from https://www.boost.org
    2. Extract the archive (no build required as only headers are used)
    3. Specify the path in `env.cfg` (see [build.sh](#run-buildps1-resp-buildsh))

4. Install _Pandoc_ 

5. [Run `./build.sh` from bash](#run-buildps1-resp-buildsh)


### Building Qt5 on Ubuntu (WSL)

1. Install development tools (if needed) <br>

    ```
    sudo apt-get update
    sudo apt-get install build-essential libgl1-mesa-dev libgstreamer-gl1.0-0 libpulse-dev libxcb-glx0 libxcb-icccm4 libxcb-image0 libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 libxcb-render0 libxcb-shape0 libxcb-shm0 libxcb-sync1 libxcb-util1 libxcb-xfixes0 libxcb-xinerama0 libxcb1 libxkbcommon-dev libxkbcommon-x11-0 libxcb-xkb-dev -y
    ```

1. Get `qt-erverywhere-opensource-src-5.15.*.tar.xz` from https://download.qt.io/official_releases/qt/5.15/

    _Note:_ Be sure to get at least Qt5.15.10 because of incompatibility with OpenSSL runtime library references (see https://bugreports.qt.io/browse/QTBUG-115146, https://gist.github.com/seyedmmousavi/b1f6681eb37f3edbe3dcabb6e89c5d43)


1. Extract the archive: 

    ```
    tar xf /mnt/c/Users/fmuec/Downloads/qt-everywhere-opensource-src-5.15.*.tar.xz
    ```

1. Install dependencies (required for audio, printing, and PDF export):

    ```
    sudo apt get install gperf pkg-config bison flex libdbus-1-dev libasound2-dev libcups2-dev libpulse-dev bison flex
    ```

    TODO: ??? sudo apt install libpulse-mainloop-glib0
    libnss3-dev nodejs python2 

1. Configure Qt build

    ```
    ./configure -prefix /usr/local/Qt5.15.13 -opensource -confirm-license -shared -silent -nomake examples -nomake tests -skip qtdoc -skip qtwebengine -openssl-runtime -cups
    ```

1. Compile & link

    ```
    gmake -jX
    ```

    with X being the number of your processor cores

1. Install

    ```
    sudo gmake install
    ```

### Installing Qt5 on Ubuntu (WSL) using `aqt`

1. Get aqt ([another qt installer](https://github.com/miurahr/aqtinstall))

    ```
    pip install -U pip
    pip install aqtinstall
    ```

2. Get the latest Qt version

    Check https://ddalcino.github.io/aqt-list-server/ and get the install command:

    ```
    aqt install-qt linux desktop 5.15.2 gcc_64
    aqt install-tool linux desktop tools_cmake
    ```

## Using *QtCreator* to develop Ipponboard

1. Download and install [QCreator](https://github.com/qt-creator/qt-creator/releases/)
2. Configure the Qt environment
3. Open `base/CmakeLists.txt` with QtCreator
4. Make sure that `QTDIR` points to the respective Qt installation
5. Add `BOOST_DIR` to the environment variables of the project
6. Define the boolean key `USE_QT5=ON` if you want to use Qt5 (use Qt4 otherwise)

### Configuring CDB Debugger

1. [Download Windows 10 SDK](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/debugger-download-tools) and be sure to install the *Debugging Tools*
2. Insert the path in the QtCreator debugger options (tab *CDB paths*): e.g., `C:\Program Files (x86)\Windows Kits\10\Debuggers\x86`
3. Restart QtCreator
