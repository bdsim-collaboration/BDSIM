/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
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

TRKParticleDefinition::TRKParticleDefinition()
{
  PrepareDefinitions();
}

void TRKParticleDefinition::PrepareDefinitions()
{
  //particle definitions
  
  //from the PDG
  //K.A. Olive et al. (Particle Data Group), Chin. Phys. C, 38, 090001 (2014).
  //http://pdg.lbl.gov/2014/tables/contents_tables.html
  
  //MASS in MeV
  //  const double elementarycharge = CLHEP::e_SI;
  const double electronmass     = 0.510998928;
  const double protonmass       = 938.272046;
  const double muonmass         = 105.6583715;
  const double antielectronmass = electronmass;
  const double antiprotonmass   = protonmass;
  const double antimuonmass     = muonmass;
  const double neutronmass      = 939.565379;
  const double photonmass       = 0.0;

  //we could potentially get at least the proton, electron and neutrom mass from CLHEP

  //CHARGE normalised to elementary charge
  const int electroncharge     = -1;
  const int protoncharge       =  1;
  const int muoncharge         = -1;
  const int antielectroncharge =  1;
  const int antiprotoncharge   = -1;
  const int antimuoncharge     =  1;
  const int neutroncharge      =  0;
  const int photoncharge       =  0;

  //PACKAGE IT ALL UP - for easy searching and construction

  particleMassCharge["electron"]     = std::make_pair (electronmass     ,electroncharge);
  particleMassCharge["proton"]       = std::make_pair (protonmass       ,protoncharge);
  particleMassCharge["muon"]         = std::make_pair (muonmass         ,muoncharge);
  particleMassCharge["antielectron"] = std::make_pair (antielectronmass ,antielectroncharge);
  particleMassCharge["antiproton"]   = std::make_pair (antiprotonmass   ,antiprotoncharge);
  particleMassCharge["antimuon"]     = std::make_pair (antimuonmass     ,antimuoncharge);
  particleMassCharge["neutron"]      = std::make_pair (neutronmass      ,neutroncharge);
  particleMassCharge["photon"]       = std::make_pair (photonmass       ,photoncharge);

  // Aliases

  particleMassCharge["e-"] = particleMassCharge["electron"];
  particleMassCharge["e+"] = particleMassCharge["antielectron"];
  particleMassCharge["positron"] = particleMassCharge["antielectron"];
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

double TRKParticleDefinition::GetParticleMass(const std::string particlename)
{
  std::map<std::string, std::pair<double, int> >::iterator it;
  it = particleMassCharge.find(particlename);
  if (it == particleMassCharge.end())
    {
      std::cout << "particle name unrecognised - " << particlename << std::endl;
      exit(1);
    }
  return it->second.first;
}

int TRKParticleDefinition::GetParticleCharge(const std::string particlename)
{
  std::map<std::string, std::pair<double, int> >::iterator it;
  it = particleMassCharge.find(particlename);
  if (it == particleMassCharge.end())
    {
      std::cout << "particle name unrecognised - " << particlename << std::endl;
      exit(1);
    }
  return it->second.second;
}
