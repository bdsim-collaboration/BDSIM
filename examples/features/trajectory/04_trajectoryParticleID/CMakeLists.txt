# short versions - generation only
set(TESTING_ARGS --ngenerate=1)
simple_testing(trajectory-em-e-250MeV-id=+11 "--file=electron_250GeV_id=+11.gmad" "")
set(TESTING_ARGS --ngenerate=1)
simple_testing(trajectory-em-e-250MeV-id=-11 "--file=electron_250GeV_id=+11.gmad" "")
