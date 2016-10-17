#include "BDSTunnelHit.hh"

#include "globals.hh" // geant4 types / globals

G4Allocator<BDSTunnelHit> BDSTunnelHitAllocator;

BDSTunnelHit::BDSTunnelHit(G4int    nCopyIn, 
			   G4double energyIn, 
			   G4double XIn, 
			   G4double YIn, 
			   G4double ZIn,
			   G4double SIn,
			   G4double xIn,
			   G4double yIn,
			   G4double zIn,
			   G4double rIn,
			   G4double thetaIn,
			   G4int    partIDIn, 
			   G4double weightIn, 
			   G4bool   precisionRegionIn, 
			   G4int    turnsTakenIn,
			   G4int    eventNoIn):
  energy(energyIn),
  X(XIn),
  Y(YIn),
  Z(ZIn),
  S(SIn),
  x(xIn),
  y(yIn),
  z(zIn),
  r(rIn),
  theta(thetaIn),
  weight(weightIn),
  copyNumber(nCopyIn),
  partID(partIDIn),
  precisionRegion(precisionRegionIn),
  turnsTaken(turnsTakenIn),
  eventNo(eventNoIn)
{;}

BDSTunnelHit::~BDSTunnelHit()
{;}
