simple_testing(physics_cherenkov       "--file=cherenkov.gmad"       ${OVERLAP_CHECK})
simple_testing(physics_decay           "--file=decay.gmad"           ${OVERLAP_CHECK})
simple_testing(physics_em              "--file=em.gmad"              ${OVERLAP_CHECK})
simple_testing(physics_emextra         "--file=emextra.gmad"         ${OVERLAP_CHECK})
simple_testing(physics_emlow           "--file=emlow.gmad"           ${OVERLAP_CHECK})
simple_testing(physics_empenelope      "--file=empenelope.gmad"      ${OVERLAP_CHECK})
simple_testing(physics_emlivermore     "--file=emlivermore.gmad"     ${OVERLAP_CHECK})
simple_testing(physics_emlivermorepolarised "--file=emlivermorepolarised.gmad" ${OVERLAP_CHECK})
simple_testing(physics_emlowep         "--file=emlowep.gmad"         ${OVERLAP_CHECK})
simple_testing(physics_emgs            "--file=emgs.gmad"            ${OVERLAP_CHECK})
simple_testing(physics_emss            "--file=emss.gmad"            ${OVERLAP_CHECK})
simple_testing(physics_emwvi           "--file=emwvi.gmad"           ${OVERLAP_CHECK})
simple_testing(physics_em1             "--file=em1.gmad"             ${OVERLAP_CHECK})
simple_testing(physics_em2             "--file=em2.gmad"             ${OVERLAP_CHECK})
simple_testing(physics_em3             "--file=em3.gmad"             ${OVERLAP_CHECK})
simple_testing(physics_em4             "--file=em3.gmad"             ${OVERLAP_CHECK})
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
simple_fail(physics_invalid         "--file=gibberish.gmad"       ${OVERLAP_CHECK})

# these require the optional G4PARTICLEHPDATA set from the Geant4 website
simple_fail(physics_ionphp          "--file=ionphp.gmad"          ${OVERLAP_CHECK})
simple_fail(physics_ioninclxx       "--file=ioninclxx.gmad"       ${OVERLAP_CHECK})
simple_fail(physics_ionbinary       "--file=ionbinary.gmad"       ${OVERLAP_CHECK})