//GammaConversion Leading Particle Biasing Method, M.D. Salt, R.B. Appleby, 15/10/09

#include "BDSGammaConversion_LPB.hh"
#include "CLHEP/Random/Random.h"
#include "BDSGlobalConstants.hh"

G4VParticleChange* GammaConversion_LPB::PostStepDoIt(const G4Track& track, const G4Step& step)
{
  //System to control degree of biasing used
  G4bool fActive = true;
  G4double fBiasFraction = BDSGlobals->GetLPBFraction();
  if(fBiasFraction < CLHEP::RandFlat::shoot()){
    fActive = false;
  }

  //Obtain initial parent weight
  G4double initialWeight = track.GetWeight();

  //Secondaries store
  std::vector<G4Track*> secondaries;

  //Declare particle change
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

  //Permit weight setting for leading particle biasing
  particleChange->SetSecondaryWeightByProcess(true);

  //Add the secondaries and perform leading particle biasing
  std::vector<G4Track*>::iterator iter = secondaries.begin();
  G4double survivalProbability(0.0); //relates to the first [0] secondary
  survivalProbability = secondaries[0]->GetKineticEnergy() / track.GetKineticEnergy();
  if(fActive)
  {
    if(survivalProbability > CLHEP::RandFlat::shoot())
    {
      //secondaries[0] survives
      secondaries[0]->SetWeight(initialWeight / survivalProbability);
      secondaries[1]->SetTrackStatus(fStopAndKill);
    }
    else
    {
      //secondaries[1] survives
      secondaries[0]->SetTrackStatus(fStopAndKill);
      secondaries[1]->SetWeight(initialWeight / (1.0 - survivalProbability));
    }
  }
  else
  {
    secondaries[0]->SetWeight(initialWeight);
    secondaries[1]->SetWeight(initialWeight);
  }
  while(iter != secondaries.end())
  {
    G4Track* myTrack = *iter;
    particleChange->AddSecondary(myTrack);
    iter++;
  }
  //Kill the incident photon
  particleChange->ProposeTrackStatus(fStopAndKill);
  return particleChange;
}
