# short versions - generation only
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_allTrajectories)
simple_testing(trajectory-em-e-250MeV-alltrajectories "--file=electron_250MeV_allTrajectories.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250GeV_allTrajectories)
simple_testing(trajectory-em-e-250GeV-alltrajectories "--file=electron_250GeV_allTrajectories.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250GeV_justPrimary)
simple_testing(trajectory-em-e-250GeV-justprimary "--file=electron_250GeV_justPrimary.gmad" "")
