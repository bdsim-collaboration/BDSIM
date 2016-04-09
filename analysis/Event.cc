#include "Config.hh"
#include "Event.hh"


ClassImp(Event);

Event::Event()
{
  this->CommonCtor();

}

void Event::CommonCtor()
{
  primaries       = nullptr;
  eloss           = nullptr;
  primaryFirstHit = nullptr;
  primaryLastHit  = nullptr;
  tunnelHit       = nullptr;
  trajectory      = nullptr;
}

BDSOutputROOTEventSampler*    Event::GetPrimaries() 
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

void Event::SetBranchAddress(TChain *t)
{
  if(Config::Instance()->Debug())
  {
    std::cout << "Event::SetBranchAddress" << std::endl;
  }
  t->SetBranchAddress("Primary.",&primaries);
  t->SetBranchAddress("Eloss.",&eloss);
  t->SetBranchAddress("PrimaryFirstHit.",&primaryFirstHit);
  t->SetBranchAddress("PrimaryLastHit.",&primaryLastHit);
  t->SetBranchAddress("TunnelHit.",&tunnelHit);
  t->SetBranchAddress("Trajectory.",&trajectory);
}

Event::~Event()
{
  delete primaries;
  delete eloss;
  delete primaryFirstHit;
  delete primaryLastHit;
  delete tunnelHit;
  delete trajectory;
}
