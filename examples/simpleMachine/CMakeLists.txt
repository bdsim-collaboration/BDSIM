simple_testing(simpleMachine "--file=sm.gmad" ${OVERLAP_CHECK})

################################
# additional arguments testing #
################################

# no output formats
set(TESTING_ARGS --output=none)
simple_testing(simpleMachineNone "--file=sm.gmad" "")
# root event output format
set(TESTING_ARGS --output=rootevent)
simple_testing(simpleMachineROOTEvent "--file=sm.gmad" "")
# unknown output format
set(TESTING_ARGS --output=abcd)
simple_testing(simpleMachineUnknownOutputFormat "--file=sm.gmad" "")
set_tests_properties(simpleMachineUnknownOutputFormat PROPERTIES WILL_FAIL 1)

# Test other arguments
# print help
simple_testing(help "--help" "")
# print list of materials
simple_testing(materials "--materials" "")
# primaries only
set(TESTING_ARGS --generatePrimariesOnly)
simple_testing(generatePrimariesOnly "--file=sm.gmad" "")
# output survey
set(TESTING_ARGS --survey=survey.dat)
simple_testing(survey "--file=sm.gmad" "")
# output export geometry
if(USE_GDML)
  set(TESTING_ARGS --exportGeometryTo=export.gdml)
  simple_testing(exportgeometry "--file=sm.gmad" "")
endif()
# seed
set(TESTING_ARGS --seed=1729)
simple_testing(simpleMachineSeed "--file=sm.gmad" "")
# skip events
set(TESTING_ARGS --startFromEvent=10)
simple_testing(simpleMachineStartFromEvent "--file=sm.gmad" "")
# verbose
set(TESTING_ARGS --verbose)
simple_testing(simpleMachineVerbose "--file=sm.gmad" "")
# verbose event
set(TESTING_ARGS --verbose_event)
simple_testing(simpleMachineVerboseEvent "--file=sm.gmad" "")
# verbose event number
set(TESTING_ARGS --verbose_event_num=0)
simple_testing(simpleMachineVerboseEventNumber "--file=sm.gmad" "")
# verbose step
set(TESTING_ARGS --verbose_step)
simple_testing(simpleMachineVerboseStep "--file=sm.gmad" "")
# verbose levels
set(TESTING_ARGS "--verbose_G4run=5 --verbose_G4event=5 --verbose_G4tracking=5 --verbose_G4stepping=5")
simple_testing(simpleMachineVerboseLevels "--file=sm.gmad" "")


