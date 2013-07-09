## This file should be placed in the root directory of your project.
## Then modify the CMakeLists.txt file in the root directory of your
## project to incorporate the testing dashboard.
## # The following are required to uses Dart and the Cdash dashboard
##   ENABLE_TESTING()
##   INCLUDE(CTest)
set(CTEST_PROJECT_NAME "BDSIM")
set(CTEST_NIGHTLY_START_TIME "00:00:00 CET")
set(CTEST_DROP_METHOD "http")
set(CTEST_DROP_SITE "ylevinse.web.cern.ch/ylevinse")
set(CTEST_DROP_LOCATION "/cdash/submit.php?project=BDSIM")
set(CTEST_DROP_SITE_CDASH TRUE)
set(CTEST_UPDATE_COMMAND "svn")
#set(UPDATE_TYPE "cvs")

#add_test(NAME LHC1 COMMAND ${binary} --file=job_inj450_B1_blm.gmad --batch WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/examples/lhc/injection/B1/)
