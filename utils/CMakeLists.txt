# add robdsim when submodule updated and has test macros

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/robdsim/CMakeLists.txt AND
    EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/robdsim/cmake/robdsim_test_macros.cmake)
  set(USE_ROBDSIM FALSE CACHE BOOL "Use submodule robdsim")
  if (USE_ROBDSIM)
    add_subdirectory(robdsim)
  endif()
  message(STATUS "Robdsim disabled by default - please move to rebdsim")
else()
  message(STATUS "Robdsim not found - switching off")
  message(STATUS "To get robdsim do 'git submodule init' and 'git submodule update'")
  # force overwrite
  set(USE_ROBDSIM FALSE CACHE BOOL "Use submodule robdsim" FORCE)
endif()


# turn off robdsim always now as being deprecated
set(USE_ROBDSIM FALSE CACHE BOOL "Use submodule robdsim" FORCE)
