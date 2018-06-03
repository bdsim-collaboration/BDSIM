cmake_minimum_required(VERSION 2.6.4)

# Suppress policy CMP0042 warning (as of cmake 3.0.0)
if (CMAKE_VERSION VERSION_LESS 3.9.0)
    cmake_policy(SET CMP0042 OLD)
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DTRKDEBUG")
endif()
if(${CMAKE_BUILD_TYPE} STREQUAL "DebugCoverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DTRKDEBUG --coverage")
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(../include)

file(GLOB TRKsources *.cc )

add_library(tracker SHARED ${TRKsources})
target_link_libraries(tracker bdsim)
target_link_libraries(tracker gmad)

bdsim_install_targets(tracker)

