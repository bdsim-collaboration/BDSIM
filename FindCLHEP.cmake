
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

IF (NOT CLHEP_INCLUDE_DIR AND NOT CLHEP_LIBRARIES)
  SET (CLHEP_INCLUDE_DIR CLHEP_INCLUDE_DIR-NOTFOUND)
  SET (CLHEP_LIBRARIES CLHEP_LIBRARIES-NOTFOUND)
  SET (_CLHEP_FIND_QUIETLY FALSE)
ELSE (NOT CLHEP_INCLUDE_DIR AND NOT CLHEP_LIBRARIES)
  SET (_CLHEP_FIND_QUIETLY TRUE)
ENDIF (NOT CLHEP_INCLUDE_DIR AND NOT CLHEP_LIBRARIES)
IF (NOT _CLHEP_FIND_QUIETLY)
  MESSAGE(STATUS "Looking for CLHEP...")
ENDIF (NOT _CLHEP_FIND_QUIETLY)

# include directory:
FIND_PATH(CLHEP_INCLUDE_DIR NAMES CLHEP clhep)
message("CLHEP_INCLUDE_DIR: ${CLHEP_INCLUDE_DIR}")

#libraries:
#FIND_LIBRARY(CLHEP_LIBRARIES NAMES CLHEP PATH_SUFFIXES clhep CLHEP)

if (CLHEP_LIBRARY_DIR)
    # YIL simple array with all libraries:
    if(APPLE)
        file(GLOB CLHEP_LIBRARIES ${CLHEP_LIBRARY_DIR}/libCLHEP*.dylib)
    else()
        file(GLOB CLHEP_LIBRARIES ${CLHEP_LIBRARY_DIR}/libCLHEP*.so)
    endif()
endif()

#message("CLHEP_LIBRARIES: ${CLHEP_LIBRARIES}")

# final checks:
IF (CLHEP_INCLUDE_DIR AND CLHEP_LIBRARIES)
   SET(CLHEP_FOUND TRUE)
ENDIF (CLHEP_INCLUDE_DIR AND CLHEP_LIBRARIES)

IF (CLHEP_FOUND)
  IF (NOT _CLHEP_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for CLHEP... - found")
  endIF (NOT _CLHEP_FIND_QUIETLY)
ELSE (CLHEP_FOUND)
  IF (CLHEP_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Looking for CLHEP... - Not found")
  ENDIF (CLHEP_FIND_REQUIRED)
ENDIF (CLHEP_FOUND)
