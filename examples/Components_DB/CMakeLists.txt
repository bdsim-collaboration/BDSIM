execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set(TESTING_ARGS "--batch")
simple_testing(Component-Drift "--file=driftTest.gmad" "")

set(TESTING_ARGS "--batch")
 simple_testing(Component-Quadrupole "--file=quadTest.gmad" "")

set(TESTING_ARGS "--batch")
 simple_testing(Component-SBend "--file=sbenTest.gmad" "")

set(TESTING_ARGS "--batch")
 simple_testing(Component-Solenoid "--file=soleTest.gmad" "")
