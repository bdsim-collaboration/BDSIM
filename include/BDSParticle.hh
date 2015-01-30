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
  BDSParticle();
  BDSParticle(G4double x,G4double y,G4double z,G4double xp,G4double yp,G4double zp,G4double E,G4double t=0.0,G4double weight=1.,G4int trackID=-1,G4int parentID=-1);
  BDSParticle(G4ThreeVector pos,G4ThreeVector mom,G4double E,G4double t=0.0,G4double weight=1.,G4int trackID=-1,G4int parentID=-1);

private:
  /// position
  G4ThreeVector itsPosition;
  /// momentum
  G4ThreeVector itsMomentum;
  /// energy
  G4double itsE;
  /// time since the particle was created
  G4double itsT;
  // optional
  /// weight
  G4double itsWeight;
  /// track ID
  G4int itsTrackID;
  /// track ID of parent
  G4int itsParentID;


public:
  G4double GetX()const{return itsPosition.x();}
  G4double GetY()const{return itsPosition.y();}
  G4double GetZ()const{return itsPosition.z();}
  G4double GetXp()const{return itsMomentum.x();}
  G4double GetYp()const{return itsMomentum.y();}
  G4double GetZp()const{return itsMomentum.z();}

  G4double GetEnergy()const{return itsE;}
  G4double GetTime()const{return itsT;}
  G4double GetWeight()const{return itsWeight;}
  
  G4int GetTrackID()const{return itsTrackID;}
  G4int GetParentID()const{return itsParentID;}

  G4ThreeVector GetPosition()const{return itsPosition;}
  G4ThreeVector GetDirection()const{return itsMomentum;}
  
};

#endif
