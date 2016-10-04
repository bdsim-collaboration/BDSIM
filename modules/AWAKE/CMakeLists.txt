# AWAKE module

BDSIM_COLLATE_APPLICATION_SOURCES(awake_sources)

if($ENV{VERBOSE})
  message(STATUS "AWAKE sources ${awake_sources}")
endif()

# add sources to bdsim sources
set(bdsim_sources ${bdsim_sources};${awake_sources} PARENT_SCOPE)

#file(GLOB AWAKE_sources
#  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh
#  ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
#  )

#add_library(bdsim-AWAKE ${AWAKE_sources})

