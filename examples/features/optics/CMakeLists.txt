# find MadX quietly
find_package(MadX QUIET)
if (MADX_FOUND)
  message(STATUS "Found MadX: ${MADX_EXECUTABLE}")
  # add tests
  # 1_fodo
  tracking_test(1_TrackingTest_FODO --filepath=1_fodo/FODO.madx)
  # 2_drift
  tracking_test(2_TrackingTest_DRIFT --filepath=2_drift/DRIFT.madx)
  # 3_quadrupole
  tracking_test(3_TrackingTest_QUADRUPOLE --filepath=3_quadrupole/QUAD.madx)
  # 4_sextupole
  tracking_test(4_TrackingTest_SEXTUPOLE --filepath=4_sextupole/SEXT.madx)
endif()

