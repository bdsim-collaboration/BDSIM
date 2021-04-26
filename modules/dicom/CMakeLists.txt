find_package(DCMTK REQUIRED)

file(GLOB BDS_DICOM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB BDS_DICOM_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh)

add_library(dicomModule SHARED ${BDS_DICOM_SOURCES})
target_include_directories(dicomModule PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${BDSIM_LIB_NAME} dicomModule)
target_include_directories(${BDSIM_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)