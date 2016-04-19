{
  gSystem->Load("libbdsimRootEvent.dylib");
  gSystem->Load("librebdsimLib.dylib");
  gSystem->AddIncludePath("-D__ROOTBUILD__");
}

void setup()
{
  gROOT->LoadMacro("Utility.C+");
  gROOT->LoadMacro("UtilityBeam.C+");
  gROOT->LoadMacro("UtilityModel.C+");
}