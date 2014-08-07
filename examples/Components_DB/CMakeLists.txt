execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(TESTING_ARGS "--batch")
simple_testing(Component-CSampler "--file=csamplerTest.gmad" "")
 
simple_testing(Component-Decapole "--file=decaTest.gmad" "")

simple_testing(Component-Drift "--file=driftTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-ECollimator "--file=ecolTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-GDML "--file=gdmlTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-HKick "--file=hkickTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-Laserwire "--file=laserTest.gmad" "")
 
set(TESTING_ARGS "--batch")
simple_testing(Component-Multipole "--file=multTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-MuonSpoiler "--file=muspTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-Octupole "--file=octTest.gmad" "")
 
set(TESTING_ARGS "--batch")
simple_testing(Component-PCLDrift "--file=pcldriftTest.gmad" "")
 
set(TESTING_ARGS "--batch")
simple_testing(Component-Quadrupole "--file=quadTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-RBend "--file=rbenTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-RCollimator "--file=rcolTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-RF "--file=rfTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-Sampler "--file=samplerTest.gmad" "")
 
set(TESTING_ARGS "--batch")
simple_testing(Component-SBend "--file=sbenTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-Sextupole "--file=sextTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-Solenoid "--file=soleTest.gmad" "")

set(TESTING_ARGS "--batch")
simple_testing(Component-VKick "--file=vkickTest.gmad" "")
