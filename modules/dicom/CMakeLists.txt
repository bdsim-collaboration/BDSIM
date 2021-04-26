file(GLOB BDS_DICOM_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)
file(GLOB BDS_DICOM_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/*.hh)

add_library(dicomModule SHARED ${BDS_DICOM_SOURCES})
target_include_directories(dicomModule SYSTEM PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(dicomModule SYSTEM PUBLIC ${Geant4_INCLUDE_DIRS})
target_link_libraries(dicomModule ${DCMKTK_LIBRARIES} ${Geant4_LIBRARIES})

target_link_libraries(${BDSIM_LIB_NAME} dicomModule)
target_include_directories(${BDSIM_LIB_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
