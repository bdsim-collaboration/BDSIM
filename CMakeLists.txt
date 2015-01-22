cmake_minimum_required(VERSION 2.6.4)

project(BDSIM) 

# project version
set( BDSIM_MAJOR_VERSION 0 )
set( BDSIM_MINOR_VERSION 6 )
set( BDSIM_PATCH_LEVEL develop )
set( BDSIM_VERSION ${BDSIM_MAJOR_VERSION}.${BDSIM_MINOR_VERSION}.${BDSIM_PATCH_LEVEL})

message(STATUS "Configuring BDSIM ${BDSIM_VERSION}")

# add source dir as a place for CMake modules (e.g. FindGSL.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# With this true we automatically include current src/binary directory
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# Default build type (defines different sets of compiler flags)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
       "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel DebugProf DebugCoverage."
       FORCE)
endif()

message(STATUS "Build Type ${CMAKE_BUILD_TYPE}")

# Compiler flags based on build type 
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    add_definitions("-g -O0 -DBDSDEBUG")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "DebugCoverage")
    if(NOT CMAKE_COMPILER_IS_GNUCXX)
         message(WARNING "DebugCoverage only works with gcc compiler")
    endif()
    add_definitions("-g -O0 -DBDSDEBUG")
    add_definitions("--coverage")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "None")
    # if specifically None optimise
    set(CMAKE_CXX_FLAGS "-O3 -ffast-math -DNDEBUG")
endif()

# Compiler-specific C++11 activation.
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
        message(STATUS "Compiler supports C++11")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    message(STATUS "Compiler supports C++11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

set(CMAKE_C_ORIG_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_ORIG_FLAGS "${CMAKE_CXX_FLAGS}")
if($ENV{VERBOSE})
	message(STATUS "CMAKE_C_ORIG_FLAGS ${CMAKE_C_ORIG_FLAGS}")
	message(STATUS "CMAKE_CXX_ORIG_FLAGS ${CMAKE_CXX_ORIG_FLAGS}")
endif()

# Check for Scientific Linux
if(UNIX)
    if("${CMAKE_SYSTEM}" MATCHES "el5")
        set(RHL5 TRUE)
    elseif("${CMAKE_SYSTEM}" MATCHES "el6")
        set(RHL6 TRUE)
    endif()
    if(RHL5 OR RHL6)
        set(RHL TRUE)
    endif()
    if("$ENV{HOSTNAME}" MATCHES "lxplus" OR "$ENV{HOSTNAME}" MATCHES "lxfssl")
        set(LXPLUS TRUE)
    else()
        set(LXPLUS FALSE)
    endif()
    if($ENV{VERBOSE})
         message(STATUS "UNIX System Version: ${CMAKE_SYSTEM}")
         message(STATUS "ON HOST: $ENV{HOSTNAME}")
         message(STATUS "ON LXPLUS... - ${LXPLUS}")
    endif()
endif()

# We use afs version of Geant4/CLHEP if we are on lxplus by default:
if(EXISTS "/afs/cern.ch" AND LXPLUS)
    option(USE_AFS "Use Geant4/CLHEP libraries from AFS" ON)
    if($ENV{VERBOSE})
         message(STATUS "Use Libraries from LXPLUS")
    endif()
else()
    option(USE_AFS "Use Geant4/CLHEP libraries from AFS" OFF)
endif()

# CLHEP
if (USE_AFS AND NOT CLHEP_LIBRARY_DIR)
   if (APPLE)
      set(CLHEP_INCLUDE_DIR /afs/cern.ch/sw/lcg/external/clhep/2.1.0.1/x86_64-mac106-gcc42-opt/include)
      set(CLHEP_LIBRARY_DIR /afs/cern.ch/sw/lcg/external/clhep/2.1.0.1/x86_64-mac106-gcc42-opt/lib)
   elseif (RHL5)
      set(CLHEP_INCLUDE_DIR /afs/cern.ch/sw/lcg/external/clhep/2.1.3.1/x86_64-slc5-gcc43-opt/include)
      set(CLHEP_LIBRARY_DIR /afs/cern.ch/sw/lcg/external/clhep/2.1.3.1/x86_64-slc5-gcc43-opt/lib)
   elseif (RHL6)
      set(CLHEP_INCLUDE_DIR /afs/cern.ch/sw/lcg/external/clhep/2.1.4.1/x86_64-slc6-gcc47-opt/include)
      set(CLHEP_LIBRARY_DIR /afs/cern.ch/sw/lcg/external/clhep/2.1.4.1/x86_64-slc6-gcc47-opt/lib)
   endif()
endif()

set(CLHEP_VERSION 9)
find_package(CLHEP REQUIRED)
include_directories(${CLHEP_INCLUDE_DIR})

# ROOT
if ($ENV{ROOTSYS})
   message(STATUS "Use ROOTSYS from environment: $ENV{ROOTSYS}")
elseif(USE_AFS)
   # add ROOT afs library to PATH (only in cmake)
   if (APPLE)
   	set(ROOTSYS "/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.14/x86_64-mac108-gcc42-opt/root")
    elseif (RHL5)
        set(ROOTSYS "/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.07/x86_64-slc5-gcc43-opt/root")
    elseif (RHL6)
        set(ROOTSYS "/afs/cern.ch/sw/lcg/app/releases/ROOT/5.34.24/x86_64-slc6-gcc47-opt/root")
    endif()
    if($ENV{VERBOSE})
        message(STATUS "ROOTSYS: ${ROOTSYS}")
    endif()
endif()

if(USE_ROOT)
    find_package(ROOT REQUIRED)
else()
    # if not specifically assigned or false, still try to find ROOT
    find_package(ROOT)
endif()
if(ROOT_FOUND)
    option(USE_ROOT "Include ROOT support."  ON )
else()
    option(USE_ROOT "Include ROOT support."  OFF )
endif()
if(USE_ROOT)
    add_definitions("-DUSE_ROOT")
    include_directories(${ROOT_INCLUDE_DIR})
    message(STATUS "ROOT support ON")
endif()

# add xercesc include path (possibly not needed with Geant4 CMake config):
if(USE_AFS)
   if(APPLE)
      set(XercesC_INCLUDE_DIR /afs/cern.ch/sw/lcg/external/XercesC/3.1.0/x86_64-mac106-gcc42-opt/include)
      set(XercesC_LIBRARY_DIR /afs/cern.ch/sw/lcg/external/XercesC/3.1.0/x86_64-mac106-gcc42-opt/lib)
   elseif(RHL5)
      set(XercesC_INCLUDE_DIR /afs/cern.ch/sw/lcg/external/XercesC/3.1.1p2/x86_64-slc5-gcc43-opt/include)
      set(XercesC_LIBRARY_DIR /afs/cern.ch/sw/lcg/external/XercesC/3.1.1p2/x86_64-slc5-gcc43-opt/lib)
   elseif(RHL6)
      set(XercesC_INCLUDE_DIR /afs/cern.ch/sw/lcg/external/XercesC/3.1.1p2/x86_64-slc6-gcc47-opt/include)
      set(XercesC_LIBRARY_DIR /afs/cern.ch/sw/lcg/external/XercesC/3.1.1p2/x86_64-slc6-gcc47-opt/lib)
   endif()
endif()

find_package(XercesC)

# GDML (on by default)
if(XercesC_FOUND)
    option( USE_GDML "Include GDML support."  ON )
else()
    option( USE_GDML "Include GDML support."  OFF )
endif()
if(USE_GDML)
    add_definitions("-DUSE_GDML")
    include_directories(${XercesC_INCLUDE_DIR})
    message(STATUS "GDML support ON")
endif()

# LCDD (on by default if xml libraries are found)

#include XML directory
find_path(XML2_INCLUDE_DIR NAMES libxml2)
set(XML_INCLUDE_DIR ${XML2_INCLUDE_DIR}/libxml2)
if($ENV{VERBOSE})
    message(STATUS "XML_INCLUDE_DIR: ${XML_INCLUDE_DIR}")
endif()
find_library(XML_LIBRARIES NAMES xml2 PATH_SUFFIXES xml2)
if(XML_LIBRARIES)
    MESSAGE(STATUS "Looking for XML2... - found")
else()
    MESSAGE(STATUS "Looking for XML2... - Not found")
endif(XML_LIBRARIES)
if($ENV{VERBOSE})
    message(STATUS "XML_LIBRARIES: ${XML_LIBRARIES}")
endif()

if(XML_LIBRARIES)
    option( USE_LCDD "Include LCDD support."  ON )
else()
    option( USE_LCDD "Include LCDD support."  OFF )
endif()
if(USE_LCDD)
    add_definitions("-DUSE_LCDD")
    include_directories(${XML_INCLUDE_DIR})
    message(STATUS "LCDD support ON")
endif()

# Try first with Geant4 built-in Config (versions 9.5 and higher should have it):
if ($ENV{VERBOSE})
   set(Geant4_CONFIG_DEBUG TRUE)
endif()

# Search for Geant4 CONFIG mode, if prefix given search that path only.
if (NOT ${Geant4_PREFIX} STREQUAL "")
   if($ENV{VERBOSE})
      message(STATUS "Geant4 prefix: ${Geant4_PREFIX}")
   endif()
   find_package(Geant4 QUIET COMPONENTS ui_all vis_all CONFIG PATHS ${Geant4_PREFIX} NO_DEFAULT_PATH)
else()
   if($ENV{VERBOSE})
      message(STATUS "No Geant4 prefix, searching default paths")
   endif()
   find_package(Geant4 QUIET COMPONENTS ui_all vis_all CONFIG)
endif()

if (NOT Geant4_FOUND AND USE_AFS)
   # Use AFS directory
   if(APPLE)
      message(STATUS "WARNING NO MACOS GEANT4 LIBRARIES AVAILABLE ON AFS")
   elseif(RHL5)
      set(Geant4_INCLUDE_DIRS /afs/cern.ch/sw/lcg/external/geant4/9.6.p03/share/include)
      set(Geant4_LIBRARY_DIR /afs/cern.ch/sw/lcg/external/geant4/9.6.p03/x86_64-slc5-gcc43-opt/lib64)
      set(Geant4_DIR /afs/cern.ch/sw/lcg/external/geant4/9.6.p03/x86_64-slc5-gcc43-opt/lib64/Geant4-9.6.3)
      set(Geant4_VERSION 9.6.3)
   elseif(RHL6)
      set(Geant4_INCLUDE_DIRS /afs/cern.ch/sw/lcg/external/geant4/10.0.p03/share/include)
      set(Geant4_LIBRARY_DIR /afs/cern.ch/sw/lcg/external/geant4/10.0.p03/x86_64-slc6-gcc47-opt/lib64)
      set(Geant4_DIR /afs/cern.ch/sw/lcg/external/geant4/10.0.p03/x86_64-slc6-gcc47-opt/lib64/Geant4-10.0.3)
      set(Geant4_VERSION 10.0.3)
   endif()
   find_package(Geant4 QUIET COMPONENTS ui_all vis_all PATHS ${Geant4_DIR} NO_DEFAULT_PATH)
endif()

if (Geant4_FOUND)
      message(STATUS "Geant4 Use File: ${Geant4_USE_FILE}")
      include(${Geant4_USE_FILE})
      message(STATUS "Geant4 Definitions: ${Geant4_DEFINITIONS}")

      if($ENV{VERBOSE})
        message(STATUS "Geant4_INCLUDE_DIRS: ${Geant4_INCLUDE_DIRS}")
        message(STATUS "Geant4_LIBRARY_DIR: ${Geant4_LIBRARY_DIR}")
        message(STATUS "Geant4_LIBRARIES: ${Geant4_LIBRARIES}")
      endif()
endif()

if (NOT Geant4_FOUND)
   # Module mode, should have found Geant4 already for 9.5 and newer
   # own module for versions of Geant4 (9.4 and older), last option:
   # ui_all and vis_all options not implemented in own FindGeant4.cmake

   # MODULE keyword not defined in versions 2.6
   if (${CMAKE_MAJOR_VERSION} GREATER 1 AND ${CMAKE_MINOR_VERSION} GREATER 7)
      find_package(Geant4 MODULE REQUIRED ui_all vis_all)
   else()
      find_package(Geant4 REQUIRED ui_all vis_all)
   endif()

   # Local UseGeant4.cmake
   set(Geant4_USE_FILE UseGeant4.cmake)
   include(${Geant4_USE_FILE})
   include_directories(${Geant4_INCLUDE_DIRS})

   # set visualisation flags (to be fixed to automagic .. easier in Geant4 9.6)
   # This should probably go soon
   option(VIS_RAYTRACER "RayTracer visualisation" OFF)
   option(VIS_RAYTRACERX "RayTracerX visualisation" OFF)
   option(VIS_OPENGLQT "OpenGLQT visualisation" OFF)
   option(VIS_OPENGLX "OpenGLX visualisation" OFF)
   option(VIS_OPENINVENTOR "Open Inventor Visualisation" OFF)

   set(VIS_FLAGS "-DG4VIS_USE")
   if(VIS_RAYTRACER)
	set(VIS_FLAGS "-DG4VIS_USE_RAYTRACER ${VIS_FLAGS}")
   endif()
   if(VIS_RAYTRACERX)
	set(VIS_FLAGS "-DG4VIS_USE_RAYTRACERX ${VIS_FLAGS}")
   endif()
   if(VIS_OPENGLQT)
	set(VIS_FLAGS "-DG4VIS_USE_OPENGLQT ${VIS_FLAGS}")
   endif()
   if(VIS_OPENGLX)
	set(VIS_FLAGS "-DG4VIS_USE_OPENGLX ${VIS_FLAGS}")
   endif()
   if(VIS_OPENINVENTOR)
	set(VIS_FLAGS "-DG4VIS_USE_OPENINVENTOR ${VIS_FLAGS}")
   endif()

   message(STATUS "Visualisation Flags: ${VIS_FLAGS}")
endif()

if (Geant4_VERSION AND NOT "${Geant4_VERSION}" MATCHES "UNKNOWN")
  string(SUBSTRING ${Geant4_VERSION} 0 1 G4_MAJOR_VERSION)
  if(${G4_MAJOR_VERSION} STREQUAL "1")
    string(SUBSTRING ${Geant4_VERSION} 0 2 G4_MAJOR_VERSION)
    string(SUBSTRING ${Geant4_VERSION} 3 1 G4_MINOR_VERSION) 
    string(SUBSTRING ${Geant4_VERSION} 5 1 G4_PATCH_LEVEL)
  else()
    string(SUBSTRING ${Geant4_VERSION} 2 1 G4_MINOR_VERSION) 
    string(SUBSTRING ${Geant4_VERSION} 4 1 G4_PATCH_LEVEL)
  endif()

  message(STATUS "G4_VERSION: ${Geant4_VERSION}")
  if($ENV{VERBOSE})
    message(STATUS "G4_MAJOR_VERSION: ${G4_MAJOR_VERSION}")
    message(STATUS "G4_MINOR_VERSION: ${G4_MINOR_VERSION}")
    message(STATUS "G4_PATCH_LEVEL: ${G4_PATCH_LEVEL}")
  endif()
else()
  set(G4_VERSION 9.9.9)
  set(G4_MAJOR_VERSION 9)
  set(G4_MINOR_VERSION 9)
  set(G4_PATCH_LEVEL 9)
endif()

link_directories(${CMAKE_CURRENT_SOURCE_DIR}/parser/)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# set compiler flags
set(WFLAGS "-Wall -Wextra -pedantic -Wno-long-long -Wpointer-arith -Woverloaded-virtual")
set(CMAKE_FLAGS "${CMAKE_FLAGS} ${CLHEP_DEFINITIONS}  ${WFLAGS}")
set(BDSIM_FLAGS "${CMAKE_FLAGS} ${VIS_FLAGS} -DCLHEP_VERSION=${CLHEP_VERSION} -DG4VERSION=${G4_MAJOR_VERSION} -DG4MINORVERSION=${G4_MINOR_VERSION} ${BDSIM_FLAGS}")

set(CMAKE_C_FLAGS "${CMAKE_C_ORIG_FLAGS} ${BDSIM_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_ORIG_FLAGS} ${BDSIM_FLAGS}")

if($ENV{VERBOSE})
	message(STATUS "BDSIM_FLAGS ${BDSIM_FLAGS}")
	message(STATUS "CMAKE_C_FLAGS ${CMAKE_C_FLAGS}")
	message(STATUS "CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")
endif()

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cnf.in ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cnf @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cnf
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
endif(DOXYGEN_FOUND)

#-----------------------------------------------------------------------
# macro BDSIM_COLLATE_APPLICATION_SOURCES(source_dest_var)
#
macro(BDSIM_COLLATE_APPLICATION_SOURCES source_dest_var)
  file(GLOB 
    ${source_dest_var} 
    ${CMAKE_CURRENT_SOURCE_DIR}/bdsim.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/BDS*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ggmad.cc
    )
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
endmacro()

# source files
BDSIM_COLLATE_APPLICATION_SOURCES(bdsim_sources)
if(NOT USE_LCDD)
       list(REMOVE_ITEM bdsim_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/BDSGeometryLCDD.cc)
endif()
if(NOT USE_GDML)
       list(REMOVE_ITEM bdsim_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/BDSGeometryGDML.cc)
endif()			
if($ENV{VERBOSE})
	message(STATUS "bdsim sources ${bdsim_sources}")
endif()
add_library(bdsim ${bdsim_sources})

# This module sets up installation:
include(bdsim_install)

add_subdirectory( parser )

add_executable(bdsimexec bdsim.cc)

set_target_properties(bdsimexec PROPERTIES OUTPUT_NAME "bdsim" VERSION ${BDSIM_VERSION})
target_link_libraries(bdsimexec bdsim gmad)
target_link_libraries(bdsim ${CLHEP_LIBRARIES})
target_link_libraries(bdsim ${XercesC_LIBRARIES})
#link_directories(${Geant4_LIBRARY_DIR})
target_link_libraries(bdsim ${Geant4_LIBRARIES})

if(USE_ROOT)
    target_link_libraries(bdsim ${ROOT_LIBRARIES_GLOB})
    target_link_libraries(bdsim ${ROOT_LIBRARIES})
endif()

if(USE_LCDD)
    target_link_libraries(bdsim ${XML_LIBRARIES})
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "DebugCoverage")
    target_link_libraries(bdsim gcov)
endif()

get_target_property(binaryname bdsimexec OUTPUT_NAME)
set(binary ${CMAKE_CURRENT_BINARY_DIR}/./${binaryname})

# Install the targets:
set(executables bdsimexec)
set(libraries bdsim)
set_target_properties(${executables} ${libraries} PROPERTIES VERSION ${BDSIM_VERSION})
bdsim_install_targets(${executables} ${libraries})
file(GLOB headers include/*.hh parser/*.h)
bdsim_install_headers(${headers})

# Utils directory
add_subdirectory(utils)

# Testing:
enable_testing()
include(CTest)
include(bdsim_test_macros)
add_subdirectory(examples)
add_subdirectory(test)
