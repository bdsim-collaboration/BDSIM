# Copy example directory over for testing
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# certain test options that can be added
# For overlap checking (needs option, checkOverlaps=1)
set(OVERLAP_CHECK "Overlap is detected")

# always test in batch mode (ASCII output is too large for some tests)
set(TESTING_PERM_ARGS --batch --output=none)

# if ROOT output ROOT format
if (USE_ROOT)
   set(TESTING_PERM_ARGS ${TESTING_PERM_ARGS} --output=root)
endif()

add_subdirectory(ATF2)
add_subdirectory(LHC)
add_subdirectory(simpleMachine)
add_subdirectory(features)
