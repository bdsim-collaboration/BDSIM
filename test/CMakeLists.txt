###############
## BDSBunchTest #
###############

add_executable(BDSBunchTest BDSBunchTest.cc)
set_target_properties(BDSBunchTest PROPERTIES OUTPUT_NAME "BDSBunchTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSBunchTest bdsim gmad)

# set binary
set (binary BDSBunchTest)
add_subdirectory(BDSBunchTestFiles)

#############
## BDSTrkTest #
#############

# add test executable
add_executable(BDSTRKTest BDSTRKTest1.cc)
set_target_properties(BDSTRKTest PROPERTIES OUTPUT_NAME "BDSTRKTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSTRKTest bdsim tracker gmad)

# set binary
set (binary BDSTRKTest)
add_subdirectory(BDSTRKTestFiles)

###############
## TRKBunchTest #
###############
add_executable(TRKBunchTest TRKBunchTest.cc)
set_target_properties(TRKBunchTest PROPERTIES OUTPUT_NAME "TRKBunchTest" VERSION ${BDSIM_VERSION})
target_link_libraries(TRKBunchTest bdsim tracker gmad)

# add different tests subdirectories
set (binary TRKBunchTest)
add_subdirectory(TRKBunchTestFiles)
