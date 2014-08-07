# Copy directory
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(BDSBunchTest BDSBunchTest.cc)
set_target_properties(BDSBunchTest PROPERTIES OUTPUT_NAME "BDSBunchTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSBunchTest bdsim gmad)

# set binary
set (binary BDSBunchTest)

add_subdirectory(BDSBunchTestFiles)
