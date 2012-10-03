# Taken from
# http://root.cern.ch/svn/root/trunk/cmake/modules/FindGEANT4.cmake
# with some additional changes

# - Try to find Geant4
# Once done this will define
#
#  Geant4_FOUND - system has Geant4
#  Geant4_INCLUDE_DIR - the Geant4 include directory
#  Geant4_LIBRARIES - The libraries needed to use Geant4
#

message(STATUS "Looking for Geant4...")

# include directory

if (NOT Geant4_INCLUDE_DIR)
  FIND_PATH(Geant4_INCLUDE_DIR NAMES Geant4 geant4)
endif (NOT Geant4_INCLUDE_DIR)

if (NOT Geant4_INCLUDE_DIR)
   message(STATUS "Geant4 include directory not found, trying default, please provide it with -DGeant4_INCLUDE_DIR=")
   if (APPLE)
       set(Geant4_INCLUDE_DIR /usr/local/include)
   else()
       set(Geant4_INCLUDE_DIR /usr/include)
   endif()
endif()

# library directory

if(NOT Geant4_LIBRARY_DIR)
   if (APPLE)
      set(Geant4_LIBRARY_NAMES libG4FR.dylib libG4modeling.dylib libG4GMocren.dylib libG4parmodels.dylib libG4RayTracer.dylib libG4particles.dylib libG4Tree.dylib libG4persistency.dylib libG4VRML.dylib libG4physicslists.dylib libG4digits_hits.dylib libG4processes.dylib libG4error_propagation.dylib libG4readout.dylib libG4event.dylib libG4run.dylib libG4geometry.dylib libG4track.dylib libG4global.dylib libG4tracking.dylib libG4graphics_reps.dylib libG4visHepRep.dylib libG4intercoms.dylib libG4visXXX.dylib libG4interfaces.dylib libG4vis_management.dylib libG4materials.dylib libG4zlib.dylib)
   else()
      set(Geant4_LIBRARY_NAMES libG4FR.so libG4modeling.so libG4GMocren.so libG4parmodels.so libG4RayTracer.so libG4particles.so libG4Tree.so libG4persistency.so libG4VRML.so libG4physicslists.so libG4digits_hits.so libG4processes.so libG4error_propagation.so libG4readout.so libG4event.so libG4run.so libG4geometry.so libG4track.so libG4global.so libG4tracking.so libG4graphics_reps.so libG4visHepRep.so libG4intercoms.so libG4visXXX.so libG4interfaces.so libG4vis_management.so libG4materials.so libG4zlib.so)
   endif()
   foreach (library_temp ${Geant4_LIBRARY_NAMES})
      # special way to unset Geant4_LIBRARY_temp
      set(Geant4_LIBRARY_temp ${library_temp}-NOTFOUND)
      FIND_LIBRARY(Geant4_LIBRARY_temp NAMES ${library_temp} PATH_SUFFIXES geant4 Geant4)
      # prevent trailing space character
      if (Geant4_LIBRARIES)
          set(Geant4_LIBRARIES "${Geant4_LIBRARIES};${Geant4_LIBRARY_temp}")
      else()
          set(Geant4_LIBRARIES "${Geant4_LIBRARY_temp}")
      endif()
      #message(STATUS "library_temp: ${library_temp} ${Geant4_LIBRARY_temp}")
   endforeach()
   #message(STATUS "Geant4_LIBRARY_NAMES: ${Geant4_LIBRARY_NAMES}")
   #message(STATUS "Geant4_LIBRARIES: ${Geant4_LIBRARIES}")
endif()

if(NOT Geant4_LIBRARY_DIR AND NOT Geant4_LIBRARIES)
   message(STATUS "Geant4 libraries not found, trying default, please provide directory with -DGeant4_LIBRARY_DIR=")
   if (APPLE)
      set(Geant4_LIBRARY_DIR /usr/local/lib)
   else()
      set(Geant4_LIBRARY_DIR /usr/lib)
   endif()
endif()

if (NOT Geant4_LIBRARIES)
    # YIL simple array with all libraries:
    if(APPLE)
        file(GLOB Geant4_LIBRARIES ${Geant4_LIBRARY_DIR}/libG4*.dylib ${G4_SUPPORT})
    else()
        file(GLOB Geant4_LIBRARIES ${Geant4_LIBRARY_DIR}/libG4*.so)
    endif()
endif(NOT Geant4_LIBRARIES)

if (Geant4_INCLUDE_DIR AND Geant4_LIBRARIES)
    set(Geant4_FOUND TRUE)
endif (Geant4_INCLUDE_DIR AND Geant4_LIBRARIES)

if($ENV{VERBOSE})
    message(STATUS "Geant4_INCLUDE_DIR: ${Geant4_INCLUDE_DIR}")
    message(STATUS "Geant4_LIBRARY_DIR: ${Geant4_LIBRARY_DIR}")
    message(STATUS "Geant4_LIBRARIES: ${Geant4_LIBRARIES}")
endif()

if (Geant4_FOUND)
  if (NOT Geant4_FIND_QUIETLY)
    MESSAGE(STATUS "Looking for Geant4... - found ${Geant4_LIBRARY_DIR}")
#    message(STATUS "Found ${Geant4_LIBRARY_DIR}")
  endif (NOT Geant4_FIND_QUIETLY)
  SET(LD_LIBRARY_PATH ${LD_LIBRARY_PATH} ${Geant4_LIBRARY_DIR})
else (Geant4_FOUND)
  if (Geant4_FIND_REQUIRED)
    message(FATAL_ERROR "Looking for Geant4... - Not found")
  endif (Geant4_FIND_REQUIRED)
endif (Geant4_FOUND)

