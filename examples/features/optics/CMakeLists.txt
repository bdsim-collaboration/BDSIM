# find MadX quietly
find_package(MadX QUIET)
if (MADX_FOUND)
  message(STATUS "Found MadX: ${MADX_EXECUTABLE}")
  # add tests
  # 1_fodo
  # 2_drift
  # 3_quadrupole
  # 4_sextupole
endif()

