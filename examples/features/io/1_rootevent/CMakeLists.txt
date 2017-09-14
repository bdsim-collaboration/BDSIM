simple_testing(IO_ROOTEVENT1 "--file=fodoRing.gmad --output=rootevent --circular" "")
simple_testing(IO_ROOTEVENT2 "--file=fodoRing.gmad --output=rootevent --circular" "")

# rebdsim merge test
add_test(IO_ROOTEVENT-rebdsim-merge ${CMAKE_BINARY_DIR}/analysis/rebdsim "analysisConfigMerge.txt")

# rebdsim test
add_test(IO_ROOTEVENT-rebdsim ${CMAKE_BINARY_DIR}/analysis/rebdsim "analysisConfig.txt")
set_tests_properties(IO_ROOTEVENT-rebdsim PROPERTIES DEPENDS IO_ROOTEVENT1)

# generate simple losses / transport
simple_testing(IO_ROOTEVENT_IO "--file=io.gmad --outfile=io_test" "")

# test histogram making
rebdsim_test_manual(rebdsim-selector "analysisConfig2.txt" "io_test.root" "io_test_ana.root")
set_tests_properties(rebdsim-selector PROPERTIES DEPENDS IO_ROOTEVENT_IO)

# seed state (uses seedstate from previous test)
simple_testing(IO_ROOTEVENT_IO_Recreate "--file=io_recreate.gmad" "")
set_tests_properties(IO_ROOTEVENT_IO_Recreate PROPERTIES DEPENDS IO_ROOTEVENT_IO)

# write the seed out per event
simple_testing(IO_SeedState "--file=io.gmad --writeSeedState" "")

# use ascii seed state
simple_testing(IO_Load_ASCII_SeedState "--file=io.gmad --seedStateFileName=output.seedstate.txt --ngenerate=1" "")
set_tests_properties(IO_Load_ASCII_SeedState PROPERTIES DEPENDS IO_SeedState)
