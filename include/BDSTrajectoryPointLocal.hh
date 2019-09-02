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
#ifndef BDSTRAJECTORYPOINTLOCAL_H
#define BDSTRAJECTORYPOINTLOCAL_H

#include "globals.hh"
#include "G4Allocator.hh"

/**
 * @brief Extra information recorded for a single piece of energy deposition.
 *
 * @author Siobhan Alden
 */

class BDSTrajectoryPointLocal
{
public:
  /// Default (in effect) constructor for energy counter hit. The intention (by a lack of
  /// setter methods is that all information should be provided as instantiation time for
  /// an instance of this class.
  BDSTrajectoryPointLocal(G4double xIn,                      // local x
			      G4double yIn,                      // local y
		          G4double zIn,                		 // local z
			      G4double pxIn,                     // local px
		  	      G4double pyIn,					 // local py
		          G4double pzIn);				 // local pz


  ~BDSTrajectoryPointLocal();

  // Local coordinate & Momentum
  G4double 		x;
  G4double 		y;
  G4double 		z;
  G4double 		px;
  G4double 		py;
  G4double 		pz;

private:
  /// Private default constructor.  
  BDSTrajectoryPointLocal() = delete;
};

/*

typedef G4THitsCollection<BDSHitEnergyDepositionExtra> BDSHitsCollectionEnergyDepositionExtra;
extern G4Allocator<BDSHitEnergyDepositionExtra> BDSAllocatorEnergyDepositionExtra;

inline void* BDSHitEnergyDepositionExtra::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSAllocatorEnergyDepositionExtra.MallocSingle();
  return aHit;
}

inline void BDSHitEnergyDepositionExtra::operator delete(void *aHit)
{
 BDSAllocatorEnergyDepositionExtra.FreeSingle((BDSHitEnergyDepositionExtra*) aHit);
}*/

#endif
