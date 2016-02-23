.. _dev-buildandtesting:

Build System & Testing
**********************

Build System
============

The build system is based on CMake.


Test System
===========

The test system is based on CTest, which works in combination with CMake. Tests are added to the
:code:`CMakeLists.txt` file beside each example declaring the maing gmad file as well as any
optional command line arguments.

Comparison of output is being added currently.

Package System
==============

The package system is based on CPack, which works in combination with CMake. One can package the libraries with :code:`cpack`. A packed .tar.gz and a Mac OS .dmg is created. The source can be packed with :code:`make dist`. It is recommended to pack the libraries after each release version.
