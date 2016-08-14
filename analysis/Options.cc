#include "Options.hh"

#include "TChain.h"

ClassImp(Options)

Options::Options()
{
  options   = nullptr;
}

Options::~Options()
{
  delete options;
}

void Options::SetBranchAddress(TChain *t)
{
  t->GetEntry(0);  // Pointers don't appear to be valid without this
  t->SetBranchAddress("Options.",&options);
}
