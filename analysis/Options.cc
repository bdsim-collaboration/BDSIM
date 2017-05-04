#include "Options.hh"
#include "BDSOutputROOTEventOptions.hh"

#include "TTree.h"

#include <iostream>

ClassImp(Options)

Options::Options():
  Options(false)
{;}

Options::Options(bool debugIn):
  debug(debugIn)
{
  options = new BDSOutputROOTEventOptions();
}

Options::~Options()
{
  delete options;
}

void Options::SetBranchAddress(TTree *t,
			       bool                      allBranchesOn,
			       const RBDS::VectorString* branchesToTurnOn)
{
  // turn off all branches by default.
  t->SetBranchStatus("*", 0);

  if (allBranchesOn)
    {t->SetBranchStatus("*", 1);}
  else if (branchesToTurnOn)
    {
      for (auto name : *branchesToTurnOn)
	{
	  std::string nameStar = name + ".*"; // necessary because of the splitting
	  if (debug)
	    {std::cout << "Turning on branch \"" << nameStar << "\"" << std::endl;}
	  t->SetBranchStatus(nameStar.c_str(), 1);
	}
    }
  
  t->SetBranchAddress("Options.",&options);
}
