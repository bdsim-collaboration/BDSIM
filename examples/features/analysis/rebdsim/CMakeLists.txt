rebdsim_test(analysis-rebdsim                    "analysisConfig.txt")
rebdsim_test(analysis-rebdsim-multiple-files     "multipleFilesAnalysis.txt")
rebdsim_test(analysis-rebdsim-selection          "selection.txt")
rebdsim_test(analysis-rebdsim-event-range        "analysisConfigRange.txt")
rebdsim_test(analysis-spectra                    "spectra.txt")
rebdsim_test(analysis-spectra-simple             "spectra-simple.txt")
rebdsim_test(analysis-spectra-all                "spectra-all.txt")
rebdsim_test(analysis-spectra-ions               "spectra-ions.txt")
rebdsim_test(analysis-spectra-particles          "spectra-particles.txt")
rebdsim_test(analysis-spectra-primary-secondary  "spectra-primary-secondary.txt")
rebdsim_test(analysis-spectra-top10              "spectra-top10.txt")
rebdsim_test(analysis-spectra-top10ions          "spectra-top10ions.txt")
rebdsim_test(analysis-spectra-top10particles     "spectra-top10particles.txt")
rebdsim_test(analysis-spectra-rigidity           "spectra-rigidity.txt")

# tests that should fail
add_test(NAME analysis-rebdsim-event-range-invalid COMMAND rebdsimexec analysisConfigRangeInvalid.txt)
set_tests_properties(analysis-rebdsim-event-range-invalid PROPERTIES WILL_FAIL 1)

rebdsim_combine_test(analysis-combine combined-ana.root ../../data/ana1.root ../../data/ana2.root)

rebdsim_histomerge_test(analysis-histomerge ../../data/sample1.root histomerge.root )