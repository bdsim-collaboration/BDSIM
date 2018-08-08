# short versions - generation only
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_100MeVECut)
simple_testing(trajectory-em-e-250mev-100mevecut "--file=electron_250MeV_100MeVECut.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_050MeVECut)
simple_testing(trajectory-em-e-250mev-050mevecut "--file=electron_250MeV_050MeVECut.gmad" "")
set(TESTING_ARGS --ngenerate=1 --outfile=electron_250MeV_025MeVECut)
simple_testing(trajectory-em-e-250meV-025mevecut "--file=electron_250MeV_025MeVECut.gmad" "")
