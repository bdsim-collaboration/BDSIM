#include "BDSBOptnSplitAtCreation.hh"
#include "TrackInformation.hh"

BDSBOptnSplitAtCreation::BDSBOptnSplitAtCreation(G4String name) :
G4VBiasingOperation(name), fParticleChange(), fParticleChangeForNothing()
{}

BDSBOptnSplitAtCreation::~BDSBOptnSplitAtCreation(){}

G4double BDSBOptnSplitAtCreation::DistanceToApplyOperation(const G4Track *track, G4double, G4ForceCondition *condition)
{
  TrackInformation *info = static_cast<TrackInformation*>(track->GetUserInformation());
  
  // Assume that we do not clone
  *condition = NotForced;

  // if track is original (meson or gamma) and does not have a clone, clone it
  G4int pdgID = track->GetParticleDefinition()->GetPDGEncoding();

  if (info->GetTrackType() == 0){
    if (abs(pdgID) != 13 && !info->GetHasClone()){
      *condition = Forced;
    }
    else if (abs(pdgID) == 13){
      *condition = Forced;
    }
  }
}

G4VParticleChange *BDSBOptnSplitAtCreation::GenerateBiasingFinalState(const G4Track *track, const G4Step*){
  TrackInformation *info = static_cast<TrackInformation*> (track->GetUserInformation());
  G4int pdgID = track->GetParticleDefinition()->GetPDGEncoding();
  G4double weight = track->GetWeight();
  if (info->GetTrackType() == 0 && !info->GetHasClone() && track->GetCurrentStepNumber() == 1){
    SplitMesonAtCreation(track);
  }
  else if (abs(pdgID) == 13 && info->GetTrackType() == 0){
    // kill muons coming from non-clones
    fParticleChange.Initialize(*track);
    fParticleChange.SetSecondaryWeightByProcess(true);
    fParticleChange.ProposeTrackStatus(fKillTrackAndSecondaries);
  }
  else{
    fParticleChangeForNothing.Initialize(*track);
    return &fParticleChangeForNothing;
  }
  return &fParticleChange;
}

void BDSBOptnSplitAtCreation::SplitMesonAtCreation(const G4Track *track){
  fParticleChange.Initialize(*track);
  fParticleChange.SetSecondaryWeightByProcess(true);
  fParticleChange.SetNumberOfSecondaries(1);

  TrackInformation *infoOriginal = static_cast<TrackInformation*> (track->GetUserInformation());

  G4ThreeVector vertexPosition = track->GetVertexPosition();
  G4ThreeVector vertexMomentumDirection = track->GetVertexMomentumDirection();
  G4DynamicParticle *dyn = new G4DynamicParticle(track->GetParticleDefinition(), vertexMomentumDirection, track->GetVertexKineticEnergy());
  fParticleChange.AddSecondary(new G4Track(dyn, track->GetGlobalTime(), vertexPosition));

  G4Track *clone = fParticleChange.GetSecondary(0);
  TrackInformation *infoClone = new TrackInformation();
  infoClone->SetParentPDGid(track->GetParticleDefinition()->GetPDGEncoding());
  infoClone->SetIsAllowedInelastic(false);
  infoClone->SetHasClone(true);
  infoClone->SetTrackType(1);

  clone->SetUserInformation(infoClone);

  infoOriginal->SetHasClone(true);
}

