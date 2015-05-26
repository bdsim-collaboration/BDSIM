# CMake for Sphinx based on
# http://ericscottbarr.com/blog/2012/03/sphinx-and-cmake-beautiful-documentation-for-c-projects/

find_package(Sphinx)

#if(NOT DEFINED SPHINX_THEME)
#    set(SPHINX_THEME default)
#endif()

#if(NOT DEFINED SPHINX_THEME_DIR)
#    set(SPHINX_THEME_DIR)
#endif()

# configured documentation tools and intermediate build results
set(BINARY_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}")

# Sphinx cache with pickled ReST documents
set(SPHINX_CACHE_DIR "${CMAKE_CURRENT_BINARY_DIR}/doctrees")

# HTML output directory
set(SPHINX_HTML_DIR "${CMAKE_CURRENT_BINARY_DIR}/html")
# LATEX output directory
set(SPHINX_LATEX_DIR "${CMAKE_CURRENT_BINARY_DIR}/latex")
# Add directory to be able to add manual-pdf command in one go (hack)
file(MAKE_DIRECTORY ${SPHINX_LATEX_DIR}) 

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/source/conf.py.in"
    "${BINARY_BUILD_DIR}/conf.py"
    @ONLY)

add_custom_target(manual
    ${SPHINX_EXECUTABLE}
        -q -b html
        -c "${BINARY_BUILD_DIR}"
        -d "${SPHINX_CACHE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
        "${SPHINX_HTML_DIR}"
    COMMENT "Building HTML documentation with Sphinx")

# The number of LaTeX runs is quite conservative, but I don't expect it
# to get run often, so the little extra time won't hurt.
add_custom_target(manual-pdf
    ${SPHINX_EXECUTABLE}
        -q -b latex
        -c "${BINARY_BUILD_DIR}"
        -d "${SPHINX_CACHE_DIR}"
        "${CMAKE_CURRENT_SOURCE_DIR}/source"
        "${SPHINX_LATEX_DIR}"
    WORKING_DIRECTORY ${SPHINX_LATEX_DIR} 
    COMMAND pdflatex BDSIM
    COMMAND pdflatex BDSIM
    COMMAND makeindex -s python.ist BDSIM.idx
    COMMAND pdflatex BDSIM
    COMMAND pdflatex BDSIM
    COMMENT "Building LaTeX documentation with Sphinx")
