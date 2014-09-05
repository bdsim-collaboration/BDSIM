## BDSBunchTest

add_executable(BDSBunchTest BDSBunchTest.cc)
set_target_properties(BDSBunchTest PROPERTIES OUTPUT_NAME "BDSBunchTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSBunchTest bdsim gmad)

# set binary
set (binary BDSBunchTest)

## BDSTrkTest

# add test executable
add_executable(BDSTRKTest BDSTRKTest.cc)
set_target_properties(BDSTRKTest PROPERTIES OUTPUT_NAME "BDSTRKTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSTRKTest tracker gmad)

# set binary
set (binary BDSTRKTest)

add_subdirectory(BDSBunchTestFiles)
add_subdirectory(BDSTrackingTestFiles)

