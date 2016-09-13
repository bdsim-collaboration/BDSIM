# long tests for output analysis
set(TESTING_ARGS --outfile=electron_250GeV_copper)
simple_testing_long(electron_250GeV_copper_LONG "--file=electron_250GeV_copper.gmad" "")
set(TESTING_ARGS --outfile=electron_250GeV_stainless)
simple_testing_long(electron_250GeV_stainless_LONG "--file=electron_250GeV_stainless.gmad" "")
set(TESTING_ARGS --outfile=electron_250GeV_titanium)
simple_testing_long(electron_250GeV_titanium_LONG "--file=electron_250GeV_titanium.gmad" "")
set(TESTING_ARGS --outfile=electron_250GeV_tungsten)
simple_testing_long(electron_250GeV_tungsten_LONG "--file=electron_250GeV_tungsten.gmad" "")

# analysis and comparison of long tests above
rebdsim_test_long(electron_250GeV_copper)
rebdsim_test_long(electron_250GeV_stainless)
rebdsim_test_long(electron_250GeV_titanium)
rebdsim_test_long(electron_250GeV_tungsten)

# short tests
set(TESTING_ARGS --ngenerate=1)
simple_testing(electron_250GeV_copper "--file=electron_250GeV_copper.gmad" "")
set(TESTING_ARGS --ngenerate=1)
simple_testing(electron_250GeV_stainless "--file=electron_250GeV_stainless.gmad" "")
set(TESTING_ARGS  --ngenerate=1)
simple_testing(electron_250GeV_titanium "--file=electron_250GeV_titanium.gmad" "")
set(TESTING_ARGS  --ngenerate=1)
simple_testing(electron_250GeV_tungsten "--file=electron_250GeV_tungsten.gmad" "")
