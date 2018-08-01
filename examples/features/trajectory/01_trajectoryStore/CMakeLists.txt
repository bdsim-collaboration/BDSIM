# short versions - generation only
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_allTrajectories)
simple_testing(trajectory-em-e-250mev-alltrajectories "--file=electron_250MeV_allTrajectories.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_justPrimary)
simple_testing(trajectory-em-e-250mev-justprimary "--file=electron_250MeV_justPrimary.gmad" "")
