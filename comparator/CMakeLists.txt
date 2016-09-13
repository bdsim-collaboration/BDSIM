file(GLOB comparatorLibSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Remove mains from dictionary
list(REMOVE_ITEM comparatorLibSources ${CMAKE_CURRENT_SOURCE_DIR}/comparator.cc)

file(GLOB rebdsimLibLinkDefs ${CMAKE_CURRENT_SOURCE_DIR}/*LinkDef.hh)


#message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(comparator SHARED ${comparatorLibSources})
target_link_libraries(comparator bdsimRootEvent)

add_executable(comparatorexec comparator.cc)
set_target_properties(comparatorexec PROPERTIES OUTPUT_NAME "comparator" VERSION ${BDSIM_VERSION})
target_link_libraries(comparatorexec comparator bdsimRootEvent)

# Installation
bdsim_install_targets(comparatorexec comparator)

file(GLOB comparatorHeaders ${CMAKE_CURRENT_SOURCE_DIR}/*.hh)
bdsim_install_comparator_headers(${comparatorHeaders})

get_target_property(comparatorBinaryName comparatorexec OUTPUT_NAME)
set(comparatorBinary ${CMAKE_CURRENT_BINARY_DIR}/${comparatorBinaryName} CACHE STRING "comparator binary")
