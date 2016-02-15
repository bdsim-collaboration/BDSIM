execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

simple_testing(Component-CSampler "--file=csamplerTest.gmad" ${OVERLAP_CHECK})
 
simple_testing(Component-Decapole "--file=decaTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Degrader "--file=degraderTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Drift "--file=driftTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Dump "--file=dumpTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-ECollimator "--file=ecolTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-GDML "--file=gdmlTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-HKick "--file=hkickTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Laserwire "--file=laserTest.gmad" ${OVERLAP_CHECK})
 
simple_testing(Component-Multipole "--file=multTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-MuonSpoiler "--file=muspTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Octupole "--file=octTest.gmad" ${OVERLAP_CHECK})
 
simple_testing(Component-Quadrupole "--file=quadTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-QuadrupoleOffset "--file=quadOffsetTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-RBend "--file=rbenTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-RCollimator "--file=rcolTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-RF_LONG "--file=rfTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Sampler "--file=samplerTest.gmad" ${OVERLAP_CHECK})
 
simple_testing(Component-SBend "--file=sbenTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Sextupole "--file=sextTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Solenoid "--file=soleTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-Tunnel "--file=tunnelTest.gmad" ${OVERLAP_CHECK})

simple_testing(Component-VKick "--file=vkickTest.gmad" ${OVERLAP_CHECK})
