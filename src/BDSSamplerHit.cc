/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 

   Modified 22.03.05 by J.C.Carter, Royal Holloway, Univ. of London.
   Changed Samplers to account for plane and cylinder types (GABs code)
*/

#include "BDSSamplerHit.hh"
#include "G4ios.hh"

G4Allocator<BDSSamplerHit> BDSSamplerHitAllocator;

BDSSamplerHit::BDSSamplerHit(G4String aName,
			     G4double init_mom,
			     G4double init_x, G4double init_xPrime,
			     G4double init_y, G4double init_yPrime,
			     G4double init_z, G4double init_zPrime,
			     G4double init_t,
			     G4double mom,
			     G4double x, G4double xPrime,
			     G4double y, G4double yPrime,
			     G4double z, G4double zPrime,
			     G4double t,
			     G4double s,
			     G4double weight,
			     G4int PDGtype, G4int nEvent,
			     G4int ParentID, G4int TrackID,
			     G4int TurnsTaken)
:itsName(aName),
  itsInit_Mom(init_mom),
  itsInit_X(init_x), itsInit_XPrime(init_xPrime),
  itsInit_Y(init_y), itsInit_YPrime(init_yPrime),
  itsInit_Z(init_z), itsInit_ZPrime(init_zPrime),
  itsInit_T(init_t),
  itsMom(mom),
  itsX(x), itsXPrime(xPrime),
  itsY(y), itsYPrime(yPrime),
  itsZ(z), itsZPrime(zPrime),
  itsGlobalX(0.0), itsGlobalXPrime(0.0),
  itsGlobalY(0.0), itsGlobalYPrime(0.0),
  itsGlobalZ(0.0), itsGlobalZPrime(0.0),
  itsT(t),
  itsS(s),
  itsWeight(weight),
  itsPDGtype(PDGtype),
  itsEventNo(nEvent),
  itsParentID(ParentID),
  itsTrackID(TrackID),
  itsTurnsTaken(TurnsTaken)
{;}

BDSSamplerHit::~BDSSamplerHit()
{;}
