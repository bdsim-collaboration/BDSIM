# This is a script for testing the source and submitting
# your results to a common server (cdash). You can see the server at:
# http://abp-cdash.web.cern.ch/abp-cdash/index.php?project=BDSIM
# 
# How to:
#  - check out the source from git:
#       http://www.pp.rhul.ac.uk/bdsim/manual/installation.html
#  - build bdsim with cmake
#  - set the CTEST_SOURCE_DIRECTORY accordingly below
#  - set a useful test site name (e.g. "your name"."machine type")
#  - set a useful build name (e.g. architecture, os and compiler used)
#  - Run this script with the command (with timeout of 15 minutes (900s):
#          ctest --timeout 900 -S cdash_bdsim.cmake

# Necessary edits:
set(CTEST_SITE "lxplus.cern.ch")
set(CTEST_BUILD_NAME "SLC6-Geant4.10.2")
# Your source should be checked out from git into this directory:
set(CTEST_SOURCE_DIRECTORY "/afs/cern.ch/work/j/jsnuveri/bdsim")
# and compilation will be done in this directory (no need to edit, must be unique for every configuration):
set(CTEST_BINARY_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/build-cdash-4.10.2")

# Optional edits (Nightly, Continuous or Experimental):
ctest_start(Experimental)
# CMake configuration options
set(cfg_options
 -DCMAKE_BUILD_TYPE=Release
 -DUSE_AFS=TRUE
)

###################################################
# Do not edit below (unless you know cmake/ctest) #
###################################################
set(CTEST_CMAKE_GENERATOR "Unix Makefiles")
set(CTEST_UPDATE_COMMAND "git")

ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})

# Update repository
ctest_update()

# Configure
ctest_configure(OPTIONS "${cfg_options}")

# Build
ctest_build()

# Run Tests
# Excluding long and robdsim tests
ctest_test(EXCLUDE "(LONG)|(robdsim)")
#ctest_test()

# Submit results to server
ctest_submit()
