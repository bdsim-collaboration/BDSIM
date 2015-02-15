cmake_minimum_required(VERSION 2.6.4)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(BISON REQUIRED)
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
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DBDSDEBUG")
endif()
if(${CMAKE_BUILD_TYPE} STREQUAL "DebugCoverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DBDSDEBUG --coverage")
endif()

if($ENV{VERBOSE})
	message(STATUS "PARSER CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")
endif()

# compile BISON, with debug if in Debug mode
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
   bison_target(PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
COMPILE_FLAGS "-t")
else ()
   bison_target(PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c)
endif()

flex_target(SCANNER parser.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
add_flex_bison_dependency(SCANNER PARSER)

# make sure cmake compiles as c++
SET_SOURCE_FILES_PROPERTIES(parser.tab.c lex.yy.c PROPERTIES LANGUAGE CXX )

# You can also use file globbing, but that can cause confusion
# when we have old makefiles and cmake next to each other.
# Reason is old makefiles creates parser.cc/lex.yy.cc in source folder..

file(GLOB gmadsources gmad.cc getEnv.cc element.cc elementlist.cc enums.cc options.cc parameters.cc)

add_library(gmad STATIC ${gmadsources} ${BISON_PARSER_OUTPUTS} ${FLEX_SCANNER_OUTPUTS})

if($ENV{VERBOSE})
	message(STATUS "BISON_PARSER_OUTPUTS ${BISON_PARSER_OUTPUTS}") 
	message(STATUS "FLEX_SCANNER_OUTPUTS ${FLEX_SCANNER_OUTPUTS}")
endif()

add_library(gmadShared SHARED ${gmadsources} ${BISON_PARSER_OUTPUTS} ${FLEX_SCANNER_OUTPUTS})

bdsim_install_targets(gmad gmadShared)
