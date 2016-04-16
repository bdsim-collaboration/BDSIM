{
  gSystem->Load("libbdsimRootEvent.dylib");
  gSystem->Load("librebdsimLib.dylib");
  gSystem->AddIncludePath("-D__ROOTBUILD__");
}