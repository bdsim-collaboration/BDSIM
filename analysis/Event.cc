#include "Config.hh"
#include "Event.hh"


ClassImp(Event)

Event::Event()
{
  this->CommonCtor();

}

void Event::CommonCtor()
{
  primaries       = 0;
  eloss           = nullptr;
  primaryFirstHit = nullptr;
  primaryLastHit  = nullptr;
  tunnelHit       = nullptr;
  trajectory      = nullptr;
}
#ifndef __ROOTDOUBLE__
BDSOutputROOTEventSampler<float>*    Event::GetPrimaries() 
#else 
BDSOutputROOTEventSampler<double>*    Event::GetPrimaries() 
#endif
{
  return primaries;
}

BDSOutputROOTEventLoss*       Event::GetLoss()
{
  return eloss;
}

BDSOutputROOTEventHit*        Event::GetPrimaryFirstHit()
{
  return primaryFirstHit;
}

BDSOutputROOTEventHit*        Event::GetPrimaryLastHit()
{
  return primaryLastHit;
}

BDSOutputROOTEventHit*        Event::GetTunnelHit()
{
  return tunnelHit;
}

BDSOutputROOTEventTrajectory* Event::GetTrajectory()
{
  return trajectory;
}

void Event::SetBranchAddress(TChain *t, std::vector<std::string> &samplerNames)
{
  if(Config::Instance()->Debug())
  {
    std::cout << "Event::SetBranchAddress" << std::endl;
  }

  t->GetEntry(0);                                            // Pointers don't appear to be valid without this

  t->SetBranchAddress("Primary.",&primaries);
  t->SetBranchAddress("Eloss.",&eloss);
  t->SetBranchAddress("PrimaryFirstHit.",&primaryFirstHit);
  t->SetBranchAddress("PrimaryLastHit.",&primaryLastHit);
  t->SetBranchAddress("TunnelHit.",&tunnelHit);
  t->SetBranchAddress("Trajectory.",&trajectory);

  if(Config::Instance()->Debug())
  {
    std::cout << "Event::SetBranchAddress> Primary.         " << primaries << std::endl;
    std::cout << "Event::SetBranchAddress> Eloss.           " << eloss << std::endl;
    std::cout << "Event::SetBranchAddress> PrimaryFirstHit. " << primaryFirstHit << std::endl;
    std::cout << "Event::SetBranchAddress> PrimaryLastHit.  " << primaryLastHit << std::endl;
    std::cout << "Event::SetBranchAddress> TunnelHit.       " << tunnelHit << std::endl;
    std::cout << "Event::SetBranchAddress> Trajectory.      " << trajectory << std::endl;
  }

  for(int i=0;i<(int)samplerNames.size();++i)
  {
    t->SetBranchAddress(samplerNames[i].c_str(),&samplersA[i]);
    if(Config::Instance()->Debug())
    {
      std::cout << "Event::SetBranchAddress> " << samplerNames[i].c_str() << " " << samplersA[i] << std::endl;
    }
    samplers.push_back(samplersA[i]);
  }
}

Event::~Event()
{
  if(Config::Instance()->Debug())
  {
    std::cout <<"Event::~Event>" << std::endl;
  }
  delete primaries;
  delete eloss;
  delete primaryFirstHit;
  delete primaryLastHit;
  delete tunnelHit;
  delete trajectory;
  for(auto s = samplers.begin(); s != samplers.end(); ++s)
  {
    delete *s;
  }
}
