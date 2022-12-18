#ifndef BDSMuonFluxEnhancementTrackInformation_hh
#define BDSMuonFluxEnhancementTrackInformation_hh 1

#include "G4Track.hh"
#include "G4VUserTrackInformation.hh"

enum MuonFluxTrackType{
  kOriginalTrack,
  kCloneTrack
};

class BDSMuonFluxEnhancementTrackInformation : public G4VUserTrackInformation
{
public:
  BDSMuonFluxEnhancementTrackInformation();
  BDSMuonFluxEnhancementTrackInformation(const BDSMuonFluxEnhancementTrackInformation &info);
  
  ~BDSMuonFluxEnhancementTrackInformation() {};

  inline void *operator new(size_t);
  inline void operator delete(void *info);
  
  void Print() const {};

public:
  G4bool GetIsAllowedInelastic() const {return fIsAllowedInelastic;}
  void SetIsAllowedInelastic(G4bool val) {fIsAllowedInelastic = val;}

  G4int GetParentPDGid() const {return fParentPDGid;}
  void SetParentPDGid(G4int val) {fParentPDGid = val;}
  
  MuonFluxTrackType GetTrackType() const {return fTrackType;}
  void SetTrackType(MuonFluxTrackType val) {fTrackType = val;}

  G4bool GetHasClone() const {return fHasClone;}
  void SetHasClone(G4bool val) {fHasClone = val;}

private:
  G4bool fIsAllowedInelastic;
  G4int fParentPDGid;
  MuonFluxTrackType fTrackType;
  G4bool fHasClone;

};

extern G4ThreadLocal
 G4Allocator<BDSMuonFluxEnhancementTrackInformation> * aTrackInformationAllocator;

inline void* BDSMuonFluxEnhancementTrackInformation::operator new(size_t)
{
  if (!aTrackInformationAllocator)
    aTrackInformationAllocator = new G4Allocator<BDSMuonFluxEnhancementTrackInformation>;
  return (void*)aTrackInformationAllocator->MallocSingle();
}

inline void BDSMuonFluxEnhancementTrackInformation::operator delete(void *info){
  aTrackInformationAllocator->FreeSingle((BDSMuonFluxEnhancementTrackInformation*) info);
}


#endif