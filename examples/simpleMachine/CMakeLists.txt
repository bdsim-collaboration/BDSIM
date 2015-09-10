simple_testing(simpleMachine "--file=sm.gmad" ${OVERLAP_CHECK})

# additional arguments testing
# test help
simple_testing(help "--help" "")
# test survey
set(TESTING_ARGS --survey=survey.dat)
simple_testing(survey "--file=sm.gmad" "")
# test export geometry
set(TESTING_ARGS --exportgeometryto=export.gdml)
simple_testing(exportgeometry "--file=sm.gmad" "")
