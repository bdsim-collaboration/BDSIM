set(TESTING_ARGS "--outfile=muon_e-")
simple_testing(muon_e-_LONG "--file=muon_e-.gmad" ${OVERLAP_CHECK})

set(TESTING_ARGS "--outfile=muon_e+")
simple_testing(muon_e+_LONG "--file=muon_e+.gmad" ${OVERLAP_CHECK})
