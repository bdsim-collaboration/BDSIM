# rebdsim ctests

# run an analysis - 'rebdsim' gets expanded by cmake to full path of executable target
macro(_rebdsimAnalTest testName configFile)
  add_test(NAME ${testName} COMMAND rebdsim ${configFile})
endmacro()

macro(rebdsim_test_manual testName configFile inputFile outputFile)
  add_test(NAME ${testName} COMMAND rebdsim ${configFile} ${inputFile} ${outputFile})
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
  # Current usage: dependent on ${testName}_LONG
  set_tests_properties(${testName}_ANALYSIS PROPERTIES DEPENDS ${testName}_LONG)
  set_tests_properties(${testName}_COMPARE  PROPERTIES DEPENDS ${testName}_ANALYSIS)
  set_tests_properties(${testName}_ANALYSIS PROPERTIES LABELS LONG)
  set_tests_properties(${testName}_COMPARE PROPERTIES LABELS LONG)
endmacro()

macro(rebdsim_optics_test testName inputFile outputFileName)
  add_test(NAME ${testName} COMMAND rebdsimOptics ${inputFile} ${outputFile})
endmacro()