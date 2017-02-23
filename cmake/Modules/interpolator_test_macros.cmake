# interpolator ctests

# run a comparison job
macro(interpolator_test testName inputFile)
  add_test(NAME ${testName} COMMAND interpolatorexec --file=${inputFile})
endmacro()