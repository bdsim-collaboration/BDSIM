#include "RebdsimTypes.hh"
#include "Run.hh"

#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventHistograms.hh"

#include <iostream>
#include <vector>

ClassImp(Run)

Run::Run():
Run(false)
{;}

Run::Run(bool debugIn):
  info(nullptr),
  histos(nullptr),
  debug(debugIn)
{
  info   = new BDSOutputROOTEventRunInfo();
  histos = new BDSOutputROOTEventHistograms();
}

Run::~Run()
{
  delete info;
  delete histos;
}

void Run::SetBranchAddress(TTree *t,
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
  
  t->SetBranchAddress("Info.",&info);
  t->SetBranchAddress("Histos.",&histos);
}
