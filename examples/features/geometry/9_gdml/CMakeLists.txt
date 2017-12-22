set(TESTING_ARGS --output=none)
simple_testing(magnet_outer_GDML "--file=1_magnet_outer.gmad" ${OVERLAP_CHECK})

set(TESTING_ARGS --output=none)
simple_testing(placement_GDML    "--file=2_placement.gmad" ${OVERLAP_CHECK})

set(TESTING_ARGS --output=none)
simple_testing(two_GDML_files    "--file=3_twogdmls.gmad" ${OVERLAP_CHECK})