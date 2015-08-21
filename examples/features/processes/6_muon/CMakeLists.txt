set(TESTING_ARGS "--outfile=muon_e-")
simple_testing(muon_e- "--file=muon_e-.gmad" ${OVERLAP_CHECK})

set(TESTING_ARGS "--outfile=muon_e+")
simple_testing(muon_e+ "--file=muon_e+.gmad" ${OVERLAP_CHECK})
