simple_testing(simpleMachine "--file=sm.gmad" ${OVERLAP_CHECK})

################################
# additional arguments testing #
################################

# all output formats
set(TESTING_ARGS --output=combined)
simple_testing(simpleMachineCombined "--file=sm.gmad" "")
# no output formats
set(TESTING_ARGS --output=none)
simple_testing(simpleMachineNone "--file=sm.gmad" "")
if (USE_ROOT)
  # root (reduced) output format
  set(TESTING_ARGS --output=root)
  simple_testing(simpleMachineROOT "--file=sm.gmad" "")
  # root detailed output format
  set(TESTING_ARGS --output=rootdetailed)
  simple_testing(simpleMachineROOTDetailed "--file=sm.gmad" "")
endif()
# print help
simple_testing(help "--help" "")
# print list of materials
simple_testing(materials "--materials" "")
# output survey
set(TESTING_ARGS --survey=survey.dat)
simple_testing(survey "--file=sm.gmad" "")
# output export geometry
set(TESTING_ARGS --exportgeometryto=export.gdml)
simple_testing(exportgeometry "--file=sm.gmad" "")
# seed
set(TESTING_ARGS --seed=1729)
simple_testing(simpleMachineSeed "--file=sm.gmad" "")
# seed state (uses seedstate from previous test)
set(TESTING_ARGS --seed=1729 --seedstate=output.seedstate.txt)
simple_testing(simpleMachineSeedState "--file=sm.gmad" "")
set_tests_properties(simpleMachineSeedState PROPERTIES DEPENDS simpleMachineSeed)
# verbose
set(TESTING_ARGS --verbose)
simple_testing(simpleMachineVerbose "--file=sm.gmad" "")
# verbose event
set(TESTING_ARGS --verbose_event)
simple_testing(simpleMachineVerboseEvent "--file=sm.gmad" "")
# verbose step
set(TESTING_ARGS --verbose_step)
simple_testing(simpleMachineVerboseStep "--file=sm.gmad" "")
# verbose levels
set(TESTING_ARGS "--verbose_G4run=5 --verbose_G4event=5 --verbose_G4tracking=5 --verbose_G4stepping=5")
simple_testing(simpleMachineVerboseLevels "--file=sm.gmad" "")


