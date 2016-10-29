cmake_minimum_required(VERSION 2.8.12)

project(bdsim)

# project version
set( BDSIM_MAJOR_VERSION 0 )
set( BDSIM_MINOR_VERSION 9 )
set( BDSIM_PATCH_LEVEL develop )
set( BDSIM_VERSION ${BDSIM_MAJOR_VERSION}.${BDSIM_MINOR_VERSION}.${BDSIM_PATCH_LEVEL})

message(STATUS "Configuring BDSIM ${BDSIM_VERSION}")

# add source dir as a place for CMake modules
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

# include BDSIM macros
include(bdsim_macros)

# With this true we automatically include current src/binary directory
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# Default build type (defines different sets of compiler flags)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
       "Choose the type of build, options are: None Debug DebugOutput Release RelWithDebInfo MinSizeRel DebugProf DebugCoverage."
       FORCE)
endif()

message(STATUS "Build Type ${CMAKE_BUILD_TYPE}")

# Add compiler flags based on build type
include(cmake/CompilerBuildFlags.cmake)

# Check if compiler is compatible
include(cmake/CompilerCheck.cmake)

# Check AFS and OS
include(cmake/SystemCheck.cmake)

#################  
# Find Packages #
#################

# CLHEP
include(cmake/CLHEP.cmake)

# ROOT
include(cmake/ROOT.cmake)

# GDML (on by default)
option( USE_GDML "Include GDML support."  ON )
if(USE_GDML)
    add_definitions("-DUSE_GDML")
    message(STATUS "GDML support ON")
endif()

# LCDD (on by default if xml libraries are found)
include(cmake/XML.cmake)

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

# Find GEANT4
include(cmake/Geant4.cmake)

## set compiler flags
include(cmake/CompilerFlags.cmake)

# add a target to generate API documentation with Doxygen
find_package(Doxygen)
if(DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cnf.in ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cnf @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cnf
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
  # add 'make doxygen' to ctest
  add_test(Doxygen make doc)
  set_tests_properties(Doxygen PROPERTIES LABELS LONG)
endif(DOXYGEN_FOUND)

# enable Testing with ctest
enable_testing()
include(CTest)
include(bdsim_test_macros)
include(comparator_test_macros)
include(rebdsim_test_macros)

# This module sets up installation:
include(bdsim_install)

# Source files
string(TIMESTAMP CURRENT_YEAR %Y)
configure_file(${CMAKE_SOURCE_DIR}/bdsim.cc ${CMAKE_BINARY_DIR}/bdsim.cc @ONLY)

BDSIM_COLLATE_APPLICATION_SOURCES(bdsim_sources)
if(NOT USE_LCDD)
  list(REMOVE_ITEM bdsim_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/BDSGeometryLCDD.cc)
endif()
if(NOT USE_GDML)
  list(REMOVE_ITEM bdsim_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/BDSGeometryGDML.cc)
endif()
if($ENV{VERBOSE})
  message(STATUS "ROOT dictionaries ${root_dicts}")
  message(STATUS "BDSIM sources ${bdsim_sources}")
endif()

# Configuration information
add_subdirectory(configuration)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/configuration)

# Parser
add_subdirectory(parser)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/parser)
# include current directory for parser includes
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# external sources
#add_subdirectory(src-external)
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/src-external)

# BDSIM library and binaries
add_library(bdsim ${root_dicts} ${bdsim_sources} )

add_executable(bdsimexec ${CMAKE_BINARY_DIR}/bdsim.cc)

set_target_properties(bdsimexec PROPERTIES OUTPUT_NAME "bdsim" VERSION ${BDSIM_VERSION})
target_link_libraries(bdsimexec bdsim gmad)
target_link_libraries(bdsim ${CLHEP_LIBRARIES})
target_link_libraries(bdsim ${Geant4_LIBRARIES})
#target_link_libraries(bdsim gzstream)

# link against ROOT
target_link_libraries(bdsim ${ROOT_LIBRARIES})

if(USE_LCDD)
    target_link_libraries(bdsim ${XML_LIBRARIES})
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "DebugCoverage")
    target_link_libraries(bdsim gcov)
endif()

# set binary name for testing usage
get_target_property(binaryname bdsimexec OUTPUT_NAME)
set(bdsimBinary ${CMAKE_CURRENT_BINARY_DIR}/${binaryname})

# ROOT Event Library
set(bdsimRootEventSources ${root_dicts} ${root_files})

# Add parser options
list(APPEND bdsimRootEventSources ${CMAKE_CURRENT_SOURCE_DIR}/parser/optionsBase.cc)
if($ENV{VERBOSE})
  message(STATUS "bdsimRootEventSources ${bdsimRootEventSources}")
endif()

add_library(bdsimRootEvent SHARED ${bdsimRootEventSources})
target_compile_definitions(bdsimRootEvent PRIVATE "-D__ROOTBUILD__")
# link against ROOT
target_link_libraries(bdsimRootEvent ${ROOT_LIBRARIES})
# Add dependency to make sure bdsimRootEvent is done first
# This is done because the ROOT dictionary generation needs to be done separately for each of them
# And even if the dictionaries are the same compiling with multiple threads will result in building them simultaneously
add_dependencies(bdsim bdsimRootEvent)

# Install the targets:
set(executables bdsimexec)
if (${ROOT_MAJOR_VERSION} STREQUAL "6")
  set(libraries bdsim bdsimRootEvent)
else()
  set(libraries bdsim)
endif()
set_target_properties(${executables} ${libraries} PROPERTIES VERSION ${BDSIM_VERSION})
bdsim_install_targets(${executables} ${libraries})
bdsim_install_libs(${root_pcms})
file(GLOB headers ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh ${CMAKE_CURRENT_SOURCE_DIR}/parser/*.h)
bdsim_install_headers(${headers} ${root_dicts_headers})

include(CPack)

option(BUILD_MACOSX_APP "Build Mac OSX application" OFF)

if (BUILD_MACOSX_APP AND APPLE)
    # building a macos executablemaking a bundle of 
    add_executable(bdsimapp MACOSX_BUNDLE ${CMAKE_BINARY_DIR}/bdsim.cc)
    set_target_properties(bdsimapp PROPERTIES OUTPUT_NAME "bdsim" VERSION ${BDSIM_VERSION})
    target_link_libraries(bdsimapp bdsim gmad)
    bdsim_install_bundle(bdsim)
endif()

# package source
add_custom_target(dist
    COMMAND git archive --prefix=${ARCHIVE_NAME}/ HEAD
        | bzip2 > ${CMAKE_BINARY_DIR}/${ARCHIVE_NAME}.tar.bz2
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR})

# copy dependent libraries in deps directory
add_custom_target(copy-deps
  COMMAND ${CMAKE_SOURCE_DIR}/cmake/copyldd.sh bdsim ${CMAKE_BINARY_DIR}/deps
)

## Add subdirectories
# Visualisation directory
add_subdirectory(vis)

# Utils directory
add_subdirectory(utils)

# For testing
add_subdirectory(examples)
# copy examples to build directory - macro defined in bdsim_macros.cmake
copy_examples()
  
# Include directory for tests
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
add_subdirectory(test)

# Include directory for analysis
add_subdirectory(analysis)

# Include directory for comparator
add_subdirectory(comparator)

# Manual
add_subdirectory(manual)

# Mark some variables as advanced to clean up GUI
if(APPLE)
  mark_as_advanced(CMAKE_OSX_ARCHITECTURES)
  mark_as_advanced(CMAKE_OSX_DEPLOYMENT_TARGET)
  mark_as_advanced(CMAKE_OSX_SYSROOT)
endif()
