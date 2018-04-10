rebdsim_test(analysis-rebdsim-multiple-files  "multipleFilesAnalysis.txt")
rebdsim_test(analysis-rebdsim-selection       "selection.txt")

rebdsim_combine_test(analysis-combine combined-ana.root ../../data/ana1.root ../../data/ana2.root)

rebdsim_histomerge_test(analysis-histomerge ../../data/sample1.root histomerge.root )