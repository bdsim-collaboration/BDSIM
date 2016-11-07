#include "Options.hh"
#include "BDSOutputROOTEventOptions.hh"

#include "TTree.h"

ClassImp(Options)

Options::Options()
{
  options = new BDSOutputROOTEventOptions();
}

Options::~Options()
{
  delete options;
}

void Options::SetBranchAddress(TTree *t)
{
  t->SetBranchAddress("Options.",&options);
}
