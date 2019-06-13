# quietly detect if we have HepMC library available - not required

option( USE_HEPMC "Include HepMC3 library support." OFF)
if (USE_HEPMC3)
  message(STATUS "Using HepMC3")
  find_package(HepMC3 REQUIRED COMPONENTS HepMC3 HepMC3fio)
  message(STATUS "HEPMC3 Use File: ${HEPMC3_ROOT_DIR}/share/HepMC3/cmake/HepMC3Config.cmake")
  include_directories("${HEPMC3_INCLUDE_DIR}")
  add_definitions(-DUSE_HEPMC3)
  #set(PREPROCESSOR_DEFS "-DUSE_HEPMC3")
endif()
