#include "Config.hh"
#include "Run.hh"

#include "BDSOutputROOTEventRunInfo.hh"
#include "BDSOutputROOTEventHistograms.hh"

#include <vector>

ClassImp(Run)

Run::Run():
Run(false)
{;}

Run::Run(bool debugIn):
  info(nullptr),
  histos(nullptr),
  debug(debugIn)
{;}

Run::~Run()
{
  delete info;
  delete histos;
}

void Run::SetBranchAddress(TTree *t)
{
  t->SetBranchStatus("*", 0);
  t->SetBranchStatus("Histos.*", 1); // always want to merge histograms

  auto branchNames = Config::Instance()->BranchesToBeActivated("Run.");
  for (auto name : branchNames)
    {
      if (debug)
	{std::cout << "Turning on branch \"" << name << "\"" << std::endl;}
      std::string nameStar = name + ".*";
      t->SetBranchStatus(nameStar.c_str(), 1);
    }
  
  t->GetEntry(0);  // Initialises local copy of class
  t->SetBranchAddress("Info.",&info);
  t->SetBranchAddress("Histos.",&histos);
}
