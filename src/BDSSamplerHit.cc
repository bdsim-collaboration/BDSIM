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
		BDSParticle init,
		BDSParticle prod,
		BDSParticle last_scat,
		BDSParticle local,
		BDSParticle global,
		G4double s,
		G4double weight,
		G4int PDGtype, G4int nEvent,
		G4int ParentID, G4int TrackID,
		G4int TurnsTaken,
		G4String sampType):
  itsName(aName),
  itsInit(init),
  itsProd(prod),
  itsLastScat(last_scat),
  itsLocal(local),
  itsGlobal(global),
  itsS(s),
  itsWeight(weight),
  itsPDGtype(PDGtype),
  itsEventNo(nEvent),
  itsParentID(ParentID),
  itsTrackID(TrackID),
  itsTurnsTaken(TurnsTaken),
  itsSampType(sampType)
{;}

BDSSamplerHit::~BDSSamplerHit()
{;}
