# short versions - generation only
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_electron)
simple_testing(trajectory-em-e-250mev-electron "--file=electron_250MeV_electron.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_positron)
simple_testing(trajectory-em-e-250mev-positron "--file=electron_250MeV_positron.gmad" "")


