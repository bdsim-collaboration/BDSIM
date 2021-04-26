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

option(USE_DICOM "Use DCMTK library for DICOM file handling" OFF)
if (USE_DICOM)
  add_definitions("-DUSE_DICOM")
  message(STATUS "DICOM module ON")
  add_subdirectory(dicom)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/dicom/include)
endif()