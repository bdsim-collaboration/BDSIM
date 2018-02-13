simple_testing(io-store-trajectories "--file=1_storeTrajectories.gmad" "")

## Some tests that check wrong input options will fail
simple_testing(option-length-safety-too-small "--file=smallLengthSafety.gmad" "")
set_tests_properties(option-length-safety-too-small PROPERTIES WILL_FAIL 1)

simple_testing(option-outer-diameter-too-small "--file=smallOuterDiameter.gmad" "")
set_tests_properties(option-outer-diameter-too-small PROPERTIES WILL_FAIL 1)

simple_testing(option-zero-beam-energy "--file=zeroBeamEnergy.gmad" "")
set_tests_properties(option-zero-beam-energy PROPERTIES WILL_FAIL 1)

simple_testing(option-ignore-local-aperture "--file=overrideAperture.gmad" "")