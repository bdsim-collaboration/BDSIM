# long tests for output analysis
#set(TESTING_ARGS --outfile=processes-em-e-250GeV-copper)
#simple_testing_long(processes-em-e-250GeV-copper_LONG    "--file=electron_250GeV_copper.gmad"    "")
#set(TESTING_ARGS --outfile=processes-em-e-250GeV-stainless)
#simple_testing_long(processes-em-e-250GeV-stainless_LONG "--file=electron_250GeV_stainless.gmad" "")
#set(TESTING_ARGS --outfile=processes-em-e-250GeV-titanium)
#simple_testing_long(processes-em-e-250GeV-titanium_LONG  "--file=electron_250GeV_titanium.gmad"  "")
#set(TESTING_ARGS --outfile=processes-em-e-250GeV-tungsten)
#simple_testing_long(processes-em-e-250GeV-tungsten_LONG  "--file=electron_250GeV_tungsten.gmad"  "")

generate_analyse_compare_long(processes-em-e-250GeV-copper electron_250GeV_copper.gmad processes-em-e-250GeV-analysis.txt)

# analysis and comparison of long tests above
#rebdsim_test_long(processes-em-e-250GeV-copper-analysis)
#rebdsim_test_long(processes-em-e-250GeV-stainless-analysis)
#rebdsim_test_long(processes-em-e-250GeV-titanium-analysis)
#rebdsim_test_long(processes-em-e-250GeV-tungsten-analysis)

# short tests
set(TESTING_ARGS --ngenerate=1)
simple_testing(processes-em-e-250GeV-copper "--file=electron_250GeV_copper.gmad" "")
set(TESTING_ARGS --ngenerate=1)
simple_testing(processes-em-e-250GeV-stainless "--file=electron_250GeV_stainless.gmad" "")
set(TESTING_ARGS  --ngenerate=1)
simple_testing(processes-em-e-250GeV-titanium "--file=electron_250GeV_titanium.gmad" "")
set(TESTING_ARGS  --ngenerate=1)
simple_testing(processes-em-e-250GeV-tungsten "--file=electron_250GeV_tungsten.gmad" "")
