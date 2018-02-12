add_subdirectory(1_rootevent)


simple_fail(io-invalid-sampler-name "--file=invalid_sampler_name.gmad")

set(TESTING_ARGS --outfile=a/fictional/path/to/data.root)
simple_fail(io-invalid-file-path  "--file=sm.gmad")