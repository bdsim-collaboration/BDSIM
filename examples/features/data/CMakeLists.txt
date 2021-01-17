add_subdirectory(dataVersions)


# skimming
add_test(NAME bdskim COMMAND bdskimexec skimselection.txt sample1.root sample1-skimmed.root)