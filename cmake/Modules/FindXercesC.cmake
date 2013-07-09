
# Modified from
# http://aliceinfo.cern.ch/static/aliroot-pro/cmake/modules/

# - Try to find XercesC
# Once done this will define
#
#  XercesC_FOUND - system has XercesC
#  XercesC_INCLUDE_DIR - the XercesC include directory
#  XercesC_LIBRARIES - The libraries needed to use XercesC
# 
# Currently not supporting static libraries..

# include directory:
if (NOT XercesC_INCLUDE_DIR)
   find_path(XercesC_INCLUDE_DIR NAMES XercesC xercesc)
   if($ENV{VERBOSE})
       message(STATUS "XercesC_INCLUDE_DIR: ${XercesC_INCLUDE_DIR}")
   endif()
endif()

#libraries:
if (XercesC_LIBRARY_DIR)
    # YIL simple array with all libraries:
    if(APPLE)
        file(GLOB XercesC_LIBRARIES ${XercesC_LIBRARY_DIR}/libxerces-c*.dylib)
    else()
        file(GLOB XercesC_LIBRARIES ${XercesC_LIBRARY_DIR}/libxerces-c*.so)
    endif()
else()
    FIND_LIBRARY(XercesC_LIBRARIES NAMES xerces-c PATH_SUFFIXES xercesc XercesC)
endif()

if($ENV{VERBOSE})
    message(STATUS "XercesC_LIBRARIES: ${XercesC_LIBRARIES}")
endif()

# final checks:
IF (XercesC_INCLUDE_DIR AND XercesC_LIBRARIES)
   SET(XercesC_FOUND TRUE)
ENDIF (XercesC_INCLUDE_DIR AND XercesC_LIBRARIES)

IF (XercesC_FOUND)
  IF (NOT _XercesC_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for XercesC... - found")
  endIF (NOT _XercesC_FIND_QUIETLY)
ELSE (XercesC_FOUND)
  IF (XercesC_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Looking for XercesC... - Not found")
  ENDIF (XercesC_FIND_REQUIRED)
ENDIF (XercesC_FOUND)
