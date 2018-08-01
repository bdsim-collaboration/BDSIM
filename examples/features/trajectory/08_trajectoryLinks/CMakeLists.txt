# short versions - generation only
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_samplerConnect)
simple_testing(trajectory-em-e-250mev-samplerconnect "--file=electron_250MeV_samplerConnect.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_elossConnect)
simple_testing(trajectory-em-e-250mev-elossconnect "--file=electron_250MeV_elossConnect.gmad" "")

