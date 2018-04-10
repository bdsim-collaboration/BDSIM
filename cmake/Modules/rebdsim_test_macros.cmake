# rebdsim ctests

# run an analysis - 'rebdsim' gets expanded by cmake to full path of executable target
macro(rebdsim_test testName configFile)
  add_test(NAME ${testName} COMMAND rebdsimexec ${configFile})
endmacro()

# fully specify the input and output files
macro(rebdsim_test_manual testName configFile inputFile outputFile)
  add_test(NAME ${testName} COMMAND rebdsimexec ${configFile} ${inputFile} ${outputFile})
endmacro()

# run analysis then a comparison
macro(rebdsim_comparator_test testName)
  rebdsim_test(${testName}-analysis ${testName}.txt)
  comparator_test(${testName}-comparison ${testName}_anal.root ${testName}_ref.root)
  set_tests_properties(${testName}-analysis PROPERTIES DEPENDS ${testName})
  set_tests_properties(${testName}-comparison  PROPERTIES DEPENDS ${testName}-analysis)
endmacro()

# same as above but with LONG label on all subtests
macro(rebdsim_test_long testName)
  rebdsim_test(${testName}-analysis ${testName}.txt)
  comparator_test(${testName}-comparison ${testName}_anal.root ${testName}_ref.root)
  # Current usage: dependent on ${testName}_LONG
  set_tests_properties(${testName}-analysis PROPERTIES DEPENDS ${testName}_LONG)
  set_tests_properties(${testName}-comparison  PROPERTIES DEPENDS ${testName}-analysis)
  set_tests_properties(${testName}-analysis PROPERTIES LABELS LONG)
  set_tests_properties(${testName}-comparison PROPERTIES LABELS LONG)
endmacro()

macro(rebdsim_optics_test testName inputFile outputFile)
  add_test(NAME ${testName} COMMAND rebdsimOptics ${inputFile} ${outputFile})
endmacro()

# rebdsimHistoMerge
macro(rebdsim_histomerge_test testName inputFile outputFile)
  add_test(NAME ${testName} COMMAND rebdsimHistoMergeExec ${inputFile} ${outputFile})
endmacro()

# rebdsimCombine - can only take two files for this test
macro(rebdsim_combine_test testName outputFile inputFile1 inputFile2)
  add_test(NAME ${testName} COMMAND rebdsimCombineExec ${outputFile} ${inputFile1} ${inputFile2})
endmacro()

# rebdsimOrbit - pull out one orbit
macro(rebdsim_orbit_test testName inputFile outputFile index)
  add_test(NAME ${testName} COMMAND rebdsimOrbit ${inputFile} ${outputFile} ${index})
endmacro()