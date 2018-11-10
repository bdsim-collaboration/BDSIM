cmake_minimum_required (VERSION 3.2)
project (externalcpp)

# point cmake to my own custom installation directory that's not a system dir
# this is where BDSIMConfig.cmake exists
set(bdsim_DIR /Users/nevay/physics/reps/bdsim-interface-install/lib/cmake/bdsim)

# find the package and set up variables
find_package(bdsim REQUIRED)

# define program includes
include_directories(${BDSIM_INCLUDE_DIR}/bdsim)

# make a program and link to Lwsim library
add_executable(customprogram customprogram.cc)
target_link_libraries (customprogram bdsim)