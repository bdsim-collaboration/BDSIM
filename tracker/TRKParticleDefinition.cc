#include "TRKParticleDefinition.hh"

#include <cstdlib>
#include <iostream>

TRKParticleDefinition* TRKParticleDefinition::_instance = 0;

TRKParticleDefinition* TRKParticleDefinition::Instance(){
if(_instance==0) {
_instance = new TRKParticleDefinition();
  }
  return _instance;
}

TRKParticleDefinition::TRKParticleDefinition():particleMassCharge(PrepareDefinitions())
{}

std::map<std::string,std::pair<double, int> > TRKParticleDefinition::PrepareDefinitions()
{
  //particle definitions
  
  //from the PDG
  //K.A. Olive et al. (Particle Data Group), Chin. Phys. C, 38, 090001 (2014).
  //http://pdg.lbl.gov/2014/tables/contents_tables.html
  
  //MASS
  const double elementarycharge = CLHEP::e_SI;
  const double electronmass     = 0.510998928*CLHEP::MeV;
  const double protonmass       = 938.272046*CLHEP::MeV;
  const double muonmass         = 105.6583715*CLHEP::MeV;
  const double antielectronmass = electronmass;
  const double antiprotonmass   = protonmass;
  const double antimuonmass     = muonmass;
  const double neutronmass      = 939.565379*CLHEP::MeV;
  const double photonmass       = 0.0;

  //we could potentially get at least the proton, electron and neutrom mass from CLHEP

  //CHARGE normalised to elementary charge
  const int electroncharge     = -1;
  const int protoncharge       =  1;
  const int muoncharge         =  1;
  const int antielectroncharge =  1;
  const int antiprotoncharge   = -1;
  const int antimuoncharge     =  1;
  const int neutroncharge      =  0;
  const int photoncharge       =  0;

  //PACKAGE IT ALL UP - for easy searching and construction
  std::map<std::string,std::pair<double, int> > pmc;

  pmc["electron"]     = std::make_pair (electronmass     ,electroncharge);
  pmc["proton"]       = std::make_pair (protonmass       ,protoncharge);
  pmc["muon"]         = std::make_pair (muonmass         ,muoncharge);
  pmc["antielectron"] = std::make_pair (antielectronmass ,antielectroncharge);
  pmc["antiproton"]   = std::make_pair (antiprotonmass   ,antiprotoncharge);
  pmc["antimuon"]     = std::make_pair (antimuonmass     ,antimuoncharge);
  pmc["neutron"]      = std::make_pair (neutronmass      ,neutroncharge);
  pmc["photon"]       = std::make_pair (photonmass       ,photoncharge);

  return pmc;
}

std::pair<double,int> TRKParticleDefinition::GetParticleMassAndCharge(const std::string particlename)
{
  std::map<std::string, std::pair<double, int> >::iterator it;
  it = particleMassCharge.find(particlename);
  if (it == particleMassCharge.end())
    {
      std::cout << "particle name unrecognised - " << particlename << std::endl;
      exit(1);
    }
  return it->second;
}
