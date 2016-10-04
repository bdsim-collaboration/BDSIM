#include "Options.hh"
#include "BDSOutputROOTEventOptions.hh"

#include "TChain.h"

ClassImp(Options)

Options::Options()
{
  options = new BDSOutputROOTEventOptions();
}

Options::~Options()
{
  delete options;
}

void Options::SetBranchAddress(TChain *t)
{
  t->SetBranchAddress("Options.",&options);
}
