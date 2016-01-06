cmake_minimum_required(VERSION 2.6.4)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(BISON 2.4 REQUIRED)
find_package(FLEX REQUIRED)

# Suppress policy CMP0042 warning (as of cmake 3.0.0)
if (${CMAKE_MAJOR_VERSION} GREATER 2)
  cmake_policy(SET CMP0042 OLD)
endif()

# suppress warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare -Wno-unused-value")
# disable clang warnings on bison/flex generated code
if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated-register -Wno-unneeded-internal-declaration")
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "DebugOutput")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DBDSDEBUG")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "DebugCoverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DBDSDEBUG --coverage")
endif()

if($ENV{VERBOSE})
  message(STATUS "PARSER CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")
endif()

# compile BISON, with debug if in Debug mode
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "DebugOutput")
  bison_target(PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cc
    COMPILE_FLAGS "-t")
else ()
  bison_target(PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cc)
endif()

flex_target(SCANNER parser.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.cc)
add_flex_bison_dependency(SCANNER PARSER)

# glob parser sources
file(GLOB GMAD_PARSER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
# remove parser main
list(REMOVE_ITEM GMAD_PARSER_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/gmad.cc)
if($ENV{VERBOSE})
  message(STATUS "GMAD parser sources ${GMAD_PARSER_SOURCES}")
endif()

add_library(gmad STATIC ${GMAD_PARSER_SOURCES}
  ${BISON_PARSER_OUTPUTS} ${FLEX_SCANNER_OUTPUTS})
add_library(gmadShared SHARED ${GMAD_PARSER_SOURCES}
  ${BISON_PARSER_OUTPUTS} ${FLEX_SCANNER_OUTPUTS})

if($ENV{VERBOSE})
  message(STATUS "BISON_PARSER_OUTPUTS ${BISON_PARSER_OUTPUTS}") 
  message(STATUS "FLEX_SCANNER_OUTPUTS ${FLEX_SCANNER_OUTPUTS}")
endif()

add_executable(gmadexec gmad.cc)
set_target_properties(gmadexec PROPERTIES OUTPUT_NAME "gmad" VERSION ${BDSIM_VERSION})
target_link_libraries(gmadexec gmad)

bdsim_install_targets(gmadexec gmad gmadShared)

# set binary name for testing usage
get_target_property(binaryname gmadexec OUTPUT_NAME)
set(gmadBinary ${CMAKE_CURRENT_BINARY_DIR}/${binaryname})

# add tests
add_subdirectory(test)
