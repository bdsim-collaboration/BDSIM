/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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

#include "BDSOutputROOTGeant4Data.hh"

#include <map>

#ifndef __ROOTBUILD__
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleTable.hh"
#include "globals.hh"

#include <string>
#endif

ClassImp(BDSOutputROOTGeant4Data)

BDSOutputROOTGeant4Data::BDSOutputROOTGeant4Data()
{;}

void BDSOutputROOTGeant4Data::Flush()
{
  particles.clear();
  ions.clear();
}

int BDSOutputROOTGeant4Data::Charge(const int& pdgID) const
{
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
	{return result->second.charge;}
      else
	{return 0;}
    }
  else
    {
      auto result = particles.find(pdgID);
      if (result != particles.end())
	{return result->second.charge;}
      else
	{return 0;}
    }
}

double BDSOutputROOTGeant4Data::Mass(const int& pdgID) const
{
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
	{return result->second.mass;}
      else
	{return 0;}
    }
  else
    {
      auto result = particles.find(pdgID);
      if (result != particles.end())
	{return result->second.mass;}
      else
	{return 0;}
    }
}

double BDSOutputROOTGeant4Data::Rigidity(const int&    pdgID,
					 const double& totalEnergy) const
{
  int ch = 0;
  double mass = 0;
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
	{
	  ch = result->second.charge;
	  mass = result->second.mass;
	}
      else
	{return 0;}
    }
  else
    {
      auto result = particles.find(pdgID);
      if (result != particles.end())
	{
	  ch = result->second.charge;
	  mass = result->second.mass;
	}
      else
	{return 0;}
    }

  if (!(std::abs(ch) > std::numeric_limits<double>::epsilon()))
    {return 0;} // not finite charge, so rigidity 0
  if (totalEnergy <= mass)
    {return 0;} // invalid - just return 0
  
  // sqrt(E_t*2 - E_rest*2) in GeV
  double numerator   = std::sqrt(std::pow(totalEnergy,2) - std::pow(mass,2));
  double denominator = ch * 2.99792458e8;
  double brho = numerator / denominator;
  return brho;
}

std::string BDSOutputROOTGeant4Data::Name(const int& pdgID) const
{
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
	{return result->second.name;}
      else
	{return "";}
    }
  else
    {
      auto result = particles.find(pdgID);
      if (result != particles.end())
	{return result->second.name;}
      else
	{return "";}
    }
}

int BDSOutputROOTGeant4Data::IonA(const int& pdgID) const
{
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
	{return result->second.a;}
      else
	{return 0;}
    }
  else
    {return 0;}
}

int BDSOutputROOTGeant4Data::IonZ(const int& pdgID) const
{
  if (IsIon(pdgID))
    {
      auto result = ions.find(pdgID);
      if (result != ions.end())
	{return result->second.z;}
      else
	{return 0;}
    }
  else
    {return 0;}
}

#ifndef __ROOTBUILD__
void BDSOutputROOTGeant4Data::Fill(const G4bool& fillIons)
{
  G4ParticleTable* pt = G4ParticleTable::GetParticleTable();

  // iterator over particles - non-standard iterator type
  auto it = pt->GetIterator();
  it->reset();
  while ((*it)() )
    {
      const G4ParticleDefinition* particle = it->value();
      const G4String& particleName = particle->GetParticleName();

      int pdgID = static_cast<int>(particle->GetPDGEncoding());

      BDSOutputROOTGeant4Data::ParticleInfo info = {(std::string)particleName,
						    (int)particle->GetPDGCharge(),
						    (double)particle->GetPDGMass()/CLHEP::GeV};
      particles[pdgID] = info;
    }

  if (fillIons)
    {// proceed to fill ion information as it was used in the simulation
      G4IonTable* ionTable = pt->GetIonTable();

      // G4IonTable has no iterator - can only query for specific ions
      // G4IonList is type def of std::multimap<G4int, const G4ParticleDefinition*>
      // Thankfully has static public holder of info.
      G4IonTable::G4IonList* ionList = ionTable->fIonList;
      for (const auto& ion : *ionList)
	{
	  const G4ParticleDefinition* def = ion.second;

	  BDSOutputROOTGeant4Data::IonInfo ionDef = {(std::string)def->GetParticleName(),
						     (int)def->GetPDGCharge(),
						     (double)def->GetPDGMass()/CLHEP::GeV,
						     (int)def->GetAtomicNumber(),
						     (int)def->GetAtomicMass()};
	  ions[ion.first] = ionDef;
	}
    }

}
#endif
