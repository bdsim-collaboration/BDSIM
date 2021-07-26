find_package(XercesC REQUIRED)

include_directories(SYSTEM ${XercesC_INCLUDE_DIR})

message(STATUS "NOW NOW ${INCLUDE_DIRECTORIES}")
message(STATUS "NOW NOW ${XercesC_INCLUDE_DIR}")


get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
  message(STATUS "dir='${dir}'")
endforeach()