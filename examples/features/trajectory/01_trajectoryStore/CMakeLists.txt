# short versions - generation only
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_allTrajectories)
simple_testing(trajectory-em-e-250MeV-alltrajectories "--file=electron_250MeV_allTrajectories.gmad" "")
xset(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_justPrimary)
simple_testing(trajectory-em-e-250MeV-justprimary "--file=electron_250MeV_justPrimary.gmad" "")
