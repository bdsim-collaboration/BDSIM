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
#ifndef BDSPARTICLE_H
#define BDSPARTICLE_H 

#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <ostream>

/**
 * @brief a particle definition
 * 
 * This class keeps information about a particle's parameters.
 * It is mainly used for output
 * 
 * @author Jochem Snuverink
 */

class BDSParticle
{
public:
  BDSParticle();
  BDSParticle(G4double x,
	      G4double y,
	      G4double z,
	      G4double xp,
	      G4double yp,
	      G4double zp,
	      G4double totalEnergyIn,
	      G4double globalTimeIn = 0.0,
	      G4double weightIn     = 1.,
	      G4int    trackIDIn    = -1,
	      G4int    parentIDIn   = -1);
  BDSParticle(G4ThreeVector pos,
	      G4ThreeVector mom,
	      G4double      totalEnergyIn,
	      G4double      globalTimeIn = 0.0,
	      G4double      weightIn     = 1.,
	      G4int         trackIDIn    = -1,
	      G4int         parentIDIn   = -1);

  /// Output stream
  friend std::ostream& operator<< (std::ostream& out, BDSParticle const& p);

private:
  /// position
  G4ThreeVector position;
  
  /// momentum
  G4ThreeVector momentum;
  
  /// energy
  G4double totalEnergy;

  /// time since event began (global time)
  G4double T;

  // optional
  /// weight
  G4double weight;
  
  /// track ID
  G4int trackID;
  
  /// track ID of parent
  G4int parentID;

public:
  G4double GetX() const {return position.x();}
  G4double GetY() const {return position.y();}
  G4double GetZ() const {return position.z();}
  G4double GetXp()const {return momentum.x();}
  G4double GetYp()const {return momentum.y();}
  G4double GetZp()const {return momentum.z();}

  G4double GetTotalEnergy() const {return totalEnergy;}
  G4double GetT()        const {return T;}
  G4double GetWeight()      const {return weight;}
  G4int    GetTrackID()     const {return trackID;}
  G4int    GetParentID()    const {return parentID;}
};

#endif
