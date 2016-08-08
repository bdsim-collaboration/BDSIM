# certain test options that can be added
# For overlap checking (needs option, checkOverlaps=1)
set(OVERLAP_CHECK "Overlap is detected")

# always test in batch mode
set(TESTING_PERM_ARGS --batch)

# ROOT output format by default
set(TESTING_PERM_ARGS ${TESTING_PERM_ARGS} --output=rootevent)

add_subdirectory(ATF2)
add_subdirectory(LHC)
add_subdirectory(simpleMachine)
add_subdirectory(simpleCollimation)
add_subdirectory(features)
