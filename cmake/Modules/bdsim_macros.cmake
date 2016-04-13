#-----------------------------------------------------------------------
# macro BDSIM_COLLATE_APPLICATION_SOURCES(source_dest_var)
#
macro(BDSIM_COLLATE_APPLICATION_SOURCES source_dest_var)
  file(GLOB 
    ${source_dest_var} 
    ${CMAKE_CURRENT_BINARY_DIR}/bdsim.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/BDS*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/src/ggmad.cc
    )
endmacro()

#from https://cmake.org/pipermail/cmake/2009-March/027892.html

MACRO(COPY_FILE_IF_CHANGED in_file out_file target)
  IF(${in_file} IS_NEWER_THAN ${out_file})    
    #message(STATUS "Copying file: ${in_file} to: ${out_file}")
    ADD_CUSTOM_COMMAND (
      TARGET     ${target}
      POST_BUILD
      COMMAND    ${CMAKE_COMMAND}
      ARGS       -E copy ${in_file} ${out_file}
      #COMMENT    "Copying file: ${in_file} to: ${out_file}"
      )
  ENDIF(${in_file} IS_NEWER_THAN ${out_file})
ENDMACRO(COPY_FILE_IF_CHANGED)

# Copy all files and directories in in_dir to out_dir. 
# Subtrees remain intact.
MACRO(COPY_DIRECTORY_IF_CHANGED in_dir out_dir target)
  message(STATUS "Copying directory ${in_dir}")
  FILE(GLOB_RECURSE in_file_list ${in_dir}/*)
  FOREACH(in_file ${in_file_list})
    STRING(REGEX REPLACE ${in_dir} ${out_dir} out_file ${in_file})
    COPY_FILE_IF_CHANGED(${in_file} ${out_file} ${target})
  ENDFOREACH(in_file)
ENDMACRO(COPY_DIRECTORY_IF_CHANGED)
