## BDSBunchTest

add_executable(BDSBunchTest BDSBunchTest.cc)
set_target_properties(BDSBunchTest PROPERTIES OUTPUT_NAME "BDSBunchTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSBunchTest bdsim gmad)

# set binary
set (binary BDSBunchTest)

## BDSTrkTest

# add test executable
add_executable(BDSTRKTest BDSTRKTest1.cc)
set_target_properties(BDSTRKTest PROPERTIES OUTPUT_NAME "BDSTRKTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSTRKTest bdsim tracker gmad)

# set binary
set (binary BDSTRKTest)

# TRKBunchTest
add_executable(TRKBunchTest TRKBunchTest.cc)
set_target_properties(TRKBunchTest PROPERTIES OUTPUT_NAME "TRKBunchTest" VERSION ${BDSIM_VERSION})
target_link_libraries(TRKBunchTest bdsim tracker gmad)

# set binary
set (binary TRKBunchTest)

add_subdirectory(BDSBunchTestFiles)
add_subdirectory(BDSTrackingTestFiles)
add_subdirectory(TRKBunchTestFiles)

