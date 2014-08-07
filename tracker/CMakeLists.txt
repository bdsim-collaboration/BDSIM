cmake_minimum_required(VERSION 2.6.4)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Suppress policy CMP0042 warning (as of cmake 3.0.0)
if (${CMAKE_MAJOR_VERSION} GREATER 2)
    cmake_policy(SET CMP0042 OLD)
endif()

# suppress warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-sign-compare -Wno-unused-value")

if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DBDSDEBUG")
endif()
if(${CMAKE_BUILD_TYPE} STREQUAL "DebugCoverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -DBDSDEBUG --coverage")
endif()

if($ENV{VERBOSE})
	message(STATUS "PARSER CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS}")
endif()

add_library(tracker STATIC TRKTrackingInterface.cc TRKApertureInterface.cc TRKPlacement.cc TRKElement.cc TRKDrift.cc TRKQuadrupole.cc TRKParticle.cc)
add_library(trackerShared SHARED TRKTrackingInterface.cc TRKApertureInterface.cc TRKPlacement.cc TRKElement.cc TRKDrift.cc TRKQuadrupole.cc TRKParticle.cc)

bdsim_install_targets(tracker)
