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
  histos          = nullptr;
  info            = nullptr;
}

void Event::SetBranchAddress(TChain *t, std::vector<std::string> &samplerNames)
{
  if(Config::Instance())
  {
    if(Config::Instance()->Debug()) std::cout << "Event::SetBranchAddress" << std::endl;
  }

  t->GetEntry(0); // Pointers don't appear to be valid without this

  t->SetBranchAddress("Primary.",&primaries);
  t->SetBranchAddress("Eloss.",&eloss);
  t->SetBranchAddress("PrimaryFirstHit.",&primaryFirstHit);
  t->SetBranchAddress("PrimaryLastHit.",&primaryLastHit);
  t->SetBranchAddress("TunnelHit.",&tunnelHit);
  t->SetBranchAddress("Trajectory.",&trajectory);
  t->SetBranchAddress("Histos.",&histos);

  if(Config::Instance())
  {
    if(Config::Instance()->Debug())
    {
      std::cout << "Event::SetBranchAddress> Primary.         " << primaries << std::endl;
      std::cout << "Event::SetBranchAddress> Eloss.           " << eloss << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryFirstHit. " << primaryFirstHit << std::endl;
      std::cout << "Event::SetBranchAddress> PrimaryLastHit.  " << primaryLastHit << std::endl;
      std::cout << "Event::SetBranchAddress> TunnelHit.       " << tunnelHit << std::endl;
      std::cout << "Event::SetBranchAddress> Trajectory.      " << trajectory << std::endl;
      std::cout << "Event::SetBranchAddress> Histos.          " << histos << std::endl;
    }
  }

  if(Config::Instance()->CalculateOpticalFunctions() || 
     Config::Instance()->ProcessSamplers()) {
    unsigned int nrSamplers = samplerNames.size();
    samplers.resize(nrSamplers);
    for(unsigned int i=0;i<nrSamplers;++i)
    {
      t->SetBranchAddress(samplerNames[i].c_str(),&samplers[i]);
      if(Config::Instance())
      {
	if(Config::Instance()->Debug())
	  {std::cout << "Event::SetBranchAddress> " << samplerNames[i] << " " << samplers[i] << std::endl;}
      }
    }
  }
}

Event::~Event()
{
  if(Config::Instance())
  {
    if(Config::Instance()->Debug())
      {std::cout << "Event::~Event>" << std::endl;}
  }
  delete primaries;
  delete eloss;
  delete primaryFirstHit;
  delete primaryLastHit;
  delete tunnelHit;
  delete trajectory;
  for(auto s = samplers.begin(); s != samplers.end(); ++s)
    {delete *s;}
}
