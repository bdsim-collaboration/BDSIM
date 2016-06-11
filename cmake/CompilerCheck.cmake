# Compiler-specific C++11 activation. Replace later with
# CMAKE_CXX_STANDARD flag only available for cmake 3.1

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
  if($ENV{VERBOSE})
	message(STATUS "GNU compiler detected")
	message(STATUS "GCC Version is ${GCC_VERSION}")
  endif()
  if (GCC_VERSION VERSION_LESS 4.9)
     message(FATAL_ERROR "C++ compiler ${CMAKE_CXX_COMPILER} does not support C++11 fully - GCC4.9 or greater requried.")
  endif ()
  # if it passes the above if guard, then it's > GCC4.9 so C++14 ok
  message(STATUS "Compiler supports C++11 (fully) and C++14")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  # get clang version
  execute_process( COMMAND ${CMAKE_CXX_COMPILER} --version OUTPUT_VARIABLE clang_full_version_string )
  string (REGEX REPLACE ".*version ([0-9]+\\.[0-9]+).*" "\\1" CLANG_VERSION_STRING ${clang_full_version_string})
  if($ENV{VERBOSE})
    message(STATUS "CLANG_VERSION_STRING ${CLANG_VERSION_STRING}")
  endif()
  if (CLANG_VERSION_STRING VERSION_GREATER 6.9)
    message(STATUS "Compiler supports C++14")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14 -stdlib=libc++")
  else()
    # apparently all versions of CLANG support C++11...
    message(STATUS "Compiler supports C++11")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
  endif()
else()
    # unknown compiler so assume doesn't support C++11
    message(FATAL_ERROR "C++ compiler ${CMAKE_CXX_COMPILER} does not support C++11")
endif()

set(CMAKE_C_ORIG_FLAGS "${CMAKE_C_FLAGS}")
set(CMAKE_CXX_ORIG_FLAGS "${CMAKE_CXX_FLAGS}")
if($ENV{VERBOSE})
	message(STATUS "CMAKE_C_ORIG_FLAGS ${CMAKE_C_ORIG_FLAGS}")
	message(STATUS "CMAKE_CXX_ORIG_FLAGS ${CMAKE_CXX_ORIG_FLAGS}")
endif()
