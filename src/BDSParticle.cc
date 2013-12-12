#include "BDSParticle.hh"

BDSParticle::BDSParticle(G4double x,G4double y,G4double z,G4double xp,G4double yp,G4double zp,G4double E,G4double t,G4double weight,G4int trackID,G4int parentID):itsPosition(x,y,z),itsMomentum(xp,yp,zp),itsE(E),itsT(t),itsWeight(weight),itsTrackID(trackID),itsParentID(parentID)
{
}

BDSParticle::BDSParticle(G4ThreeVector pos,G4ThreeVector mom,G4double E,G4double t,G4double weight,G4int trackID,G4int parentID):itsPosition(pos),itsMomentum(mom),itsE(E),itsT(t),itsWeight(weight),itsTrackID(trackID),itsParentID(parentID)
{
}
