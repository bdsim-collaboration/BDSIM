/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSParticleCoordsFull.hh"
#include "BDSHitSamplerLink.hh"

#include "G4Allocator.hh"
#include "G4Types.hh"

G4Allocator<BDSHitSamplerLink> BDSAllocatorSamplerLink;

BDSHitSamplerLink::BDSHitSamplerLink(G4int samplerIDIn,
				     const BDSParticleCoordsFull& coordsIn,
				     G4double momentumIn,
				     G4double massIn,
				     G4int    ZIn,
				     G4int    AIn,
				     G4double chargeIn,
				     G4int    pdgIDIn,
				     G4int    parentIDIn,
				     G4int    trackIDIn,
				     G4int    nElectronsIn):
  samplerID(samplerIDIn),
  coords(coordsIn),
  momentum(momentumIn),
  mass(massIn),
  Z(ZIn),
  A(AIn),
  charge(chargeIn),
  pdgID(pdgIDIn),
  parentID(parentIDIn),
  trackID(trackIDIn),
  nElectrons(nElectronsIn),
  eventID(0)
{;}

BDSHitSamplerLink::~BDSHitSamplerLink()
{;}
