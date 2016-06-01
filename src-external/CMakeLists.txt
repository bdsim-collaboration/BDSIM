cmake_minimum_required(VERSION 2.6.4)

# unpack gzstream
execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/gzstream.tgz ${CMAKE_CURRENT_BINARY_DIR})
execute_process(COMMAND tar zxf gzstream.tgz
WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}) 

# glob gzstream sources
file(GLOB GZSTREAM_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/gzstream/*.C)
list(REMOVE_ITEM GZSTREAM_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/gzstream/test_gzip.C)
list(REMOVE_ITEM GZSTREAM_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/gzstream/test_gunzip.C)
file(GLOB GZSTREAM_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/gzstream/*.h)
if($ENV{VERBOSE})
  message(STATUS "GZSTREAM sources ${GZSTREAM_SOURCES}")
endif()

add_library(gzstream STATIC ${GZSTREAM_SOURCES})
target_include_directories(gzstream PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/gzstream)

bdsim_install_targets(gzstream)


