# Prerequisites

Library | Tested Version
-- | --
Visual Studio C++ | 2015 (VC14)
Qt library | 4.8.x (5.x not yet supported)
[Boost C++ Libraries](http://www.boost.org/) | 1.59
[Inno Setup](https://jrsoftware.org/isinfo.php) | 5.0

The first run of `build.cmd` will create a file to configure the paths to the above libraries
    > build.cmd
	Please configure paths in "env_cfg.bat" first!
    Press any key to continue . . .

Modify those according to your environment.

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
