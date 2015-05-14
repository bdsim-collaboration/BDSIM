execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(TESTING_ARGS "--batch")
simple_testing(Component-CSampler "--file=csamplerTest.gmad" "")
 
simple_testing(Component-Decapole "--file=decaTest.gmad" "")

simple_testing(Component-Drift "--file=driftTest.gmad" "")

simple_testing(Component-ECollimator "--file=ecolTest.gmad" "")

simple_testing(Component-GDML "--file=gdmlTest.gmad" "")

simple_testing(Component-HKick "--file=hkickTest.gmad" "")

simple_testing(Component-Laserwire "--file=laserTest.gmad" "")
 
simple_testing(Component-Multipole "--file=multTest.gmad" "")

simple_testing(Component-MuonSpoiler "--file=muspTest.gmad" "")

simple_testing(Component-Octupole "--file=octTest.gmad" "")
 
simple_testing(Component-Quadrupole "--file=quadTest.gmad" "")

simple_testing(Component-RBend "--file=rbenTest.gmad" "")

simple_testing(Component-RCollimator "--file=rcolTest.gmad" "")

simple_testing(Component-RF "--file=rfTest.gmad" "")

simple_testing(Component-Sampler "--file=samplerTest.gmad" "")
 
simple_testing(Component-SBend "--file=sbenTest.gmad" "")

simple_testing(Component-Sextupole "--file=sextTest.gmad" "")

simple_testing(Component-Solenoid "--file=soleTest.gmad" "")

simple_testing(Component-Tunnel "--file=tunnelTest.gmad" "")

simple_testing(Component-VKick "--file=vkickTest.gmad" "")
