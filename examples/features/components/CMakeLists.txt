simple_testing(Component-CSampler "--file=csampler.gmad" ${OVERLAP_CHECK})
 
simple_testing(Component-Decapole "--file=deca.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Degrader "--file=degrader.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Drift "--file=drift.gmad" ${OVERLAP_CHECK})

simple_testing(Component-ECollimator "--file=ecol.gmad" ${OVERLAP_CHECK})

simple_testing(Component-GDML "--file=gdml.gmad" ${OVERLAP_CHECK})

simple_testing(Component-HKick "--file=hkick.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Laserwire "--file=laser.gmad" ${OVERLAP_CHECK})
 
simple_testing(Component-Multipole "--file=mult.gmad" ${OVERLAP_CHECK})

simple_testing(Component-ThinMultipole "--file=thinMult.gmad" ${OVERLAP_CHECK})

simple_testing(Component-MuonSpoiler "--file=musp.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Octupole "--file=oct.gmad" ${OVERLAP_CHECK})
 
simple_testing(Component-Quadrupole "--file=quad.gmad" ${OVERLAP_CHECK})

simple_testing(Component-QuadrupoleOffset "--file=quadOffset.gmad" ${OVERLAP_CHECK})

simple_testing(Component-RBend "--file=rben.gmad" ${OVERLAP_CHECK})

simple_testing(Component-RBendPoleFace "--file=rbenPoleFace.gmad" ${OVERLAP_CHECK})

simple_testing(Component-RCollimator "--file=rcol.gmad" ${OVERLAP_CHECK})

simple_testing(Component-RF_LONG "--file=rf.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Sampler "--file=sampler.gmad" ${OVERLAP_CHECK})

simple_testing(Component-SBend "--file=sben.gmad" ${OVERLAP_CHECK})

simple_testing(Component-SBendPoleFace "--file=sbenPoleFace.gmad" ${OVERLAP_CHECK})

simple_testing(Component-SBendFringefields "--file=sbenFringefields.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Screen "--file=screen.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Sextupole "--file=sext.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Solenoid "--file=sole.gmad" ${OVERLAP_CHECK})

set(TESTING_ARGS --circular)
simple_testing(Component-Teleporter "--file=teleporter.gmad" "")

simple_testing(Component-Tunnel "--file=tunnel.gmad" ${OVERLAP_CHECK})

simple_testing(Component-VKick "--file=vkick.gmad" ${OVERLAP_CHECK})

if(USE_AWAKE)
  simple_testing(Component-AwakeSpectrometer "--file=awakespectrometer.gmad" ${OVERLAP_CHECK})
endif()
