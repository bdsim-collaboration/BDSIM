#ifndef BDSParticle_h
#define BDSParticle_h 

// GEANT4 types
#include "G4ThreeVector.hh"
#include "G4Types.hh"

/**
 * @brief a particle definition
 * 
 * This class keeps information about a particle's parameters.
 * It is mainly used for output
 * 
 *
 * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk>
 */

class BDSParticle {

public:
  BDSParticle(G4double x,G4double y,G4double z,G4double xp,G4double yp,G4double zp,G4double E,G4double t,G4double weight,G4int trackID,G4int parentID);
  BDSParticle(G4ThreeVector pos,G4ThreeVector mom,G4double E,G4double t,G4double weight,G4int trackID,G4int parentID);

  /// position, momentum
  G4ThreeVector itsPosition;
private:
  G4ThreeVector itsMomentum;
  /// energy
  G4double itsE;
  /// time since the particle was created
  G4double itsT;
  G4double itsWeight;
  G4int itsTrackID;
  G4int itsParentID;

public:
  G4double GetX(){return itsPosition.x();}
  G4double GetY(){return itsPosition.y();}
  G4double GetZ(){return itsPosition.z();}
  G4double GetXp(){return itsMomentum.x();}
  G4double GetYp(){return itsMomentum.y();}
  G4double GetZp(){return itsMomentum.z();}

  G4double GetEnergy(){return itsE;}
  G4double GetTime(){return itsT;}
  G4double GetWeight(){return itsWeight;}
  
  G4int GetTrackID(){return itsTrackID;}
  G4int GetParentID(){return itsParentID;}
  
};

#endif
