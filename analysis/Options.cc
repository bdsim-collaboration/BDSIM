#include "Options.hh"
#include "BDSOutputROOTEventOptions.hh"

#include "TTree.h"

ClassImp(Options)

Options::Options():
  Options(false)
{;}

Options::Options(bool debugIn):
  options(nullptr),
  debug(debugIn)
{;}

Options::~Options()
{
  delete options;
}

void Options::SetBranchAddress(TTree *t)
{
  t->GetEntry(0);
  t->SetBranchAddress("Options.",&options);
}
