execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})
 
simple_testing(1_Aperture "--file=1_ApertureTest.gmad" "")
simple_testing(2_TrackerTest "--file=2_TrackerTest.gmad" "")
simple_testing(3_TrackerTestAllElements "--file=3_TrackerTestAllElements.gmad" "")

simple_testing(101_ThinDrift "--file=101_ThinDriftTest.gmad" "")
simple_testing(102_ThinDipole "--file=102_ThinDipoleTest.gmad" "")
simple_testing(103_ThinQuadrupole "--file=103_ThinQuadTest.gmad" "")
simple_testing(104_ThinSextupole "--file=104_ThinSextTest.gmad" "")
simple_testing(105_ThinOctupole "--file=105_ThinOctTest.gmad" "")
