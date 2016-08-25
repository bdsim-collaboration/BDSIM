#include "Run.hh"

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
  t->GetEntry(0);  // Pointers don't appear to be valid without this
  t->SetBranchAddress("Info.",&info);
  t->SetBranchAddress("Histos.",&histos);
}
