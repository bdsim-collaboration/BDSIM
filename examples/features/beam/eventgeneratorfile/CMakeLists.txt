execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# data stored using HepMC2 library but loaded using HepMC3 in BDSIM
if (USE_HEPMC3)
  simple_testing(bunch-eventgeneratorfile-hepmc2             "--file=eventgeneratorfile-hepmc2.gmad"              "")
  simple_testing(bunch-eventgeneratorfile-hepmc2-loop        "--file=eventgeneratorfile-hepmc2-loop.gmad"         "")
  simple_testing(bunch-eventgeneratorfile-hepmc2-lowphysics  "--file=eventgeneratorfile-hepmc2-lowphysics.gmad"   "")
  simple_testing(bunch-eventgeneratorfile-hepmc2-offset      "--file=eventgeneratorfile-hepmc2-offset.gmad"       "")
  simple_testing(bunch-eventgeneratorfile-hepmc2-recreate    "--file=eventgeneratorfile-hepmc2-recreate.gmad"     "")
  if (DEFINED HEPMC3_ROOTIO_LIB)
    # we have no example file for now
    #simple_testing(bunch-eventgeneratorfile-hepmc2-root      "--file=eventgeneratorfile-hepmc2root.gmad"          "")
  endif()
endif()
