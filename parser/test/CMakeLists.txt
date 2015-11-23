# Copy example directory over for testing
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

###################
### Test Errors ###
###################

# test binary without input file
add_test(gmad ${gmadBinary})
set_tests_properties(gmad PROPERTIES WILL_FAIL 1)

# try to overwrite a reserved variable
add_test(gmad-reservedVar ${gmadBinary} reservedvariable.gmad)
set_tests_properties(gmad-reservedVar PROPERTIES WILL_FAIL 1)

# try to use a reserved function
add_test(gmad-reservedFunction ${gmadBinary} reservedfunction.gmad)
set_tests_properties(gmad-reservedFunction PROPERTIES WILL_FAIL 1)

# options with no semicolon
add_test(gmad-nosemicolon ${gmadBinary} nosemicolon.gmad)
set_tests_properties(gmad-nosemicolon PROPERTIES WILL_FAIL 1)

# undefined element
add_test(gmad-undefinedElement ${gmadBinary} undefinedelement.gmad)
set_tests_properties(gmad-undefinedElement PROPERTIES WILL_FAIL 1)

# duplicate name
add_test(gmad-duplicateName ${gmadBinary} duplicateName.gmad)
set_tests_properties(gmad-duplicateName PROPERTIES WILL_FAIL 1)

# add sampler to undefined element
add_test(gmad-addSamplerUndefinedElement ${gmadBinary} addSamplerUndefinedElement.gmad)
set_tests_properties(gmad-addSamplerUndefinedElement PROPERTIES WILL_FAIL 1)

# element with no options
add_test(gmad-elementnoargs ${gmadBinary} elementnoargs.gmad)
set_tests_properties(gmad-elementnoargs PROPERTIES WILL_FAIL 1)

# try to overwrite an element
add_test(gmad-overwriteElement ${gmadBinary} overwriteElement.gmad)
set_tests_properties(gmad-overwriteElement PROPERTIES WILL_FAIL 1)

# try to overwrite a variable
add_test(gmad-overwriteVariable ${gmadBinary} overwriteVariable.gmad)
set_tests_properties(gmad-overwriteVariable PROPERTIES PASS_REGULAR_EXPRESSION WARNING)

# try to use an undeclared variable
add_test(gmad-undeclaredVariable ${gmadBinary} undeclaredVariable.gmad)
set_tests_properties(gmad-undeclaredVariable PROPERTIES WILL_FAIL 1)

##########################
### Test Functionality ###
##########################

# test functions
add_test(gmad-testFunctions ${gmadBinary} testfunctions.gmad)
set_tests_properties(gmad-testFunctions PROPERTIES PASS_REGULAR_EXPRESSION 3.14159265)

# test stop command
add_test(gmad-stop ${gmadBinary} stop.gmad)

# test access attribute
add_test(gmad-testAccessAttribute ${gmadBinary} accessattribute.gmad)
set_tests_properties(gmad-testAccessAttribute PROPERTIES PASS_REGULAR_EXPRESSION 0.1)

# test non-existing attribute
add_test(gmad-testMissingAccessAttribute ${gmadBinary} accessMissingAttribute.gmad)
set_tests_properties(gmad-testMissingAccessAttribute PROPERTIES WILL_FAIL 1)

# test access element
add_test(gmad-testAccessElement ${gmadBinary} accesselement.gmad)
set_tests_properties(gmad-testAccessElement PROPERTIES PASS_REGULAR_EXPRESSION Sampler_d)

# test access element
add_test(gmad-testAccessElementOutsiderange ${gmadBinary} accesselementoutsiderange.gmad)
set_tests_properties(gmad-testAccessElementOutsiderange PROPERTIES WILL_FAIL 1)

# test add sampler
add_test(gmad-testAddSampler ${gmadBinary} addsampler.gmad)
set_tests_properties(gmad-testAddSampler PROPERTIES PASS_REGULAR_EXPRESSION Sampler_d)

# test add all samplers
add_test(gmad-testAddAllSamplers ${gmadBinary} addallsamplers.gmad)
set_tests_properties(gmad-testAddAllSamplers PROPERTIES PASS_REGULAR_EXPRESSION Sampler_d2)

# test inheritance declaration
add_test(gmad-inherit ${gmadBinary} inherit.gmad)
set_tests_properties(gmad-inherit PROPERTIES PASS_REGULAR_EXPRESSION 0.05)

# test extension
add_test(gmad-extend ${gmadBinary} extend.gmad)
set_tests_properties(gmad-extend PROPERTIES PASS_REGULAR_EXPRESSION 0.05)

# test arrays
add_test(gmad-array ${gmadBinary} array.gmad)
set_tests_properties(gmad-array PROPERTIES PASS_REGULAR_EXPRESSION 32)
add_test(gmad-arraystring ${gmadBinary} arraystring.gmad)
