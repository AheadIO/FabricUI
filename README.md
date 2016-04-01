FabricUI Library Source
==========================

Copyright (c) 2010-2016, Fabric Software Inc. All rights reserved.

Introduction
---------------

Fabric Software makes available the source code for the FabricUI library, used in various parts of Fabric Engine.  The primary purpose of making the source code available is to allow users to rebuild the library in a different environment than the one the Fabric Engine binary distribution assumes.  For instance, FabricUI can be rebuilt against a custom version of Qt.

Licensing
------------

The FabricUI library source is made available under the terms of a new (3-clause) BSD open-source license; see the file `LICENSE.txt` for details.

Prerequsites
----------------

- You must have the C++ compiler toolchain installed for your platform

  - Windows: currently the scripts only build against Visual Studio 2013 (at Fabric we use Visual Studio 2013 Express for Windows Desktop).  However, it should be possible to use a different version of Visual Studio by changing the value of MSVC_VERSION in SConstruct

  - CentOS 6: current Linux looks for a build of gcc located under /opt/centos5/usr/bin/.  However, it should be possible to remove the lines in SConscript that reference these paths to make it instead use the default system compiler (or change them to a different compiler)

  - Mac OS X: the compiler that comes with Xcode since at least 2013, along with the command lines tools package, should work fine

- You must have Qt installed.  If you specify QT_DIR in the environment then it will be used to look for the Qt installation, otherwise the build scripts will attept to find it as follows:

  - Windows: will look for the default Qt 4.8 installation, available from http://www.qt.io/download/

  - CentOS 6: will look for the Qt 4.6 build available from the built-in CentOS repository and that gets installed through apt

  - Mac OS X: will look for Qt 4.8 from homebrew (http://brew.sh/)

- You must have Python installed
  
  - Windows: Download the latest Python 2.7.x from https://www.python.org/downloads/

  - CentOS 6: install Python 2.6 from the built-in CentOS repository

  - Mac OS X: already comes with Python installed

- You must have SCons installed.  On all platforms, use the either easy_install or or pip to install the SCons package for Python.

Performing the Build
-----------------------------

You need to have the Fabric environment set up.  Refer to http://docs.fabric-engine.com/FabricEngine/latest/HTML/GettingStartedGuide/baseinstallation.html for instructions on installing Fabric and setting up the environment. 

The build happens directly into $FABRIC_DIR, ie. the top level of your unpacked distribution.  If you don't want to replace the existing files in your Fabric distribution, make a copy of it and use the copy for $FABRIC_DIR.

To run the build, simple change directory to the top level of your FabricUI checkout and run "scons".  It should build all of the UI source files and produce the new library in place of the old one.
