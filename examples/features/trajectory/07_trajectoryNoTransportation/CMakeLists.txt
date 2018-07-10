# short versions - generation only
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_withTransportation)
simple_testing(trajectory-em-e-250MeV-alltrajectories "--file=electron_250MeV_withTransportation.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_withoutTransportation)
simple_testing(trajectory-em-e-250GeV-alltrajectories "--file=electron_250MeV_withoutTransportation.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250GeV_withTransportation)
simple_testing(trajectory-em-e-250MeV-alltrajectories "--file=electron_250GeV_withTransportation.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250GeV_withoutTransportation)
simple_testing(trajectory-em-e-250GeV-alltrajectories "--file=electron_250GeV_withoutTransportation.gmad" "")

