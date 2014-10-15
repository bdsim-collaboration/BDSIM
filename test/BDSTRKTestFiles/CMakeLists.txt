execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
 
simple_testing(1_Aperture "--file=1_ApertureTest.gmad" "")
simple_testing(2_TrackerTest "--file=2_TrackerTest.gmad" "")
simple_testing(3_TrackerTestAllElements "--file=3_TrackerTestAllElements.gmad" "")
