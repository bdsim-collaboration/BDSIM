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
	      G4double tIn        = 0.0,
	      G4double weightIn   = 1.,
	      G4int    trackIDIn  = -1,
	      G4int    parentIDIn = -1);
  BDSParticle(G4ThreeVector pos,
	      G4ThreeVector mom,
	      G4double      totalEnergyIn,
	      G4double      tIn        = 0.0,
	      G4double      weightIn   = 1.,
	      G4int         trackIDIn  = -1,
	      G4int         parentIDIn = -1);

  /// Output stream
  friend std::ostream& operator<< (std::ostream& out, BDSParticle const& p);

private:
  /// position
  G4ThreeVector position;
  
  /// momentum
  G4ThreeVector momentum;
  
  /// energy
  G4double totalEnergy;
  
  /// time since the particle was created
  G4double t;
  
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
  G4double GetTime()        const {return t;}
  G4double GetWeight()      const {return weight;}
  G4int    GetTrackID()     const {return trackID;}
  G4int    GetParentID()    const {return parentID;}
};

#endif
