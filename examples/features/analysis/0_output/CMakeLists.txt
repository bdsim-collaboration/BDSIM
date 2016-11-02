configure_file(${CMAKE_CURRENT_SOURCE_DIR}/loadLibraries.C.in ${CMAKE_CURRENT_BINARY_DIR}/loadLibraries.C)

add_test(NAME "root-library-load" COMMAND ${ROOT_EXECUTABLE} -q -l loadLibraries.C)
set_tests_properties("root-library-load" PROPERTIES FAIL_REGULAR_EXPRESSION "Error")