void rootlogon() {
  gSystem->CompileMacro("classStubs.C");
  gSystem->CompileMacro("robdsimManager.C");
  gSystem->CompileMacro("sampler.C");
  //  gSystem->CompileMacro("lossTree.C");
  //  gSystem->CompileMacro("precisionLossTree.C");
  gSystem->CompileMacro("optics.C");
  //  gSystem->CompileMacro("loss.C"); 
}
