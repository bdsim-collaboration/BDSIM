# Copy example directory over for testing
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

###################
### Test Errors ###
###################

# test binary without input file
add_test(gmad-noinputfile ${gmadBinary})
set_tests_properties(gmad-noinputfile PROPERTIES WILL_FAIL 1)

# test binary without two input files
add_test(gmad-twoinputfiles ${gmadBinary} a;b)
set_tests_properties(gmad-twoinputfiles PROPERTIES WILL_FAIL 1)

# test binary with non existent input file
add_test(gmad-nonexistent ${gmadBinary} nonexistent.gmad)
set_tests_properties(gmad-nonexistent PROPERTIES WILL_FAIL 1)

# try to overwrite a reserved variable
add_test(gmad-reservedVar ${gmadBinary} reservedvariable.gmad)
set_tests_properties(gmad-reservedVar PROPERTIES WILL_FAIL 1)

# try to use a reserved function
add_test(gmad-reservedFunction ${gmadBinary} reservedfunction.gmad)
set_tests_properties(gmad-reservedFunction PROPERTIES WILL_FAIL 1)

# try to define a reserved name
add_test(gmad-defineReservedName ${gmadBinary} definereservedname.gmad)
set_tests_properties(gmad-defineReservedName PROPERTIES PASS_REGULAR_EXPRESSION "already defined")

# try to define an already defined numeric variable
add_test(gmad-defineReservedVariable ${gmadBinary} definereservedvariable.gmad)
set_tests_properties(gmad-defineReservedVariable PROPERTIES PASS_REGULAR_EXPRESSION "already defined")

# try to define an already defined string variable
add_test(gmad-defineReservedStringVariable ${gmadBinary} definereservedstringvariable.gmad)
set_tests_properties(gmad-defineReservedStringVariable PROPERTIES PASS_REGULAR_EXPRESSION "already defined")

# try to define a reserved function
add_test(gmad-defineReservedFunction ${gmadBinary} definereservedfunction.gmad)
set_tests_properties(gmad-defineReservedFunction PROPERTIES PASS_REGULAR_EXPRESSION "already defined")

# options with no semicolon
add_test(gmad-nosemicolon ${gmadBinary} nosemicolon.gmad)
set_tests_properties(gmad-nosemicolon PROPERTIES WILL_FAIL 1)

# addition of string and number
add_test(gmad-stringnumber ${gmadBinary} stringnumber.gmad)
set_tests_properties(gmad-stringnumber PROPERTIES WILL_FAIL 1)

# string with comma
add_test(gmad-stringcomma ${gmadBinary} stringcomma.gmad)
set_tests_properties(gmad-stringcomma PROPERTIES WILL_FAIL 1)

# string with newline
add_test(gmad-stringnewline ${gmadBinary} stringnewline.gmad)
set_tests_properties(gmad-stringnewline PROPERTIES WILL_FAIL 1)

# undefined element
add_test(gmad-undefinedElement ${gmadBinary} undefinedelement.gmad)
set_tests_properties(gmad-undefinedElement PROPERTIES WILL_FAIL 1)

# use undefined option
add_test(gmad-undefinedOption ${gmadBinary} undefinedOption.gmad)
set_tests_properties(gmad-undefinedOption PROPERTIES WILL_FAIL 1)

# use undefined element with undefined option (issue #161)
add_test(gmad-undefinedElementAndOption ${gmadBinary} undefinedElementAndOption.gmad)
set_tests_properties(gmad-undefinedElementAndOption PROPERTIES PASS_REGULAR_EXPRESSION "not been defined")

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

# try to overwrite a string variable
add_test(gmad-overwriteVariableString ${gmadBinary} overwriteVariableString.gmad)
set_tests_properties(gmad-overwriteVariableString PROPERTIES PASS_REGULAR_EXPRESSION WARNING)

# try to use an undeclared variable
add_test(gmad-undeclaredVariable ${gmadBinary} undeclaredVariable.gmad)
set_tests_properties(gmad-undeclaredVariable PROPERTIES PASS_REGULAR_EXPRESSION "is not declared")

# include non existent file
add_test(gmad-includenonexistent ${gmadBinary} includenonexistent.gmad)
set_tests_properties(gmad-includenonexistent PROPERTIES WILL_FAIL 1)

# recursive inclusion
add_test(gmad-recursiveinclusion ${gmadBinary} recursiveinclusion.gmad)
set_tests_properties(gmad-recursiveinclusion PROPERTIES WILL_FAIL 1)

##########################
### Test Functionality ###
##########################

# test functions
add_test(gmad-testFunctions ${gmadBinary} testfunctions.gmad)
set_tests_properties(gmad-testFunctions PROPERTIES PASS_REGULAR_EXPRESSION 3.14159265)

# test stop command
add_test(gmad-stop ${gmadBinary} stop.gmad)

# test if construct
add_test(gmad-if ${gmadBinary} if.gmad)
set_tests_properties(gmad-if PROPERTIES PASS_REGULAR_EXPRESSION 2)

# test if/else construct
add_test(gmad-ifelse ${gmadBinary} ifelse.gmad)
set_tests_properties(gmad-ifelse PROPERTIES PASS_REGULAR_EXPRESSION 2)

# test special printing
add_test(gmad-print ${gmadBinary} print.gmad)

# test access attribute
add_test(gmad-testAccessAttribute ${gmadBinary} accessattribute.gmad)
set_tests_properties(gmad-testAccessAttribute PROPERTIES PASS_REGULAR_EXPRESSION 0.1)

# test non-existing attribute
add_test(gmad-testMissingAccessAttribute ${gmadBinary} accessMissingAttribute.gmad)
set_tests_properties(gmad-testMissingAccessAttribute PROPERTIES WILL_FAIL 1)

# test access element
add_test(gmad-testAccessElement ${gmadBinary} accesselement.gmad)
set_tests_properties(gmad-testAccessElement PROPERTIES PASS_REGULAR_EXPRESSION plane)

# test access element outside range
add_test(gmad-testAccessElementOutsiderange ${gmadBinary} accesselementoutsiderange.gmad)
set_tests_properties(gmad-testAccessElementOutsiderange PROPERTIES WILL_FAIL 1)

# test access option
add_test(gmad-testAccessOption ${gmadBinary} accessoption.gmad)
set_tests_properties(gmad-testAccessOption PROPERTIES PASS_REGULAR_EXPRESSION 123)

# test add sampler
add_test(gmad-testAddSampler ${gmadBinary} addsampler.gmad)
set_tests_properties(gmad-testAddSampler PROPERTIES PASS_REGULAR_EXPRESSION plane)

# test add all samplers
add_test(gmad-testAddAllSamplers ${gmadBinary} addallsamplers.gmad)
set_tests_properties(gmad-testAddAllSamplers PROPERTIES PASS_REGULAR_EXPRESSION plane)

# test add samplers at all quadrupoles
add_test(gmad-testAddQuadSamplers ${gmadBinary} addquadsamplers.gmad)
set_tests_properties(gmad-testAddQuadSamplers PROPERTIES PASS_REGULAR_EXPRESSION plane)

# test inheritance declaration
add_test(gmad-inherit ${gmadBinary} inherit.gmad)
set_tests_properties(gmad-inherit PROPERTIES PASS_REGULAR_EXPRESSION 0.05)

# test extension
add_test(gmad-extend ${gmadBinary} extend.gmad)
set_tests_properties(gmad-extend PROPERTIES PASS_REGULAR_EXPRESSION 0.05)

# test string (issue #126)
add_test(gmad-string ${gmadBinary} string.gmad)

# test arrays
add_test(gmad-array ${gmadBinary} array.gmad)
set_tests_properties(gmad-array PROPERTIES PASS_REGULAR_EXPRESSION 32)
add_test(gmad-arraystring ${gmadBinary} arraystring.gmad)

# test atom
add_test(gmad-atom ${gmadBinary} atom.gmad)

# test beam
add_test(gmad-beam ${gmadBinary} beam.gmad)

# test material
add_test(gmad-material ${gmadBinary} material.gmad)

# test cavitymodel
add_test(gmad-cavitymodel ${gmadBinary} cavitymodel.gmad)

# test field
add_test(gmad-field ${gmadBinary} field.gmad)

# test query
add_test(gmad-query ${gmadBinary} query.gmad)

# test physics biasing
add_test(gmad-physicsbiasing ${gmadBinary} physicsbiasing.gmad)

# test placement
add_test(gmad-placement ${gmadBinary} placement.gmad)
add_test(gmad-placement-sequence ${gmadBinary} placement_sequence.gmad)

# test regions
add_test(gmad-region ${gmadBinary} region.gmad)

# test tunnel
add_test(gmad-tunnel ${gmadBinary} tunnel.gmad)

