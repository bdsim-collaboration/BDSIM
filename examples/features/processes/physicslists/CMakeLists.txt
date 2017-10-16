simple_testing(physics_cherenkov       "--file=cherenkov.gmad"       ${OVERLAP_CHECK})
simple_testing(physics_decay           "--file=decay.gmad"           ${OVERLAP_CHECK})
simple_testing(physics_em              "--file=em.gmad"              ${OVERLAP_CHECK})
simple_testing(physics_emextra         "--file=emextra.gmad"         ${OVERLAP_CHECK})
simple_testing(physics_emlow           "--file=emlow.gmad"           ${OVERLAP_CHECK})
simple_testing(physics_ftfpbert        "--file=ftfpbert.gmad"        ${OVERLAP_CHECK})
simple_testing_long(physics_ftfpberthp "--file=ftfpberthp.gmad"      ${OVERLAP_CHECK})
simple_testing(physics_hadronicelastic "--file=hadronicelastic.gmad" ${OVERLAP_CHECK})
simple_testing(physics_ion             "--file=ion.gmad"             ${OVERLAP_CHECK})
simple_testing(physics_muon            "--file=muon.gmad"            ${OVERLAP_CHECK})
simple_testing(physics_optical         "--file=optical.gmad"         ${OVERLAP_CHECK})
simple_testing(physics_qgspbert        "--file=qgspbert.gmad"        ${OVERLAP_CHECK})
simple_testing_long(physics_qgspberthp "--file=qgspberthp.gmad"      ${OVERLAP_CHECK})
simple_testing(physics_qgspbic         "--file=qgspbic.gmad"         ${OVERLAP_CHECK})
simple_testing_long(physics_qgspbichp  "--file=qgspbichp.gmad"       ${OVERLAP_CHECK})
simple_testing(physics_spindecay       "--file=spindecay.gmad"       ${OVERLAP_CHECK})
simple_testing(physics_synchrad        "--file=synchrad.gmad"        ${OVERLAP_CHECK})

# high precision tests take a while to load neutron data, so labelled long


# test invalid physics list
simple_testing(physics_invalid         "--file=gibberish.gmad"       ${OVERLAP_CHECK})
set_tests_properties(physics_invalid PROPERTIES WILL_FAIL 1)