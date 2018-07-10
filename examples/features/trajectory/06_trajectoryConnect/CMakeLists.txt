# short versions - generation only
# set(TESTING_ARGS --ngenerate=1)
# simple_testing(trajectory-em-e-250mev-electron-connect "--file=electron_250MeV_electron.gmad" "")
# set(TESTING_ARGS --ngenerate=1)
# simple_testing(trajectory-em-e-250gev-electron-connect "--file=electron_250GeV_electron.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_positron)
simple_testing(trajectory-em-e-250mev-positron-connect "--file=electron_250MeV_positron.gmad" "")
# set(TESTING_ARGS --ngenerate=1)
# simple_testing(trajectory-em-e-250gev-positron-connect "--file=electron_250GeV_positron.gmad" "")

