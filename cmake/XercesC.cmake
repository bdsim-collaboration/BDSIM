message(STATUS "Looking for XercesC for GDML support")
find_package(XercesC REQUIRED)
message(STATUS "Found XercesC version: ${XercesC_VERSION}")
include_directories(SYSTEM ${XercesC_INCLUDE_DIR})