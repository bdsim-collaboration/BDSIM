# Copy files to build directory
#execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

add_executable(BDSBunchTest BDSBunchTest.cc)
set_target_properties(BDSBunchTest PROPERTIES OUTPUT_NAME "BDSBunchTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSBunchTest bdsim gmad)

# set binary (for testing usage in macros)
set (bdsimBinary BDSBunchTest)

#add_executable(BDSFieldTest FieldTest.cc)
#set_target_properties(BDSFieldTest PROPERTIES OUTPUT_NAME "BDSFieldTest" VERSION ${BDSIM_VERSION})
#target_link_libraries(BDSFieldTest bdsim gmad)

# set binary (for testing usage in macros)
#set (bdsimBinary BDSFieldTest)


add_subdirectory(BDSBunchTestFiles)
add_subdirectory(TrackingTestFiles)
