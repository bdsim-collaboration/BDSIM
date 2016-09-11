#include "Options.hh"

#include "TChain.h"

ClassImp(Options)

Options::Options()
{
  options = nullptr;
}

Options::~Options()
{
  delete options;
}

void Options::SetBranchAddress(TChain *t)
{
  t->GetEntry(0);  // Initialises local copy of class
  t->SetBranchAddress("Options.",&options);
}
