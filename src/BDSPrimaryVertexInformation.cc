/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include "BDSParticleCoordsFullGlobal.hh"
#include "BDSParticleDefinition.hh"
#include "BDSPrimaryVertexInformation.hh"

#include "globals.hh"
#include "G4ParticleDefinition.hh"

#include "CLHEP/Units/PhysicalConstants.h"

BDSPrimaryVertexInformation:: BDSPrimaryVertexInformation(const BDSParticleCoordsFullGlobal& primaryVertexIn,
							  const BDSParticleDefinition*       particle,
							  const G4int                        nElectronsIn):
  primaryVertex(primaryVertexIn),
  charge(particle->Charge()),
  rigidity(particle->BRho()),
  mass(particle->Mass()),
  pdgID(particle->ParticleDefinition()->GetPDGEncoding()),
  nElectrons(nElectronsIn)
{;}

void BDSPrimaryVertexInformation::Print() const
{
  G4cout << primaryVertex << G4endl;
  G4cout << "Primary particle charge " << charge / CLHEP::eplus << G4endl;
}
