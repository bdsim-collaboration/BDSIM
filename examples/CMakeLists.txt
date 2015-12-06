# Copy example directory over for testing
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# certain test options that can be added
# For overlap checking (needs option, checkOverlaps=1)
set(OVERLAP_CHECK "Overlap is detected")

# always test in batch mode
set(TESTING_PERM_ARGS --batch)

# ROOT output format by default
set(TESTING_PERM_ARGS ${TESTING_PERM_ARGS} --output=root)

add_subdirectory(ATF2)
add_subdirectory(LHC)
add_subdirectory(simpleMachine)
add_subdirectory(features)
