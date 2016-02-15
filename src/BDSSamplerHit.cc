#include "BDSSamplerHit.hh"
#include "G4ios.hh"

G4Allocator<BDSSamplerHit> BDSSamplerHitAllocator;

BDSSamplerHit::BDSSamplerHit(G4String    aName,
			     G4int       samplerID,
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
  itsSamplerID(samplerID),
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

std::ostream& operator<< (std::ostream& out, BDSSamplerHit const& h)
{
  out << "Sampler: \"" << h.GetName() << "\", ID: " << h.GetSamplerID() << G4endl;
  out << "Local point:  " << h.GetLocal();
  out << "Global point: " << h.GetGlobal();
  out << "S, weight, PDGID, Event#, ParentID, TrackID, Turns:" << G4endl;
  out << h.GetS()          << ", "
      << h.GetWeight()     << ", "
      << h.GetPDGtype()    << ", "
      << h.GetEventNo()    << ", "
      << h.GetParentID()   << ", "
      << h.GetTrackID()    << ", "
      << h.GetTurnsTaken() << G4endl;
  return out;
}
