configure_file(${CMAKE_CURRENT_SOURCE_DIR}/loadLibraries.C.in ${CMAKE_CURRENT_BINARY_DIR}/loadLibraries.C)

simple_root_test("root-library-load" loadLibraries.C "Error")