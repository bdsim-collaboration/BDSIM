find_program(SPHINX_EXECUTABLE NAMES sphinx-build sphinx-build-2.7
    HINTS
    $ENV{SPHINX_DIR}
    PATH_SUFFIXES bin
    DOC "Sphinx documentation generator"
)

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Sphinx DEFAULT_MSG
    SPHINX_EXECUTABLE
)

# don't show variables in cmake GUI
mark_as_advanced(SPHINX_EXECUTABLE)