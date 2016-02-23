
add_executable(BDSBunchTest BDSBunchTest.cc)
set_target_properties(BDSBunchTest PROPERTIES OUTPUT_NAME "BDSBunchTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSBunchTest bdsim gmad)


add_executable(BDSHistogramTest BDSHistogramTest.cc)
set_target_properties(BDSHistogramTest PROPERTIES OUTPUT_NAME "BDSHistogramTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSHistogramTest bdsim gmad)

# set binary (for testing usage in macros)
set (bdsimBinary BDSBunchTest)

add_subdirectory(BDSBunchTestFiles)
add_subdirectory(TrackingTestFiles)


