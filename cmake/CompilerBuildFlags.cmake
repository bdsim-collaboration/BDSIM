# Compiler flags based on build type 
if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "DebugOutput")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -DBDSDEBUG")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "DebugCoverage")
  if(NOT CMAKE_COMPILER_IS_GNUCXX)
    message(WARNING "DebugCoverage only works with gcc compiler")
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "None")
  # if specifically None optimise
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -ffast-math -DNDEBUG")
endif()

# Navigation debugging
option( DEBUGNAVIGATION "Debug navigation" OFF )
if (DEBUGNAVIGATION)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DDEBUGNAV")
    message(STATUS "Debug navigation ON")
endif()