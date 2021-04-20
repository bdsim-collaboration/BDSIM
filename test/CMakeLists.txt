add_executable(BDSFieldTest FieldTest.cc)
set_target_properties(BDSFieldTest PROPERTIES OUTPUT_NAME "BDSFieldTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSFieldTest ${BDSIM_LIB_NAME} ${GMAD_LIB_NAME})
configure_file(purefieldplotter.py purefieldplotter.py COPYONLY)
configure_file(converttobdsim.py converttobdsim.py COPYONLY)

# set binary (for testing usage in macros)
#set (bdsimBinary BDSFieldTest)

add_executable(BDSInterpolatorTester BDSInterpolatorTester.cc)
set_target_properties(BDSInterpolatorTester PROPERTIES OUTPUT_NAME "BDSInterpolatorTester" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSInterpolatorTester ${BDSIM_LIB_NAME} ${GMAD_LIB_NAME})

add_executable(bdsim-linkexec link-test.cc)
set_target_properties(bdsim-linkexec PROPERTIES OUTPUT_NAME "BDSLink" VERSION ${BDSIM_VERSION})
target_link_libraries(bdsim-linkexec ${BDSIM_LIB_NAME} gmad)

add_executable(bdsim-sixtrackexec SixtrackTest.cc)
set_target_properties(bdsim-sixtrackexec PROPERTIES OUTPUT_NAME "bdssixtracktest" ${BDSIM_VERSIO})
target_link_libraries(bdsim-sixtrackexec ${BDSIM_LIB_NAME} gmad)

add_subdirectory(TrackingTestFiles)

add_executable(BDSTrajectoryTest TrajectoryTest.cc)
set_target_properties(BDSTrajectoryTest PROPERTIES OUTPUT_NAME "BDSTrajectoryTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSTrajectoryTest rebdsim bdsimRootEvent bdsim)

add_test(NAME "trajectory-functions" COMMAND BDSTrajectoryTest "../examples/features/data/trajectory-sample.root")

add_executable(TH1SetTest TH1SetTest.cc)
target_link_libraries(TH1SetTest ${BDSIM_LIB_NAME} ${ROOT_LIBRARIES} rebdsim)
