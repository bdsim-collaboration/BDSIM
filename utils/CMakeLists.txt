# add robdsim when submodule updated and has test macros
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/robdsim/CMakeLists.txt AND
    EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/robdsim/cmake/robdsim_test_macros.cmake)
  set(USE_ROBDSIM TRUE CACHE BOOL "Use submodule robdsim")
  if (USE_ROBDSIM)
    add_subdirectory(robdsim)
  endif()
else()
   set(USE_ROBDSIM FALSE CACHE BOOL "Use submodule robdsim")
endif()
