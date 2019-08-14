# executable options
simple_testing(option-executable-distrFile             "--file=userfile.gmad --distrFile=../beam/userfile/userbeamdata-comment.dat" "")
simple_testing(option-executable-distrFileNLinesSkip   "--file=userfile.gmad --distrFileNLinesSkip=2" "")
simple_testing(option-executable-generatePrimariesOnly "--file=sm.gmad --generatePrimariesOnly" "")
simple_testing(option-executable-help                  "--help"                                 "")
simple_testing(option-executable-materials             "--materials"                            "")
simple_testing(option-executable-printPhysicsProcesses "--file=sm.gmad --printPhysicsProcesses" "")
simple_testing(option-executable-seed                  "--file=sm.gmad --seed=1729"             "")
simple_testing(option-executable-startFromEvent        "--file=sm.gmad --startFromEvent=10"     "")
simple_testing(option-executable-survey                "--file=sm.gmad --survey=survey.dat"     "")
simple_testing(option-executable-verbose               "--file=sm.gmad --verbose"               "")
simple_testing(option-executable-verbose_event         "--file=sm.gmad --verbose_event"         "")
simple_testing(option-executable-verbose_event_num     "--file=sm.gmad --verbose_event_num=0"   "")
simple_testing(option-executable-verbose_step          "--file=sm.gmad --verbose_step"          "")
simple_testing(option-executable-verbose-g4levels "--file=sm.gmad --verbose_G4run=5 --verbose_G4event=5 --verbose_G4tracking=5 --verbose_G4stepping=5" "")

# Test that original teleporter approach does not crash bdsim
simple_testing(option-originalTeleporterMethod "--file=originalTeleporterMethod.gmad --circular" "")

## Some tests that check wrong input options will fail
simple_fail(option-length-safety-too-small         "--file=smallLengthSafety.gmad"  "")
simple_fail(option-outer-diameter-too-small        "--file=smallOuterDiameter.gmad" "")
simple_fail(option-zero-beam-energy                "--file=zeroBeamEnergy.gmad"     "")

# general input options
simple_testing(option-eloss-sensitive-vacuum       "--file=eloss-vacuum.gmad"             "")
simple_testing(option-ignore-local-aperture        "--file=overrideAperture.gmad"         "")
simple_testing(option-ignore-local-magnet-geometry "--file=overrideMagnetGeometry.gmad"   "")
simple_testing(option-noeloss-beampipes            "--file=noeloss-beampipes.gmad"        "")
simple_testing(option-noeloss-outer                "--file=noeloss-outer.gmad"            "")
simple_testing(option-ptc-otm                      "--file=ptcOneTurnMap.gmad --circular" "")
simple_testing(option-verboseEvent                 "--file=verboseEvent.gmad"             "")
simple_testing(option-verboseEvent-primaries       "--file=verboseEvent-primaries.gmad"   "")

# optional depending on build
# output export geometry
if(USE_GDML)
  set(TESTING_ARGS --exportGeometryTo=export.gdml)
  simple_testing(option-executable-exportGeometryTo "--file=sm.gmad" "")
endif()