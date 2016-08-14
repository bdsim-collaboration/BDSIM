simple_testing(IO_ROOTEVENT1 "--file=fodoRing.gmad --output=rootevent --circular" "")
simple_testing(IO_ROOTEVENT2 "--file=fodoRing.gmad --output=rootevent --circular" "")

#rebdsim merge test
add_test(IO_ROOTEVENT-rebdsim-merge ${CMAKE_BINARY_DIR}/analysis/rebdsim "analysisConfigMerge.txt")
set_tests_properties(IO_ROOTEVENT-rebdsim-merge PROPERTIES DEPENDS IO_ROOTEVENT2)



# rebdsim test
add_test(IO_ROOTEVENT-rebdsim ${CMAKE_BINARY_DIR}/analysis/rebdsim "analysisConfig.txt")
set_tests_properties(IO_ROOTEVENT-rebdsim PROPERTIES DEPENDS IO_ROOTEVENT1)


# seed state (uses seedstate from previous test)
simple_testing(rooteventRecreate "--file=io_recreate.gmad" "")
set_tests_properties(rooteventRecreate PROPERTIES DEPENDS IO_ROOTEVENT1)

# seed state (uses seedstate from previous test)
set(TESTING_ARGS --ngenerate=14)
simple_testing(rooteventRecreateRepeat "--file=io_recreate.gmad" "")
set_tests_properties(rooteventRecreate PROPERTIES DEPENDS IO_ROOTEVENT1)