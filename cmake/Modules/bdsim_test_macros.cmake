# Note:
# For all tests, you can add additional arguments to the command using the 
# variable TESTING_ARGS. This variable will be unset after each test is ran.

# simple_testing:
#  Tests a script and looks for the expression in standard out. Expression
#  can be set to "" if you do not want to require anything in the output.
#
# compare_test: 
#  Run a script and afterwards require that file1 and file2 have equal output.
#  The file comparison is done using the python script cmake/compare_files.py
#  
# unit_test:
#  Run a binary which does unit testing. Only require that the binary returns 0
#  to define success or failure.

macro(_run_test test_name args)
    # If loop can be removed when we no longer support cmake 2.6...
    if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} VERSION_GREATER 2.7)
       add_test(NAME ${test_name} COMMAND ${binary} ${args} ${TESTING_PERM_ARGS} ${TESTING_ARGS})
    else()
       add_test(${test_name} ${binary} ${args} ${TESTING_PERM_ARGS} ${TESTING_ARGS})
    endif()
    unset(TESTING_ARGS)
endmacro()

# A simple macro which runs a script and looks for some defined
# string in stdout and fails if found:
macro(simple_testing test_name script expression)
    _run_test(${test_name} "${script}")
    if(NOT "${expression}" STREQUAL "")
        set_tests_properties(${test_name} PROPERTIES FAIL_REGULAR_EXPRESSION "${expression}")
    endif()
endmacro()

macro(compare_test test_name script file1 file2)
    simple_testing("${test_name}" "${script}" "")
    find_package(PythonInterp REQUIRED)
    add_test(${test_name}_CheckOutput ${PYTHON_EXECUTABLE}
       ${CMAKE_SOURCE_DIR}/cmake/compare_files.py
       ${file1} ${file2})
    set_tests_properties(${test_name}_CheckOutput PROPERTIES DEPENDS ${test_name})
endmacro()

macro(unit_test test_name binary)
  # If loop can be removed when we no longer support cmake 2.6...
  if(${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} VERSION_GREATER 2.7)
    add_test(NAME ${test_name} COMMAND ${binary})
  else()
    add_test(${test_name} ${binary})
  endif()
endmacro()
