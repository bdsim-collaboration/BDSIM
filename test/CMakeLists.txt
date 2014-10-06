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

## TRKAperture

# add test executable
add_executable(TRKApertureTest1 TRKApertureTest1.cc)
set_target_properties(TRKApertureTest1 PROPERTIES OUTPUT_NAME "TRKApertureTest1" VERSION ${BDSIM_VERSION})
target_link_libraries(TRKApertureTest1 bdsim tracker gmad)

# set binary
set (binary TRKApertureTest1)

# TRKBunchTest
add_executable(TRKBunchTest TRKBunchTest.cc)
set_target_properties(TRKBunchTest PROPERTIES OUTPUT_NAME "TRKBunchTest" VERSION ${BDSIM_VERSION})
target_link_libraries(TRKBunchTest bdsim tracker gmad)

# set binary
set (binary TRKBunchTest)

add_subdirectory(BDSBunchTestFiles)
add_subdirectory(BDSTRKTestFiles)
add_subdirectory(TRKBunchTestFiles)

