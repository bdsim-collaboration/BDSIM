simple_testing(simpleMachine "--file=sm.gmad" ${OVERLAP_CHECK})

# additional arguments testing
# all output formats
set(TESTING_ARGS --output=combined)
simple_testing(simpleMachineCombined "--file=sm.gmad" "")
# no output formats
set(TESTING_ARGS --output=none)
simple_testing(simpleMachineNone "--file=sm.gmad" "")
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
