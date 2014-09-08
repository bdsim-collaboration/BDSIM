cmake_minimum_required(VERSION 2.6.4)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Suppress policy CMP0042 warning (as of cmake 3.0.0)
if (${CMAKE_MAJOR_VERSION} GREATER 2)
    cmake_policy(SET CMP0042 OLD)
endif()

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DTRKDEBUG")
endif()
if(${CMAKE_BUILD_TYPE} STREQUAL "DebugCoverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DTRKDEBUG --coverage")
endif()

file(GLOB TRKsources *.cc )

add_library(tracker STATIC ${TRKsources})

#file(GLOB gmadsources ../parser/gmad.cc ../parser/getEnv.cc ../parser/element.cc ../parser/elementlist.cc ../parser/enums.cc ../parser/options.cc ../parser/parameters.cc)
#add_library(trackerShared SHARED ${TRKsources} ${gmadsources})

bdsim_install_targets(tracker)
