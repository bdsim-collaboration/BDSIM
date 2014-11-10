void rootlogon() {
  gSystem->CompileMacro("classStubs.C");
  gSystem->CompileMacro("robdsimManager.C");
  gSystem->CompileMacro("Sampler.C");
  gSystem->CompileMacro("Eloss.C");
  gSystem->CompileMacro("PrecisionRegionELoss.C");
  gSystem->CompileMacro("optics.C");
  //  gSystem->CompileMacro("loss.C"); 
}
