//BackScatterBias - particles being backscattered are split

#include "BDSBackScatterBias.hh"
#include "G4SteppingManager.hh"


BDSBackScatterBias::BDSBackScatterBias(const G4String& aName,
                                         G4ProcessType   aType)
  : G4WrapperProcess(aName, aType), eFactor(1.0)
{
}

BDSBackScatterBias::BDSBackScatterBias(const BDSBackScatterBias& right)
  : G4WrapperProcess(right), eFactor(right.eFactor)
{
}

BDSBackScatterBias::~BDSBackScatterBias()
{
}



G4VParticleChange* BDSBackScatterBias::PostStepDoIt(
                                                   const G4Track& track,
                                                   const G4Step&  stepData
                                                   )
{
#ifdef DEBUG
  G4cout <<" ###PostStepDoIt " << G4endl;
  G4cout << "BDSBackScatterBias::PostStepDoit  Getting pChange" << G4endl;
#endif
  G4VParticleChange* pChange = pRegProcess->PostStepDoIt( track, stepData );
  pChange->SetVerboseLevel(0);
#ifdef DEBUG
  G4cout << "BDSBackScatterBias::PostStepDoit Choosing setsecondaryweightbyprocess" << G4endl;
#endif
  pChange->SetSecondaryWeightByProcess(true);
  pChange->SetParentWeightByProcess(true);
#ifdef DEBUG
  G4cout << "BDSBackScatterBias::PostStepDoit Getting parent weight" << G4endl;
#endif
  G4double w =  pChange->GetParentWeight();
  G4double ws = w / eFactor;
  //  G4double survivalProb = w - ws;
  
#ifdef DEBUG
  G4cout << "BDSBackScatterBias::PostStepDoit Getting number of secondaries" << G4endl;
#endif
  G4int iNSec = pChange->GetNumberOfSecondaries();

#ifdef DEBUG  
  G4cout << "BDSBackScatterBias::PostStepDoit Setting secondary weights" << G4endl;
#endif

  for (G4int i = 0; i < iNSec; i++) {
    pChange->GetSecondary(i)->SetWeight(ws); 
  }
  
#ifdef DEBUG  
  G4cout << "BDSBackScatterBias::PostStepDoIt number of secondaries: " << pChange->GetNumberOfSecondaries() << G4endl;
#endif
  return pChange;
}

