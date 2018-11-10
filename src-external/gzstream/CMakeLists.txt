# gzstream


if ($ENV{VERBOSE})
  message(STATUS "Building gzstream using library - ${ZLIB_LIB_LINK}")
endif()
# unpack gzstreamexecute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/gzstream.tgz ${CMAKE_CURRENT_BINARY_DIR})
#execute_process(COMMAND tar zxf gzstream.tgz WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}) 

# glob gzstream sources
file(GLOB GZSTREAM_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/*.C)
list(REMOVE_ITEM GZSTREAM_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test_gzip.C)
list(REMOVE_ITEM GZSTREAM_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/test_gunzip.C)
file(GLOB GZSTREAM_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/*.h)
#if($ENV{VERBOSE})
  message(STATUS "GZSTREAM sources ${GZSTREAM_SOURCES}")
#endif()

link_directories(${CMAKE_CURRENT_BINARY_DIR}/gzstream)

add_library(gzstream SHARED ${GZSTREAM_SOURCES})
target_link_libraries(gzstream ${ZLIB_LIB_LINK})
target_include_directories(gzstream INTERFACE
		           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}:gzstream>
                           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}:include/gzstream>)

bdsim_install_targets(gzstream)
