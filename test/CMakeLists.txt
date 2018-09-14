add_executable(BDSFieldTest FieldTest.cc)
set_target_properties(BDSFieldTest PROPERTIES OUTPUT_NAME "BDSFieldTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSFieldTest bdsim gmad)
configure_file(purefieldplotter.py purefieldplotter.py COPYONLY)

# set binary (for testing usage in macros)
#set (bdsimBinary BDSFieldTest)

add_executable(BDSInterpolatorTester BDSInterpolatorTester.cc)
set_target_properties(BDSInterpolatorTester PROPERTIES OUTPUT_NAME "BDSInterpolatorTester" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSInterpolatorTester gmad bdsim)


add_subdirectory(TrackingTestFiles)


