# rebdsim ctests

# run an analysis
macro(_rebdsimAnalTest testName configFile)
  add_test(NAME ${testName} COMMAND ${rebdsimBinary} ${configFile})
endmacro()

# run a comparison job
macro(_comparatorTest testName refFile testFile)
  add_test(NAME ${testName} COMMAND ${comparatorBinary} ${refFile} ${testFile})
  set_tests_properties(${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "TEST_FAILED")
endmacro()

# run analysis then a comparison
macro(rebdsim_test testName)
  _rebdsimAnalTest(${testName}_ANALYSIS ${testName}.txt)
  _comparatorTest(${testName}_COMPARE ${testName}_anal.root ${testName}_ref.root)
  set_tests_properties(${testName}_ANALYSIS PROPERTIES DEPENDS ${testName})
  set_tests_properties(${testName}_COMPARE  PROPERTIES DEPENDS ${testName}_ANALYSIS)
endmacro()

# same as above but with LONG label on all subtests
macro(rebdsim_test_long testName)
  _rebdsimAnalTest(${testName}_ANALYSIS ${testName}.txt)
  _comparatorTest(${testName}_COMPARE ${testName}_anal.root ${testName}_ref.root)
  set_tests_properties(${testName}_ANALYSIS PROPERTIES DEPENDS ${testName})
  set_tests_properties(${testName}_COMPARE  PROPERTIES DEPENDS ${testName}_ANALYSIS)
  set_tests_properties(${testName}_ANALYSIS PROPERTIES LABELS LONG)
  set_tests_properties(${testName}_COMPARE PROPERTIES LABELS LONG)
endmacro()