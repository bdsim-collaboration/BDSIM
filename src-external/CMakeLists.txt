add_subdirectory(gdml)

#find_package(ZLIB)

option(USE_GZSTREAM "Zlib support through gzstream library - requires zlib" ON)

#define the builder function
function(build_gzstream ZLIB_LIB_LINK)
  if ($ENV{VERBOSE})
    message(STATUS "Building gzstream using library - ${ZLIB_LIB_LINK}")
  endif()
  # unpack gzstream
  execute_process(COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/gzstream.tgz ${CMAKE_CURRENT_BINARY_DIR})
  execute_process(COMMAND tar zxf gzstream.tgz WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}) 

  # glob gzstream sources
  file(GLOB GZSTREAM_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/gzstream/*.C)
  list(REMOVE_ITEM GZSTREAM_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/gzstream/test_gzip.C)
  list(REMOVE_ITEM GZSTREAM_SOURCES ${CMAKE_CURRENT_BINARY_DIR}/gzstream/test_gunzip.C)
  file(GLOB GZSTREAM_HEADERS ${CMAKE_CURRENT_BINARY_DIR}/gzstream/*.h)
  if($ENV{VERBOSE})
    message(STATUS "GZSTREAM sources ${GZSTREAM_SOURCES}")
  endif()

  link_directories(${CMAKE_CURRENT_BINARY_DIR}/gzstream)

  add_library(gzstream SHARED ${GZSTREAM_SOURCES})
  target_link_libraries(gzstream ${ZLIB_LIB_LINK})
  target_include_directories(gzstream PUBLIC ${CMAKE_CURRENT_BINARY_DIR}/gzstream)

  bdsim_install_targets(gzstream)

endfunction(build_gzstream)


if (USE_GZSTREAM)
  message(STATUS "Looking for zlib")
  # first search for geant4 zlib
  # then general system zlib
  # then if none, disable zlib component
  if (Geant4_builtin_zlib_FOUND)
    add_definitions("-DUSE_GZSTREAM")
    set(ZLIB_LIBS "G4zlib")
    message(STATUS "Using Geant4 built in zlib")
    build_gzstream(${ZLIB_LIBS})
  else()
    find_package(ZLIB)
      if(ZLIB_FOUND)
        add_definitions("-DUSE_GZSTREAM")
        set(ZLIB_LIBS ${ZLIB_LIBRARIES})
        message(STATUS "Using system zlib")
        build_gzstream(${ZLIB_LIBS})
      else()
        set(ZLIB_LIBS "")
        set(USE_GZSTREAM OFF)
        message(STATUS "No Geant4 included or system ZLIB found - turning off zlib support")
      endif(ZLIB_FOUND)
   endif(Geant4_builtin_zlib_FOUND)
endif(USE_GZSTREAM)
