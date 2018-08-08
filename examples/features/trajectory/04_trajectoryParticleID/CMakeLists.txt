# short versions - generation only
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_id=+11)
simple_testing(trajectory-em-e-250mev-id=+11 "--file=electron_250MeV_id=+11.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_id=-11)
simple_testing(trajectory-em-e-250mev-id=-11 "--file=electron_250MeV_id=-11.gmad" "")
