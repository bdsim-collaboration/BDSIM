file(GLOB rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)
list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/src/rebdsim.cc)

file(GLOB rebdsimLibLinkDefs ${CMAKE_CURRENT_SOURCE_DIR}/*.LinkDef.h)

add_library(rebdsimLib SHARED ${rebdsimLibSources})

target_link_libraries(rebdsimLib ${ROOT_LIBRARIES_GLOB})
target_link_libraries(rebdsimLib ${ROOT_LIBRARIES})
target_link_libraries(rebdsimLib bdsimRootEvent)

add_executable(rebdsim rebdsim.cc)
set_target_properties(rebdsim PROPERTIES OUTPUT_NAME "rebdsim" VERSION ${BDSIM_VERSION})
target_link_libraries(rebdsim rebdsimLib bdsimRootEvent)

set (bdsimBinary rebdsim)
