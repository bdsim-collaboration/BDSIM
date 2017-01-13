# Copy files to build directory
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

bdsim_install_vis(dawnfile.mac gui.mac icons.mac run.png vis.mac)
