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
  t->SetBranchAddress("Options.",&options);
}
