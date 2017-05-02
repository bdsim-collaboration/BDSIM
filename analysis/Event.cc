#include "Config.hh"
#include "Event.hh"

#include "BDSOutputROOTEventHistograms.hh"
#include "BDSOutputROOTEventInfo.hh"
#include "BDSOutputROOTEventLoss.hh"
#include "BDSOutputROOTEventTrajectory.hh"

#include <vector>

#include "TChain.h"

ClassImp(Event)

Event::Event():
  debug(false),
  processSamplers(false)
{
  CommonCtor();
}

Event::Event(bool debugIn,
	     bool processSamplersIn):
  debug(debugIn),
  processSamplers(processSamplersIn)
{
  CommonCtor();
}

void Event::CommonCtor()
{
  primaries       = nullptr;
  eloss           = nullptr;
  primaryFirstHit = nullptr;
  primaryLastHit  = nullptr;
  tunnelHit       = nullptr;
  trajectory      = nullptr;
  histos          = nullptr;
  info            = nullptr;
}

void Event::SetBranchAddress(TTree *t, std::vector<std::string>* samplerNames)
{
  if(debug)
    {std::cout << "Event::SetBranchAddress" << std::endl;}

  // turn off all branches by default.
  t->SetBranchStatus("*", 0);

  // turn on what we need
  if (Config::Instance()->GetOptionBool("mergehistograms"))
    {t->SetBranchStatus("Histos.*", 1);}

  if (Config::Instance()->AllEventBranchesToBeActivated())
    {t->SetBranchStatus("*", 1);}
  else
    {
      auto branchNames = Config::Instance()->BranchesToBeActivated("Event.");
      for (auto name : branchNames)
	{
	  if (debug)
	    {std::cout << "Turning on branch \"" << name << "\"" << std::endl;}
	  std::string nameStar = name + ".*";
	  t->SetBranchStatus(nameStar.c_str(), 1);
	}
    }

  t->GetEntry(0); // this initialises the local variables it would seem.
  // only set address of primary branch if it exists
  if (((*t).GetListOfBranches()->FindObject("Primary.")) != nullptr)
    {t->SetBranchAddress("Primary.",     &primaries);}
  t->SetBranchAddress("Eloss.",          &eloss);
  t->SetBranchAddress("Histos.",         &histos);
  t->SetBranchAddress("PrimaryFirstHit.",&primaryFirstHit);
  t->SetBranchAddress("PrimaryLastHit.", &primaryLastHit);
  t->SetBranchAddress("TunnelHit.",      &tunnelHit);
  t->SetBranchAddress("Trajectory.",     &trajectory);
  t->SetBranchAddress("Info.",           &info);

  if(debug)
    {
      std::cout << "Event::SetBranchAddress> Primary.         " << primaries       << std::endl;
      std::cout << "Event::SetBranchAddress> Eloss.           " << eloss           << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryFirstHit. " << primaryFirstHit << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryLastHit.  " << primaryLastHit  << std::endl;
      std::cout << "Event::SetBranchAddress> TunnelHit.       " << tunnelHit       << std::endl;
      std::cout << "Event::SetBranchAddress> Trajectory.      " << trajectory      << std::endl;
      std::cout << "Event::SetBranchAddress> Histos.          " << histos          << std::endl;
      std::cout << "Event::SetBranchAddress> Info.            " << info            << std::endl;
    }

  if (processSamplers && samplerNames)
  {
    unsigned int nrSamplers = samplerNames->size();
    samplers.resize(nrSamplers); // reserve and nominally instantiate instances.
    for(unsigned int i=0;i<nrSamplers;++i)
    {
      t->SetBranchAddress((*samplerNames)[i].c_str(),&samplers[i]);
      if(debug)
      {std::cout << "Event::SetBranchAddress> " << (*samplerNames)[i] << " " << samplers[i] << std::endl;}
	  }
  }
}

Event::~Event()
{
  delete primaries;
  delete eloss;
  delete primaryFirstHit;
  delete primaryLastHit;
  delete tunnelHit;
  delete trajectory;
  for(auto s = samplers.begin(); s != samplers.end(); ++s)
    {delete *s;}
}
