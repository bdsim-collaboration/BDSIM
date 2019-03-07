
# set binary (for testing usage in macros)
#set (bdsimBinary BDSFieldTest)

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
target_link_libraries(tracker bdsim gmad)
#############
## tracker #
#############

add_executable(trackerexec tracker.cc)
set_target_properties(trackerexec PROPERTIES OUTPUT_NAME "tracker" VERSION ${BDSIM_VERSION})
target_link_libraries(trackerexec tracker)

bdsim_install_targets(trackerexec)


set (binary tracker)
