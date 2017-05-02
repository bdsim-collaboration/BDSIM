#include "Config.hh"
#include "Run.hh"

#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventHistograms.hh"

#include <vector>

ClassImp(Run)

Run::Run():
Run(false)
{;}

Run::Run(bool debugIn):
  info(nullptr),
  histos(nullptr),
  debug(debugIn)
{;}

Run::~Run()
{
  delete info;
  delete histos;
}

void Run::SetBranchAddress(TTree *t)
{
  t->GetEntry(0);  // Initialises local copy of class
  t->SetBranchAddress("Info.",&info);
  t->SetBranchAddress("Histos.",&histos);
}
