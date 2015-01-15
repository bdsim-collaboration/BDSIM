execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(TESTING_PERM_ARGS --batch)

if (USE_ROOT)
   set(TESTING_PERM_ARGS ${TESTING_PERM_ARGS} --output=root)
endif()

add_subdirectory(Components_DB)
add_subdirectory(ATF2)
add_subdirectory(LHC)
add_subdirectory(simpleMachine)

add_subdirectory(features)
