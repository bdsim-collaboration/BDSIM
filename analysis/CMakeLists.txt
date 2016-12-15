file(GLOB rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/*.cc)

# Remove mains from dictionary
list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/rebdsim.cc)
list(REMOVE_ITEM rebdsimLibSources ${CMAKE_CURRENT_SOURCE_DIR}/edbdsim.cc)

file(GLOB rebdsimLibLinkDefs ${CMAKE_CURRENT_SOURCE_DIR}/*LinkDef.hh)

# For ROOT 5 there is no event display.
if (${ROOT_MAJOR_VERSION} STREQUAL "5")
   list(REMOVE_ITEM rebdsimLibSources  ${CMAKE_CURRENT_SOURCE_DIR}/EventDisplay.cc)
   list(REMOVE_ITEM rebdsimLibLinkDefs ${CMAKE_CURRENT_SOURCE_DIR}/EventDisplayLinkDef.hh)
endif()

#message(STATUS "${CMAKE_CURRENT_SOURCE_DIR}")
#message(STATUS "ROOT DICTS : ${rebdsimLibLinkDefs}")

# Add BDSIM label to original files
set_source_files_properties(${rebdsimLibSources} ${rebdsimLibLinkDefs} PROPERTIES LABELS "BDSIM")

foreach(header ${rebdsimLibLinkDefs})
    # remove LinkDef.hh
    string(FIND ${header} "LinkDef.h" pos REVERSE)
    string(FIND ${header} "/" dir REVERSE)
    MATH(EXPR beginpos "${dir}+1")
    MATH(EXPR length "${pos}-${beginpos}")
    string(SUBSTRING ${header} ${beginpos} ${length} className)
    #message(STATUS "${ROOTCINT_EXECUTABLE} -f ${CMAKE_CURRENT_BINARY_DIR}/root/${className}Dict.cc -c ${CMAKE_CURRENT_SOURCE_DIR}/include/${className}.hh ${header}")
    if (${ROOT_MAJOR_VERSION} STREQUAL "6")
        # add preprocessor flag __ROOTBUILD__ as used in BDSIM.
        # root6 .pcm file needs to be at library level
        # for discussion see e.g. this forum topic: https://root.cern.ch/phpBB3/viewtopic.php?t=17654
        add_custom_command(
                OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.cc
                ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm
                COMMAND ${ROOTCINT_EXECUTABLE}
                ARGS -f ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.cc
                -D__ROOTBUILD__
                -c -p -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}/../include
                -I${CMAKE_CURRENT_SOURCE_DIR}/../
                ${CMAKE_CURRENT_SOURCE_DIR}/${className}.hh ${header}
                DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${className}.hh ${header}
                IMPLICIT_DEPENDS CXX ${CMAKE_CURRENT_SOURCE_DIR}/${className}.hh ${header}
                COMMENT "Generate ROOT Dictionary for ${className}"
        )
        add_custom_command(
                OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm
                COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm
                ${CMAKE_CURRENT_BINARY_DIR}
                DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.cc
        )
        # add arbitrary dependence to make it run at compile time (since DEPENDS at custom_command seems not to work)
        set_property(
                SOURCE ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.cc
                APPEND PROPERTY OBJECT_DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm
        )

        # keep list of ROOT pcms
        set(rebdsim_pcms ${rebdsim_pcms} ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict_rdict.pcm)
    else()
        # ROOT 5
        add_custom_command(
                OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.cc ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.h
                COMMAND ${ROOTCINT_EXECUTABLE}
                ARGS -f ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.cc
                -c -p -I${CMAKE_CURRENT_SOURCE_DIR} -I${CMAKE_CURRENT_SOURCE_DIR}/../include
		-D__ROOTBUILD__
                -I${CMAKE_CURRENT_SOURCE_DIR}/../
                ${CMAKE_CURRENT_SOURCE_DIR}/${className}.hh ${header}
                DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${className}.hh ${header}
                IMPLICIT_DEPENDS CXX ${CMAKE_CURRENT_SOURCE_DIR}/${className}.hh ${header}
                COMMENT "Generate ROOT Dictionary for ${className}"
        )
    endif()
    set(rebdsimLibSources ${rebdsimLibSources} ${CMAKE_CURRENT_BINARY_DIR}/${className}Dict.cc)
endforeach()


add_library(rebdsimLib SHARED ${rebdsimLibSources})
target_link_libraries(rebdsimLib bdsimRootEvent)

add_executable(rebdsim rebdsim.cc)
set_target_properties(rebdsim PROPERTIES OUTPUT_NAME "rebdsim" VERSION ${BDSIM_VERSION})
target_link_libraries(rebdsim rebdsimLib bdsimRootEvent)

add_executable(rebdsimOptics rebdsimOptics.cc)
set_target_properties(rebdsimOptics PROPERTIES OUTPUT_NAME "rebdsimOptics" VERSION ${BDSIM_VERSION})
target_link_libraries(rebdsimOptics rebdsimLib bdsimRootEvent)
bdsim_install_targets(rebdsimOptics)

if (${ROOT_MAJOR_VERSION} STREQUAL "6")
	add_executable(edbdsim edbdsim.cc)
	set_target_properties(edbdsim PROPERTIES OUTPUT_NAME "edbdsim" VERSION ${BDSIM_VERSION})
	target_link_libraries(edbdsim rebdsimLib ${ROOT_EVELIBRARIES})
	target_link_libraries(rebdsimLib ${ROOT_EVELIBRARIES})
	#message(STATUS ${ROOT_EVELIBRARIES})
	# Installation
	bdsim_install_targets(rebdsim edbdsim rebdsimLib)
else()
	# ROOT 5 - EVE parts are not compatible with ROOT5 - turn off.
	message(STATUS "ebdsim is not compatible with ROOT V5 - not compiling")
	
	# Installation
	bdsim_install_targets(rebdsim rebdsimLib)
endif()

# Install pcms
bdsim_install_libs(${rebdsim_pcms})

file(GLOB rebdsimMacros ${CMAKE_CURRENT_SOURCE_DIR}/*.C ${CMAKE_CURRENT_SOURCE_DIR}/*.H)
bdsim_install_macros(${rebdsimMacros})
#message(STATUS "rebdsimMacros ${rebdsimMacros}")

file(GLOB rebdsimHeaders ${CMAKE_CURRENT_SOURCE_DIR}/*.hh)
# remove linkdefs
foreach(header ${rebdsimLibLinkDefs})
  list(REMOVE_ITEM rebdsimHeaders ${header})
endforeach()
bdsim_install_analysis_headers(${rebdsimHeaders})
#message(STATUS "rebdsimHeaders ${rebdsimHeaders}")

get_target_property(rebdsimBinaryName rebdsim OUTPUT_NAME)
set(rebdsimBinary ${CMAKE_CURRENT_BINARY_DIR}/${rebdsimBinaryName} CACHE STRING "rebdsim binary")

# load test macros
include(rebdsim_test_macros)
