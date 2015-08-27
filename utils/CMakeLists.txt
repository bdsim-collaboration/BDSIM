# Copy files to build directory
#execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

# add robdsim when submodule updated and has test macros
if(USE_ROOT AND EXISTS
${CMAKE_CURRENT_SOURCE_DIR}/robdsim/CMakeLists.txt AND EXISTS
${CMAKE_CURRENT_SOURCE_DIR}/robdsim/cmake/robdsim_test_macros.cmake)
   set(USE_ROBDSIM TRUE CACHE BOOL "Use submodule robdsim")
   add_subdirectory(robdsim)
   # hack to move dictionary files from robdsim to right place
   file(GLOB robdsimdictionaries ${CMAKE_BINARY_DIR}/*Dict.C ${CMAKE_BINARY_DIR}/*Dict.h)
   execute_process(COMMAND mv ${robdsimdictionaries} ${CMAKE_BINARY_DIR}/utils/robdsim)
else()
   set(USE_ROBDSIM FALSE CACHE BOOL "Use submodule robdsim")
endif()
