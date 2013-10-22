
# Taken from
# http://aliceinfo.cern.ch/static/aliroot-pro/cmake/modules/
# with some additional changes...

# - Try to find CLHEP
# Once done this will define
#
#  CLHEP_FOUND - system has CLHEP
#  CLHEP_INCLUDE_DIR - the CLHEP include directory
#  CLHEP_LIBRARIES - The libraries needed to use CLHEP
# 
# Currently not supporting static libraries..

set(CLHEP_INCLUDE_DIR $ENV{CLHEP_INCLUDE_DIR})

# include directory:
if (NOT CLHEP_INCLUDE_DIR)
  find_path(CLHEP_INCLUDE_DIR clhep-2.0.3.3/include NAMES CLHEP clhep)
  message(STATUS "CLHEP_INCLUDE_DIR=" ${CLHEP_INCLUDE_DIR})	
endif()

#libraries:
message(STATUS "CLHEP_INCLUDE_DIR=" ${CLHEP_INCLUDE_DIR})	

if (CLHEP_INCLUDE_DIR)
    set(CLHEP_LIBRARY_DIR "${CLHEP_INCLUDE_DIR}/../lib")	
    message(STATUS "${CLHEP_LIBRARY_DIR}=${${CLHEP_LIBRARY_DIR}}")
    find_library(CLHEP_LIBRARIES NAMES CLHEP HINTS "${CLHEP_LIBRARY_DIR}")
else()
    find_library(CLHEP_LIBRARIES NAMES CLHEP PATH_SUFFIXES clhep CLHEP)
endif()

if($ENV{VERBOSE})
  message(STATUS "CLHEP_INCLUDE_DIR: ${CLHEP_INCLUDE_DIR}")
  message(STATUS "CLHEP_LIBRARY_DIR: ${CLHEP_LIBRARY_DIR}")
  message(STATUS "CLHEP_LIBRARIES: ${CLHEP_LIBRARIES}")
endif()

# final checks:
IF (CLHEP_INCLUDE_DIR AND CLHEP_LIBRARIES)
   SET(CLHEP_FOUND TRUE)
ENDIF (CLHEP_INCLUDE_DIR AND CLHEP_LIBRARIES)

IF (CLHEP_FOUND)
  IF (NOT _CLHEP_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for CLHEP... - found")
  ENDIF (NOT _CLHEP_FIND_QUIETLY)
ELSE (CLHEP_FOUND)
  IF (CLHEP_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Looking for CLHEP... - Not found")
  ENDIF (CLHEP_FIND_REQUIRED)
ENDIF (CLHEP_FOUND)
