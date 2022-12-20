#include "BDSBOptnSplitAtCreation.hh"
#include "BDSMuonFluxEnhancementTrackInformation.hh"
#include "BDSGlobalConstants.hh"

BDSBOptnSplitAtCreation::BDSBOptnSplitAtCreation(G4String name) :
G4VBiasingOperation(name), fParticleChange(), fParticleChangeForNothing(),
fMinWeight(BDSGlobalConstants::Instance()->MuonFluxBiasingMinWeight()),
fMaxWeight(BDSGlobalConstants::Instance()->MuonFluxBiasingMaxWeight())
{
  fShouldSplitHighWeightParticles = fMaxWeight > 0.;
  fShouldRussianRoulette = (fMinWeight > 0. && fMaxWeight > 0. && fMaxWeight > fMinWeight);
}

BDSBOptnSplitAtCreation::~BDSBOptnSplitAtCreation(){}

G4double BDSBOptnSplitAtCreation::DistanceToApplyOperation(const G4Track *track, G4double, G4ForceCondition *condition)
{
  BDSMuonFluxEnhancementTrackInformation *info = static_cast<BDSMuonFluxEnhancementTrackInformation*>(track->GetUserInformation());
  
  // Assume that we do not clone
  *condition = NotForced;

  // if track is original (meson or gamma) and does not have a clone, clone it
  G4int pdgID = track->GetParticleDefinition()->GetPDGEncoding();

  if (info->GetTrackType() == kOriginalTrack){
    if (abs(pdgID) != 13 && !info->GetHasClone()){// always clone original mesons/gammas
      *condition = Forced;
    }
    else if (abs(pdgID) == 13){ // always kill original muons
      *condition = Forced;
    }
  }
  else
  {
    // here we have a clone.
    // apply split/RR if muon is outside window
    
    if (abs(pdgID) == 13 &&
        (fShouldRussianRoulette || fShouldSplitHighWeightParticles))
    { 
      *condition = Forced;
    }
    
    if (abs(pdgID) != 13 && fShouldRussianRoulette)
    { 
      *condition = Forced;
    }
  } 
  return DBL_MAX;
}

G4VParticleChange *BDSBOptnSplitAtCreation::GenerateBiasingFinalState(const G4Track *track, const G4Step*){
  BDSMuonFluxEnhancementTrackInformation *info = static_cast<BDSMuonFluxEnhancementTrackInformation*> (track->GetUserInformation());
  G4int pdgID = track->GetParticleDefinition()->GetPDGEncoding();
  if (abs(pdgID) != 13 &&
      info->GetTrackType() == kOriginalTrack && 
      !info->GetHasClone() && 
      track->GetCurrentStepNumber() == 1)
  { // original mesons/gamma are always cloned at creation since they always have weight=1
    SplitMesonAtCreation(track);
  }
  else if (abs(pdgID) != 13 && 
           info->GetTrackType() == kCloneTrack && 
           track->GetWeight() < fMinWeight && 
           fShouldRussianRoulette)
  { // clone mesons/gamma are russian roulette'd only if below weight threshold
    PlayRussianRoulette(track);
  }
  else if (abs(pdgID) == 13 && 
           info->GetTrackType() == kOriginalTrack)
  {// kill original muons
    fParticleChange.Initialize(*track);
    fParticleChange.SetSecondaryWeightByProcess(true);
    fParticleChange.ProposeTrackStatus(fKillTrackAndSecondaries);
  }
  else if (abs(pdgID) == 13 &&
           info->GetTrackType() == kCloneTrack)
  {
    if (track->GetWeight() > fMaxWeight && fShouldSplitHighWeightParticles)
    {SplitHighWeightMuon(track);}
    else if (track->GetWeight() < fMinWeight && fShouldRussianRoulette)
    {PlayRussianRoulette(track);}
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

  BDSMuonFluxEnhancementTrackInformation *infoOriginal = static_cast<BDSMuonFluxEnhancementTrackInformation*> (track->GetUserInformation());

  G4ThreeVector vertexPosition = track->GetVertexPosition();
  G4ThreeVector vertexMomentumDirection = track->GetVertexMomentumDirection();
  G4DynamicParticle *dyn = new G4DynamicParticle(track->GetParticleDefinition(), vertexMomentumDirection, track->GetVertexKineticEnergy());
  fParticleChange.AddSecondary(new G4Track(dyn, track->GetGlobalTime(), vertexPosition));

  G4Track *clone = fParticleChange.GetSecondary(0);
  BDSMuonFluxEnhancementTrackInformation *infoClone = new BDSMuonFluxEnhancementTrackInformation();
  infoClone->SetParentPDGid(track->GetParticleDefinition()->GetPDGEncoding());
  infoClone->SetIsAllowedInelastic(false);
  infoClone->SetHasClone(true);
  infoClone->SetTrackType(kCloneTrack);

  clone->SetUserInformation(infoClone);

  infoOriginal->SetHasClone(true);
}

void BDSBOptnSplitAtCreation::SplitHighWeightMuon(const G4Track *track)
{
  fParticleChange.Initialize(*track);
  
  BDSMuonFluxEnhancementTrackInformation *infoOriginal = static_cast<BDSMuonFluxEnhancementTrackInformation*> (track->GetUserInformation());

  G4double weight = track->GetWeight();
  G4int nClones = (int)(weight / fMaxWeight);
  G4double finalWeight = weight / nClones;

  fParticleChange.ProposeParentWeight(finalWeight);
  fParticleChange.SetNumberOfSecondaries(nClones);

  G4ThreeVector vertexPos = track->GetVertexPosition();
  G4ThreeVector vertexMomDir = track->GetVertexMomentumDirection();

  for (G4int iClone = 1; iClone < nClones; iClone++)
  {
    G4DynamicParticle *dyn = new G4DynamicParticle(track->GetParticleDefinition(),
                                                   vertexMomDir, track->GetVertexKineticEnergy());

    fParticleChange.AddSecondary(new G4Track(dyn, track->GetGlobalTime(), vertexPos));

    G4Track *clone = fParticleChange.GetSecondary(iClone - 1);

    clone->SetWeight(finalWeight);

    BDSMuonFluxEnhancementTrackInformation *infoClone = new BDSMuonFluxEnhancementTrackInformation(*infoOriginal);
    infoClone->SetHasClone(true);
    
    clone->SetUserInformation(infoClone);
  }
  fParticleChange.SetSecondaryWeightByProcess(true);
}

#include "G4RandomTools.hh"

void BDSBOptnSplitAtCreation::PlayRussianRoulette(const G4Track *track)
{
  fParticleChange.Initialize(*track);
  fParticleChange.SetSecondaryWeightByProcess(true);
  // fParticleChange.SetNumberOfSecondaries(0);

  G4double weight = track->GetWeight();
  G4double killingProb = 1.0 - weight / (0.5 * (fMinWeight + fMaxWeight));

  if (G4RandFlat::shoot() < killingProb)
  {
    fParticleChange.ProposeTrackStatus(fKillTrackAndSecondaries);
  }
  else
  {
    fParticleChange.ProposeParentWeight(0.5 * (fMinWeight + fMaxWeight));
  }
}

