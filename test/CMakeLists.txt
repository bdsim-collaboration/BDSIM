add_executable(BDSHistogramTest BDSHistogramTest.cc)
set_target_properties(BDSHistogramTest PROPERTIES OUTPUT_NAME "BDSHistogramTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSHistogramTest bdsim gmad)

add_subdirectory(TrackingTestFiles)


