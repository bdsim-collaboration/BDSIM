set(TESTING_ARGS "--outfile=muon_e-")
simple_testing(muon_e- "--file=muon_e-.gmad" ${OVERLAP_CHECK})

set(TESTING_ARGS "--outfile=muon_e+")
simple_testing(muon_e+ "--file=muon_e+.gmad" ${OVERLAP_CHECK})

simple_testing(muon-splitting-piplus          "--file=muon_splitting_piplus_decay.gmad" "")
simple_testing(muon-splitting-piplus-biased   "--file=muon_splitting_piplus_decay_biased.gmad" "")
simple_testing(muon-splitting-kaonplus        "--file=muon_splitting_kaonplus_decay.gmad" "")

if (G4_MINOR_VERSION GREATER 2)
  simple_testing(muon-splitting-eplus           "--file=muon_splitting_eplus_annihilation.gmad" "")
  simple_testing(muon-splitting-eplus-biased    "--file=muon_splitting_eplus_annihilation_biased.gmad" "")
endif()