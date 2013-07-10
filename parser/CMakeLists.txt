cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# suppress warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings")

if($ENV{VERBOSE})
	message(STATUS "PARSER CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")
endif()

bison_target(PARSER parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c)
flex_target(SCANNER parser.l  ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)
add_flex_bison_dependency(SCANNER PARSER)

# make sure cmake compiles as c++
SET_SOURCE_FILES_PROPERTIES( parser.tab.c lex.yy.c PROPERTIES LANGUAGE CXX )

# You can also use file globbing, but that can cause confusion
# when we have old makefiles and cmake next to each other.
# Reason is old makefiles creates parser.cc/lex.yy.cc in source folder..
add_library(gmad STATIC gmad.cc ${BISON_PARSER_OUTPUTS} ${FLEX_SCANNER_OUTPUTS})

if($ENV{VERBOSE})
	message(STATUS "BISON_PARSER_OUTPUTS ${BISON_PARSER_OUTPUTS}") 
	message(STATUS "FLEX_SCANNER_OUTPUTS ${FLEX_SCANNER_OUTPUTS}")
endif()

add_library(gmadShared SHARED gmad.cc getEnv.cc)

bdsim_install_targets(gmad gmadShared)
