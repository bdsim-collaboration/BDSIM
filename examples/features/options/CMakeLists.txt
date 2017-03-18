simple_testing(1_storeTrajectories "--file=1_storeTrajectories.gmad" "")

## Some tests that check wrong input options will fail
simple_testing(option_smallLengthSafety "--file=smallLengthSafety.gmad" "")
set_tests_properties(option_smallLengthSafety PROPERTIES WILL_FAIL 1)

simple_testing(option_smallOuterDiameter "--file=smallOuterDiameter.gmad" "")
set_tests_properties(option_smallOuterDiameter PROPERTIES WILL_FAIL 1)

simple_testing(option_zeroBeamEnergy "--file=zeroBeamEnergy.gmad" "")
set_tests_properties(option_zeroBeamEnergy PROPERTIES WILL_FAIL 1)

simple_testing(option_ignoreLocalAperture "--file=override_aperture.gmad" "")