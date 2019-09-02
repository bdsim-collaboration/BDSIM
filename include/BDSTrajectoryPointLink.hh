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
#ifndef BDSTRAJECTORYPOINTLINK_H
#define BDSTRAJECTORYPOINTLINK_H

#include "globals.hh"
#include "G4Allocator.hh"

/**
 * @brief Extra information recorded for a single piece of energy deposition.
 *
 * @author Siobhan Alden
 */

class BDSTrajectoryPointLink
{
public:
  /// Default (in effect) constructor for energy counter hit. The intention (by a lack of
  /// setter methods is that all information should be provided as instantiation time for
  /// an instance of this class.
  BDSTrajectoryPointLink(
		          G4int    chargeIn,					 // charge
		          G4double kineticEnergyIn,		     // kinetic energy
			      G4int    turnsTakenIn,             // turns taken if circular
		          G4double massIn,					 // mass
		  		  G4double rigidityIn			 // rigidity
  );

  ~BDSTrajectoryPointLink();

  // links
  G4int 	charge;
  G4double 	kineticEnergy;
  G4int     turnsTaken;
  G4double 	mass;
  G4double  rigidity;


private:
  /// Private default constructor.  
  BDSTrajectoryPointLink() = delete;
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
