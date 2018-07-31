# short versions - generation only
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_withTransportation)
simple_testing(trajectory-em-e-250MeV-withtransportation "--file=electron_250MeV_withTransportation.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_withoutTransportation)
simple_testing(trajectory-em-e-250MeV-withouttransportation "--file=electron_250MeV_withoutTransportation.gmad" "")
