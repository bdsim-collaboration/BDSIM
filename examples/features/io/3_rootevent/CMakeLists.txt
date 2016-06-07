simple_testing(IO_ROOTEVENT "--file=fodoRing.gmad --output=rootevent --circular" "")

# rebdsim test
add_test(IO_ROOTEVENT-rebdsim ${CMAKE_BINARY_DIR}/analysis/rebdsim "analysisConfig.txt")
set_tests_properties(IO_ROOTEVENT-rebdsim PROPERTIES DEPENDS IO_ROOTEVENT)
