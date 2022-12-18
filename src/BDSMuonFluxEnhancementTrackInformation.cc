#include "BDSMuonFluxEnhancementTrackInformation.hh"

G4ThreadLocal G4Allocator<BDSMuonFluxEnhancementTrackInformation> * aTrackInformationAllocator = 0;

BDSMuonFluxEnhancementTrackInformation::BDSMuonFluxEnhancementTrackInformation(): 
G4VUserTrackInformation("muonFluxEnhancement"),
fIsAllowedInelastic(true),fParentPDGid(0), fTrackType(kOriginalTrack), fHasClone(false)
{}

BDSMuonFluxEnhancementTrackInformation::BDSMuonFluxEnhancementTrackInformation(const BDSMuonFluxEnhancementTrackInformation &info): 
G4VUserTrackInformation("muonFluxEnhancement"),
fIsAllowedInelastic(info.fIsAllowedInelastic), fParentPDGid(info.fParentPDGid), fTrackType(info.fTrackType), fHasClone(info.fHasClone)
{}
