simple_testing(IO_ROOTEVENT1 "--file=fodoRing.gmad --output=rootevent --circular" "")
simple_testing(IO_ROOTEVENT2 "--file=fodoRing.gmad --output=rootevent --circular" "")

# rebdsim merge test
add_test(IO_ROOTEVENT-rebdsim-merge ${CMAKE_BINARY_DIR}/analysis/rebdsim "analysisConfigMerge.txt")
set_tests_properties(IO_ROOTEVENT-rebdsim-merge PROPERTIES DEPENDS IO_ROOTEVENT2)
set_tests_properties(IO_ROOTEVENT-rebdsim-merge PROPERTIES DEPENDS IO_ROOTEVENT1)

# rebdsim test
add_test(IO_ROOTEVENT-rebdsim ${CMAKE_BINARY_DIR}/analysis/rebdsim "analysisConfig.txt")
set_tests_properties(IO_ROOTEVENT-rebdsim PROPERTIES DEPENDS IO_ROOTEVENT1)

# generate simple losses / transport
simple_testing(IO_ROOTEVENT_IO "--file=io.gmad" "")

# seed state (uses seedstate from previous test)
simple_testing(IO_ROOTEVENT_IO_Recreate "--file=io_recreate.gmad" "")
set_tests_properties(IO_ROOTEVENT_IO_Recreate PROPERTIES DEPENDS IO_ROOTEVENT)

# write the seed out per event
simple_testing(IO_SeedState "--file=io.gmad --writeseedstate" "")
