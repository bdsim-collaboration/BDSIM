.. _dev-buildandtesting:

Build System & Testing
**********************

Build System
============

The build system is based on CMake. The build options and variables are described in the user's manual. Some additional options for developers.

 * make dist: This archives the git repository in a file bdsim-0.9.develop.tar.bz2
 * make copy-deps: This copies all dependent libraries to the build/devs/ directory
 * BUILD_MACOSX_APP: CMake variable that builds an MacOS application at installation time.

Test System
===========

The test system is based on CTest, which works in combination with CMake. Tests are added to the
:code:`CMakeLists.txt` file beside each example declaring the main gmad file as well as any
optional command line arguments.

Comparison of output is being added currently.

Package System
==============

The package system is based on CPack, which works in combination with CMake. One can package the libraries with :code:`cpack`. A packed .tar.gz and a Mac OS .dmg is created. The source can be packed with :code:`make dist`. It is recommended to pack the libraries after each release version.
