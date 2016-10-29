# Configure source files
string(TIMESTAMP CURRENT_YEAR %Y)
configure_file(${CMAKE_SOURCE_DIR}/interpolator/bdsinterpolator.cc ${CMAKE_BINARY_DIR}/interpolator/bdsinterpolator.cc @ONLY)

file(GLOB interpolatorLibSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Remove mains from dictionary
list(REMOVE_ITEM interpolatorLibSources ${CMAKE_CURRENT_SOURCE_DIR}/bdsinterpolator.cc)

#message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}")

add_library(interpolator SHARED ${interpolatorLibSources})
target_link_libraries(interpolator bdsim)

add_executable(interpolatorexec ${CMAKE_BINARY_DIR}/interpolator/bdsinterpolator.cc)
set_target_properties(interpolatorexec PROPERTIES OUTPUT_NAME "bdsinterpolator" VERSION ${BDSIM_VERSION})
target_link_libraries(interpolatorexec interpolator bdsim gmad ${CLHEP_LIBRARIES})
target_link_libraries(interpolatorexec ${Geant4_LIBRARIES})

# Installation
bdsim_install_targets(interpolatorexec interpolator)

file(GLOB interpolatorHeaders ${CMAKE_CURRENT_SOURCE_DIR}/*.hh)
bdsim_install_interpolator_headers(${interpolatorHeaders})

get_target_property(interpolatorBinaryName interpolatorexec OUTPUT_NAME)
set(interpolatorBinary ${CMAKE_CURRENT_BINARY_DIR}/${interpolatorBinaryName} CACHE STRING "interpolator binary")
