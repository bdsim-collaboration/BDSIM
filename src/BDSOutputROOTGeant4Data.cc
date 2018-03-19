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
      
      BDSOutputROOTParticleInfo info = BDSOutputROOTParticleInfo((std::string)particleName,
								 (int)particle->GetPDGCharge(),
								 (double)particle->GetPDGMass());
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
	  BDSOutputROOTParticleInfoIon ionDef = BDSOutputROOTParticleInfoIon((std::string)def->GetParticleName(),
									     (int)def->GetPDGCharge(),
									     (int)def->GetPDGMass(),
									     (int)def->GetAtomicNumber(),
									     (int)def->GetAtomicMass());
	  ions[ion.first] = ionDef;
	}
    }

}
#endif
