//eBremsstrahlung Leading Particle Biasing Method, M.D. Salt, R.B. Appleby, 15/10/09

#include "BDSeBremsstrahlung_LPB.hh"
#include "CLHEP/Random/Random.h"
#include "BDSGlobalConstants.hh"

G4VParticleChange* eBremsstrahlung_LPB::PostStepDoIt(const G4Track& track, const G4Step& step)
{
  //System to control degree of biasing used
  G4bool fActive = true;
  G4double fBiasFraction = BDSGlobals->GetLPBFraction(); 
  if(fBiasFraction < CLHEP::RandFlat::shoot()) {
    fActive = false;
  }
  
  //Obtain initial parent weight
  G4double initialWeight = track.GetWeight();
  
  //Store for the secondaries
  std::vector<G4Track*> secondaries;
  
  //Particle change declaration
  G4VParticleChange* particleChange = NULL;

  //Get secondaries
  particleChange = pRegProcess->PostStepDoIt(track, step);
  assert (0 != particleChange);
  G4int j(0);

  for(j=0; j<particleChange->GetNumberOfSecondaries(); j++)
  {
    secondaries.push_back(new G4Track(*(particleChange->GetSecondary(j))));
  }
  particleChange->Clear();
  particleChange->SetNumberOfSecondaries(secondaries.size());
  
  //Permit weight adjustment for leading particle biasing
  particleChange->SetParentWeightByProcess(true);
  particleChange->SetSecondaryWeightByProcess(true);
  
  //Add secondaries and perform leading particle biasing
  std::vector<G4Track*>::iterator iter = secondaries.begin();
  G4double survivalProbability(0.0);
  while(iter != secondaries.end())
  {
    G4Track* myTrack = *iter;
    if(fActive)
    {
      survivalProbability = myTrack->GetKineticEnergy() / track.GetKineticEnergy();
      if(survivalProbability > CLHEP::RandFlat::shoot())
      {
        //Parent killed
        myTrack->SetWeight(initialWeight / survivalProbability);
        particleChange->ProposeTrackStatus(fStopAndKill);
      }
      else
      {
        //Secondary Killed
        myTrack->SetTrackStatus(fStopAndKill);
        particleChange->ProposeParentWeight(initialWeight / (1.0 - survivalProbability));
      }
    }
    else
    {
      particleChange->ProposeParentWeight(initialWeight);
      myTrack->SetWeight(initialWeight);
    }
    particleChange->AddSecondary(myTrack);
    iter++;
  }
  return particleChange;
}

