# awake experiment-specific classes
option(USE_AWAKE "Use AWAKE Module" OFF)
if (USE_AWAKE)
  add_definitions("-DUSE_AWAKE")
  message(STATUS "AWAKE module ON")
  add_subdirectory(AWAKE)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/AWAKE/include)
endif()

# material builder
add_subdirectory(nistmaterials)

if (USE_DICOM)
  add_subdirectory(dicom)
endif()