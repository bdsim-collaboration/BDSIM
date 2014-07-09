execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(TESTING_PERM_ARGS --batch)

if (USE_ROOT)
   set(TESTING_PERM_ARGS ${TESTING_PERM_ARGS} --output=root)
endif()

add_subdirectory(BDSBunchTest)

add_executable(BDSBunchTest BDSBunchTest.cc)
set_target_properties(BDSBunchTest PROPERTIES OUTPUT_NAME "BDSBunchTest" VERSION ${BDSIM_VERSION})
target_link_libraries(BDSBunchTest bdsim gmad)

set(executables BDSBunchTest)
