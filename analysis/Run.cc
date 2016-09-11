#include "Run.hh"

#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventHistograms.hh"

ClassImp(Run)

Run::Run()
{
  info   = nullptr;
  histos = nullptr;
}

Run::~Run()
{;}

void Run::SetBranchAddress(TChain *t)
{
  t->GetEntry(0);  // Initialises local copy of class
  t->SetBranchAddress("Info.",&info);
  t->SetBranchAddress("Histos.",&histos);
}
