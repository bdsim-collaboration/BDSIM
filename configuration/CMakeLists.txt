# record which version of bdsim is being built in the code itself
find_package(git)
if (GIT_FOUND)
  exec_program(
    ${GIT_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ARGS "describe --dirty"
    OUTPUT_VARIABLE VERSION_SHA1 )
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/version.hh.in ${CMAKE_CURRENT_BINARY_DIR}/version.hh @ONLY)
endif()
