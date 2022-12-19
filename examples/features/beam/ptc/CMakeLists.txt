execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

simple_testing(bunch-ptc                      "--file=ptc.gmad"                     "")
simple_testing(bunch-ptc-loop-file            "--file=ptc-loop.gmad"                "")


simple_fail(bunch-ptc-loop-file-bad        "--file=ptc-loop-fail.gmad"           "")