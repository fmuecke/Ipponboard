# How to build Ipponboard

To build Ipponboard, there are currently two recommended development platforms:

Build system | Compiler | Target system
-- | -- | --
Windows 10/11 | [Microsoft Visual Studio C++](https://aka.ms/buildtools) (last used: VS 2022 a.k.a. VC143, 64-bit) | Windows 10 and higher, 64-bit
Linux (Ubuntu/WSL2) | gcc 13 (or newer) | Linux 64-bit (WSL2 or desktop)

> Note: Support for Linux builds is still experimental and may lack some features (printing not 100% functional).

## Prerequisites

Ipponboard now targets a pure Qt 6 toolchain and requires the following libraries and tools to be built:

- [Qt framework](https://www.qt.io/) (required: 6.9.2; newer Qt 6 minor releases are usually fine)
- [CMake](https://cmake.org) (last used: 3.29.x)
- [Ninja](https://ninja-build.org/) build tool (last used: 1.11)
- [LLVM lld linker](https://lld.llvm.org/) (last used: 17.x) for faster linking on Linux
- [Git](https://git-scm.com/) (required for downloading third-party sources such as Catch2 during the CMake configure step)
- [Boost C++ Libraries](http://www.boost.org/) (last used: 1.89)
- [Pandoc](https://pandoc.org/) to build the HTML manual
- [Inno Setup](https://jrsoftware.org/isinfo.php) to create the setup on Windows (last used: 6.0)
- [LLVM clang-format](https://clang.llvm.org/docs/ClangFormat.html) (required: 14.x) for enforcing layout before builds/PRs

For compilation and configuration of the Qt framework, see the section [Building Qt](#building-qt).

### Qt SDK layout and helper tools

The build scripts assume a standard Qt directory layout:

```
<QTDIR>/
  bin/          # executables such as lrelease, windeployqt, qtpaths, lupdate
  lib/          # Qt libraries
  plugins/      # platform, multimedia, printsupport plugins
  translations/ # shipped Qt translation catalogues
```

- Keep `<QTDIR>/bin` on your `PATH` (or call the tools via their absolute path) so `scripts/create-versioninfo.*`, translation targets, and packaging steps can find `qtpaths`, `lrelease`, and `windeployqt`.
- On Linux/WSL, `qtpaths --plugin-dir` is used to locate runtime plugins; make sure it points to the same Qt installation that CMake consumes.
- On Windows, `windeployqt` is invoked from the build scripts and packaging workflow; confirm it matches the `msvc2022_64` kit you installed.

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

     QTDIR     : c:\devtools\qt\6.9.2\msvc2022_64
     BOOST_DIR : c:\devtools\boost_1_89_0
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

1. Install _Visual Studio 2022_ (Desktop development with C++) with the x64 toolset
2. Install `cmake` and make sure it's available via `%PATH%`
3. Install `ninja` and make sure it's available via `%PATH%`
4. Install Qt 6.9.x via the Qt Online Installer or `aqtinstall` (recommended kits: `msvc2022_64`)
5. Install _Boost_ 1.89 (headers-only usage; binaries not required)
6. Install _Pandoc_ and make sure it's available via `%PATH%`
7. [Run `build.ps1`](#run-buildps1-resp-buildsh) from the **x64 Native Tools Command Prompt for VS 2022** (or the equivalent Developer PowerShell).

### Installing Qt 6 on Windows 10/11

1. (Optional) For scripted installs, ensure Python 3 is available and install [`aqtinstall`](https://github.com/miurahr/aqtinstall) via `pip install -U pip aqtinstall`.
2. Download the Qt Online Installer from https://www.qt.io/download or use `aqtinstall`.
3. Install the `Qt/6.9.x/msvc2022_64` component (debug+release).
4. Ensure `QTDIR` in `env_cfg.bat` points to the chosen installation.

----

## Building on Linux/Ubuntu/WSL 🐧


1. Install the `ninja-build` package from your distribution
2. Install the `lld` package (often `sudo apt install lld`)
3. Install Qt 6.9.x via the Qt Online Installer (under WSL2) or [use aqtinstall](#installing-qt-6-on-ubuntu-wsl-using-aqt)

4. Install _Boost_ 1.89:
    1. Download the recent version from https://www.boost.org
    2. Build & install into a user-writable prefix (e.g. `~/devtools/boost_1_89_0`) or extract headers only
    3. Specify the path in `env.cfg` (see [build.sh](#run-buildps1-resp-buildsh))

4. Install _Pandoc_ 

5. [Run `./build.sh` from bash](#run-buildps1-resp-buildsh)


### Installing Qt 6 on Ubuntu/WSL2

1. Install dependencies (required for audio, printing, and PDF export):

    ```
    sudo apt-get update
    sudo apt-get install build-essential libgl1-mesa-dev libgstreamer-gl1.0-0 libpulse-dev libxcb-glx0 libxcb-icccm4 \
        libxcb-image0 libxcb-keysyms1 libxcb-randr0 libxcb-render-util0 libxcb-render0 libxcb-shape0 libxcb-shm0 \
        libxcb-sync1 libxcb-util1 libxcb-xfixes0 libxcb-xinerama0 libxcb1 libxkbcommon-dev libxkbcommon-x11-0 \
        libxcb-xkb-dev
    ```

2. Install Qt via binary packages using `aqtinstall` (recommended).

### Installing Qt 6 on Ubuntu (WSL) using `aqt`

1. Get aqt ([another qt installer](https://github.com/miurahr/aqtinstall))

    ```
    pip install -U pip
    pip install aqtinstall
    ```

2. Get the latest Qt version

    Check https://ddalcino.github.io/aqt-list-server/ and get the install command:

    ```
    aqt install-qt linux desktop 6.9.2 gcc_64
    aqt install-tool linux desktop tools_cmake
    ```

## Using *QtCreator* to develop Ipponboard

1. Download and install [QCreator](https://github.com/qt-creator/qt-creator/releases/)
2. Configure the Qt environment
3. Open the top-level `CMakeLists.txt` with QtCreator
4. Ensure that `QTDIR` is exported in the environment for your kit
5. Add `BOOST_DIR` to the environment variables of the project
6. Confirm the kit uses a compiler with C++20 support (MSVC 2022 or gcc 13+)

### Configuring CDB Debugger

1. [Download Windows 10 SDK](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/debugger-download-tools) and be sure to install the *Debugging Tools*
2. Insert the path in the QtCreator debugger options (tab *CDB paths*): e.g., `C:\Program Files (x86)\Windows Kits\10\Debuggers\x86`
3. Restart QtCreator
