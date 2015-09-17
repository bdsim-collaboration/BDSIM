# Copy example directory over for testing
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

###################
### Test Errors ###
###################

# test binary without input file
add_test(gmad ${gmadBinary})
set_tests_properties(gmad PROPERTIES WILL_FAIL 1)

# try to overwrite a reserved variable
add_test(gmad-reservedvar ${gmadBinary} reservedvariable.gmad)
set_tests_properties(gmad-reservedvar PROPERTIES WILL_FAIL 1)

# try to use a reserved function
add_test(gmad-reservedfunction ${gmadBinary} reservedfunction.gmad)
set_tests_properties(gmad-reservedfunction PROPERTIES WILL_FAIL 1)

# options with no semicolon
add_test(gmad-nosemicolon ${gmadBinary} nosemicolon.gmad)
set_tests_properties(gmad-nosemicolon PROPERTIES WILL_FAIL 1)

# undefined element
add_test(gmad-undefinedelement ${gmadBinary} undefinedelement.gmad)
set_tests_properties(gmad-undefinedelement PROPERTIES WILL_FAIL 1)

# element with no options
add_test(gmad-elementnoargs ${gmadBinary} elementnoargs.gmad)
set_tests_properties(gmad-elementnoargs PROPERTIES WILL_FAIL 1)

# try to overwrite a variable
add_test(gmad-overwritevariable ${gmadBinary} overwritevariable.gmad)
set_tests_properties(gmad-overwritevariable PROPERTIES WILL_FAIL 1)

##########################
### Test Functionality ###
##########################

# test functions
add_test(gmad-testfunctions ${gmadBinary} testfunctions.gmad)
set_tests_properties(gmad-testfunctions PROPERTIES PASS_REGULAR_EXPRESSION 3.14159265)

# test access attribute
add_test(gmad-testaccessattribute ${gmadBinary} accessattribute.gmad)
set_tests_properties(gmad-testaccessattribute PROPERTIES PASS_REGULAR_EXPRESSION 0.5)

# test access element
add_test(gmad-testaccesselement ${gmadBinary} accesselement.gmad)

# test access element
add_test(gmad-testaccesselementoutsiderange ${gmadBinary} accesselementoutsiderange.gmad)
set_tests_properties(gmad-testaccesselementoutsiderange PROPERTIES WILL_FAIL 1)
