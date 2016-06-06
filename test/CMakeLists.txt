add_executable(BDSHistogramTest BDSHistogramTest.cc)
set_target_properties(BDSHistogramTest PROPERTIES OUTPUT_NAME "BDSHistogramTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSHistogramTest bdsim gmad)

add_subdirectory(TrackingTestFiles)

#############
## BDSTRKTest #
#############

add_executable(BDSTRKTest BDSTRKTest1.cc)
set_target_properties(BDSTRKTest PROPERTIES OUTPUT_NAME "BDSTRKTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSTRKTest bdsim tracker gmad)

# set binary
set (binary BDSTRKTest)

add_subdirectory(BDSTRKTestFiles)
