# find MadX quietly
find_package(MadX QUIET)
#find_package(PythonLibs)
find_package(PythonInterp QUIET)

# find python modules
include(FindPythonModule)
find_python_module(root_numpy QUIET)
find_python_module(pybdsim QUIET)
find_python_module(pymadx QUIET)

# configure tester with correct BDSIM and MadX executables
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/run_bdsimMadx.py.in ${CMAKE_CURRENT_BINARY_DIR}/run_bdsimMadx.py)

if (MADX_FOUND AND USE_ROBDSIM AND PY_ROOT_NUMPY AND PY_PYBDSIM AND PY_PYMADX)
  message(STATUS "Tracking tests enabled")
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
  # 5_dba
  tracking_test(5_TrackingTest_DBA --filepath=5_dba/DBA.madx)
  # 6_rbend
  tracking_test(6_TrackingTest_RBEND --filepath=6_rbend/RBEND.madx)
  # 7_sbend
  tracking_test(7_TrackingTest_SBEND --filepath=7_sbend/SBEND.madx)
  # 9_octupole
  tracking_test(9_TrackingTest_OCTUPOLE --filepath=9_octupole/OCT.madx)
  # 10_hkicker
  tracking_test(10_TrackingTest_HKICKER --filepath=10_hkicker/hkicker.madx)
endif()

