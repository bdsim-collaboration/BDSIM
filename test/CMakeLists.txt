add_executable(BDSHistogramTest BDSHistogramTest.cc)
set_target_properties(BDSHistogramTest PROPERTIES OUTPUT_NAME "BDSHistogramTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSHistogramTest bdsim gmad)

# set binary (for testing usage in macros)
set (bdsimBinary BDSBunchTest)

#add_executable(BDSFieldTest FieldTest.cc)
#set_target_properties(BDSFieldTest PROPERTIES OUTPUT_NAME "BDSFieldTest" VERSION ${BDSIM_VERSION})
#target_link_libraries(BDSFieldTest bdsim gmad)

# set binary (for testing usage in macros)
#set (bdsimBinary BDSFieldTest)


add_subdirectory(TrackingTestFiles)


