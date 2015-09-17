# Copy example directory over for testing
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# test binary without input file
add_test(gmad ${gmadBinary})
set_tests_properties(gmad PROPERTIES WILL_FAIL 1)

# try to overwrite a reserved variable
add_test(gmad-reservedvar ${gmadBinary} reservedvariable.gmad)
set_tests_properties(gmad-reservedvar PROPERTIES WILL_FAIL 1)

# try to use a reserved function
add_test(gmad-reservedfunction ${gmadBinary} reservedfunction.gmad)
set_tests_properties(gmad-reservedfunction PROPERTIES WILL_FAIL 1)
