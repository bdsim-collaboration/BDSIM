# we should absolutely NOT include the subdirectories here for specific external examples
# these require the installed version so cannot possibly complete at build time and will
# break the build.

simple_fail(usercomponent-undefined "--file=undefinedusercomponent.gmad" "")
