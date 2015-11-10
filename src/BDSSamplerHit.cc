#include "BDSSamplerHit.hh"
#include "G4ios.hh"

G4Allocator<BDSSamplerHit> BDSSamplerHitAllocator;

BDSSamplerHit::BDSSamplerHit(G4String    aName,
			     BDSParticle init,
			     BDSParticle prod,
			     BDSParticle last_scat,
			     BDSParticle local,
			     BDSParticle global,
			     G4double    s,
			     G4double    weight,
			     G4int       PDGtype,
			     G4int       nEvent,
			     G4int       ParentID,
			     G4int       TrackID,
			     G4int       TurnsTaken,
			     G4String    sampType,
			     G4String    process):
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
  itsSampType(sampType),
  itsProcess(process)
{;}

BDSSamplerHit::~BDSSamplerHit()
{;}
