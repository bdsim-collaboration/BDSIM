find_program(MADX_EXECUTABLE NAMES madx madx-linux32 madx-linux64 madx-macosx32 madx-macosx64 
    PATH_SUFFIXES bin
    DOC "MadX particle accelerator tracking"
)

# needs read permission on binary in order to find it!

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(MadX DEFAULT_MSG
    MADX_EXECUTABLE
)

# don't show variables in cmake GUI
mark_as_advanced(MADX_EXECUTABLE)