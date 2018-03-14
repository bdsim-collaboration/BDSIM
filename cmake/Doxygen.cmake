find_package(Doxygen)
if(DOXYGEN_FOUND)
  include(ProcessorCount)
  ProcessorCount(N)
  set(CMAKE_DOT_NUM_THREADS ${N})
  if ($N GREATER 8)
    set(CMAKE_DOT_NUM_THREADS 8)
  endif()
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/doxygen.cnf.in ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cnf @ONLY)
  add_custom_target(doc
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/doxygen.cnf
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM
  )
  # add 'make doxygen' to ctest
  add_test(doxygen make doc)
  set_tests_properties(doxygen PROPERTIES LABELS LONG)
endif(DOXYGEN_FOUND)
