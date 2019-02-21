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
#ifndef BDSHITENERGYDEPOSITION_H
#define BDSHITENERGYDEPOSITION_H

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"

class BDSHitEnergyDeposition;

typedef G4THitsCollection<BDSHitEnergyDeposition> BDSHitsCollectionEnergyDeposition;
extern G4Allocator<BDSHitEnergyDeposition> BDSAllocatorEnergyDeposition;

/**
 * @brief Information recorded for a single piece of energy deposition.
 *
 */

class BDSHitEnergyDeposition: public G4VHit
{
public:
  /// Default (in effect) constructor for energy counter hit. The intention (by a lack of
  /// setter methods is that all information should be provided as instantiation time for
  /// an instance of this class.
  BDSHitEnergyDeposition(G4double energyIn          = 0,    // energy in this 'hit'
			 G4double preStepKineticEnergy = 0, // pre step point kinetic energy
			 G4double XIn               = 0,    // global x
			 G4double YIn               = 0,    // global y
			 G4double ZIn               = 0,    // global x
			 G4double SHitIn            = 0,    // assigned s of loss
			 G4double xIn               = 0,    // local x
			 G4double yIn               = 0,    // local y
			 G4double zIn               = 0,    // local z
			 G4double globalTimeIn      = 0,    // global time
			 G4int    partIDIn          = 0,    // PDG id - particle type
			 G4int    trackID           = -1,   // Track ID
			 G4int    parentID          = -1,   // Parent ID
			 G4double weightIn          = 1,    // weight
			 G4int    turnsTakenIn      = 1,    // turns taken if circular
			 G4double stepLengthIn      = 0,
			 G4int    beamlineIndexIn   = -1);
  
  virtual ~BDSHitEnergyDeposition();
  
  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);

  inline G4double GetEnergy()          const {return energy;}
  inline G4double GetPreStepKineticEnergy() const {return preStepKineticEnergy;}
  inline G4double GetX()               const {return X;} 
  inline G4double GetY()               const {return Y;}
  inline G4double GetZ()               const {return Z;}
  inline G4double GetSHit()            const {return sHit;}
  inline G4double Getx()               const {return x;} 
  inline G4double Gety()               const {return y;}
  inline G4double Getz()               const {return z;} 
  inline G4double GetGlobalTime()      const {return globalTime;}
  inline G4int    GetPartID()          const {return partID;}
  inline G4int    GetTrackID()         const {return trackID;}
  inline G4int    GetParentID()        const {return parentID;}
  inline G4double GetWeight()          const {return weight;} 
  inline G4int    GetTurnsTaken()      const {return turnsTaken;}
  inline G4double GetStepLength()      const {return stepLength;}
  inline G4int    GetBeamlineIndex()   const {return beamlineIndex;}
  inline G4double GetEnergyWeighted()  const {return weight * energy;}
  
private:
  /// Private default constructor (not implemented) as the constructor
  /// provided as defaults for everything which in effect provides a default
  /// constructor
  BDSHitEnergyDeposition();

  G4double energy;
  G4double preStepKineticEnergy;
  
  /// @{ Global coordinate
  G4double X;
  G4double Y;
  G4double Z;
  /// @}

  /// The position this energy deposition is assigned to. As we represent
  /// a Monte Carlo of discrete processes, this is randomly scattered along
  /// the length of the track this energy deposition corresponds to (uniform
  /// random distribution).
  G4double sHit;
  
  /// @{ Local coordinate
  G4double x;
  G4double y;
  G4double z;
  /// @}

  G4double globalTime; ///< Time since start of event
  G4int    partID;
  G4int    trackID;
  G4int    parentID;
  G4double weight;
  G4int    turnsTaken;
  G4double stepLength;
  G4int    beamlineIndex;
};

inline void* BDSHitEnergyDeposition::operator new(size_t)
{
  void* aHit;
  aHit=(void*) BDSAllocatorEnergyDeposition.MallocSingle();
  return aHit;
}

inline void BDSHitEnergyDeposition::operator delete(void *aHit)
{
 BDSAllocatorEnergyDeposition.FreeSingle((BDSHitEnergyDeposition*) aHit);
}

#endif
