# Taken from
# http://root.cern.ch/svn/root/trunk/cmake/modules/FindGEANT4.cmake
# with some additional changes

# - Try to find Geant4
# Once done this will define
#
#  Geant4_FOUND - system has Geant4
#  Geant4_INCLUDE_DIRS - the Geant4 include directory
#  Geant4_LIBRARIES - The libraries needed to use Geant4
#  Geant4_VERSION - The version of Geant4
#

message(STATUS "Looking for Geant4...")

# include directory

if (Geant4_PREFIX)
   if(EXISTS "${Geant4_PREFIX}/include/geant4")
      set(Geant4_INCLUDE_DIRS ${Geant4_PREFIX}/include/geant4)
   elseif(EXISTS "${Geant4_PREFIX}/include")
      set(Geant4_INCLUDE_DIRS ${Geant4_PREFIX}/include)
   endif()
   if(EXISTS "${Geant4_PREFIX}/lib")
      set(Geant4_LIBRARY_DIR ${Geant4_PREFIX}/lib)
   endif()
endif()

if (NOT Geant4_INCLUDE_DIRS)
  FIND_PATH(Geant4_INCLUDE_DIRS NAMES geant4)
endif (NOT Geant4_INCLUDE_DIRS)

if (NOT Geant4_INCLUDE_DIRS)
   message(STATUS "Geant4 include directory not found, trying default, please provide it with -DGeant4_INCLUDE_DIRS=")
   if (APPLE)
       set(Geant4_INCLUDE_DIRS /usr/local/include/geant4)
   else()
       set(Geant4_INCLUDE_DIRS /usr/include/geant4)
   endif()
else()
   if(EXISTS "${Geant4_INCLUDE_DIRS}/geant4")
       set(Geant4_INCLUDE_DIRS ${Geant4_INCLUDE_DIRS}/geant4)
   endif()
endif()

# library directory

if (APPLE)
   set(Geant4_LIBRARY_NAMES libG4FR.dylib libG4GMocren.dylib libG4OpenGL.dylib libG4RayTracer.dylib libG4Tree.dylib libG4VRML.dylib libG4digits_hits.dylib libG4error_propagation.dylib libG4event.dylib libG4geometry.dylib libG4gl2ps.dylib libG4global.dylib libG4graphics_reps.dylib libG4intercoms.dylib libG4interfaces.dylib libG4materials.dylib libG4modeling.dylib libG4parmodels.dylib libG4particles.dylib libG4persistency.dylib libG4physicslists.dylib libG4processes.dylib libG4readout.dylib libG4run.dylib libG4track.dylib libG4tracking.dylib libG4visHepRep.dylib libG4visXXX.dylib libG4vis_management.dylib libG4zlib.dylib)
else()
   set(Geant4_LIBRARY_NAMES libG4FR.so libG4GMocren.so libG4OpenGL.so libG4RayTracer.so libG4Tree.so libG4VRML.so libG4digits_hits.so libG4error_propagation.so libG4event.so libG4geometry.so libG4gl2ps.so libG4global.so libG4graphics_reps.so libG4intercoms.so libG4interfaces.so libG4materials.so libG4modeling.so libG4parmodels.so libG4particles.so libG4persistency.so libG4physicslists.so libG4processes.so libG4readout.so libG4run.so libG4track.so libG4tracking.so libG4visHepRep.so libG4visXXX.so libG4vis_management.so libG4zlib.so)
endif()
# remove G4zlib, which is somehow not there on SLC5 - LXPLUS, Geant4 9.5
if (LXPLUS AND USE_AFS AND RHL5)
   list(REMOVE_ITEM Geant4_LIBRARY_NAMES libG4zlib.so)
endif()
foreach (library_temp ${Geant4_LIBRARY_NAMES})
    # special way to unset Geant4_LIBRARY_temp
    set(Geant4_LIBRARY_temp ${library_temp}-NOTFOUND)

    find_library(Geant4_LIBRARY_temp NAMES ${library_temp} PATHS ${Geant4_LIBRARY_DIR} PATH_SUFFIXES geant4 Geant4)

    # prevent trailing space character
    if (DEFINED Geant4_LIBRARIES)
       if (NOT ${Geant4_LIBRARY_temp} STREQUAL Geant4_LIBRARY_temp-NOTFOUND)
           set(Geant4_LIBRARIES "${Geant4_LIBRARIES};${Geant4_LIBRARY_temp}")
       else()
           message(STATUS "WARNING ${library_temp} NOT FOUND")
       endif()
   else()
       set(Geant4_LIBRARIES "${Geant4_LIBRARY_temp}")
   endif()
      #message(STATUS "library_temp: ${library_temp} ${Geant4_LIBRARY_temp}")
      #message(STATUS "Geant4_LIBRARIES: ${Geant4_LIBRARIES}")
endforeach()
unset(Geant4_LIBRARY_temp CACHE)
#message(STATUS "Geant4_LIBRARY_NAMES: ${Geant4_LIBRARY_NAMES}")
#message(STATUS "Geant4_LIBRARIES: ${Geant4_LIBRARIES}")

if(NOT Geant4_LIBRARY_DIR AND NOT DEFINED Geant4_LIBRARIES)
   message(STATUS "Geant4 libraries not found, trying default, please provide directory with -DGeant4_LIBRARY_DIR=")
   if (APPLE)
      set(Geant4_LIBRARY_DIR /usr/local/lib)
   else()
      set(Geant4_LIBRARY_DIR /usr/lib)
   endif()
endif()

if (DEFINED Geant4_INCLUDE_DIRS AND DEFINED Geant4_LIBRARIES)
    set(Geant4_FOUND TRUE)
endif (DEFINED Geant4_INCLUDE_DIRS AND DEFINED Geant4_LIBRARIES)

if($ENV{VERBOSE})
    message(STATUS "Geant4_INCLUDE_DIRS: ${Geant4_INCLUDE_DIRS}")
    message(STATUS "Geant4_LIBRARY_DIR: ${Geant4_LIBRARY_DIR}")
    message(STATUS "Geant4_LIBRARIES: ${Geant4_LIBRARIES}")
endif()

if (Geant4_FOUND)
  if(Geant4_PREFIX AND EXISTS ${Geant4_PREFIX}/bin/geant4-config)
     set(_G4_CONFIG ${Geant4_PREFIX}/bin/geant4-config)
  else()
     find_program(_G4_CONFIG geant4-config)
  endif()
  if(_G4_CONFIG)
     execute_process(COMMAND ${_G4_CONFIG} --version
        OUTPUT_VARIABLE Geant4_VERSION
        OUTPUT_STRIP_TRAILING_WHITESPACE)
  elseif(NOT Geant4_VERSION)
     set(Geant4_VERSION "UNKNOWN")
  endif()
  unset(_G4_CONFIG CACHE)
  if (NOT Geant4_FIND_QUIETLY)
     message(STATUS "Looking for Geant4... - found version ${Geant4_VERSION}")
  endif (NOT Geant4_FIND_QUIETLY)
else (Geant4_FOUND)
  if (Geant4_FIND_REQUIRED)
     message(FATAL_ERROR "Looking for Geant4... - Not found")
  endif (Geant4_FIND_REQUIRED)
endif (Geant4_FOUND)

