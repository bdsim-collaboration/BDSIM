//XSBias with artificially scaled cross section

#include "BDSXSBias.hh"
#include "G4SteppingManager.hh"


BDSXSBias::BDSXSBias(const G4String& aName,
                                         G4ProcessType   aType)
  : G4WrapperProcess(aName, aType), eFactor(1.0)
{
}

BDSXSBias::BDSXSBias(const BDSXSBias& right)
  : G4WrapperProcess(right), eFactor(right.eFactor)
{
}

BDSXSBias::~BDSXSBias()
{
}



G4VParticleChange* BDSXSBias::PostStepDoIt(
                                                   const G4Track& track,
                                                   const G4Step&  stepData
                                                   )
{
#ifdef DEBUG
  G4cout <<" ###PostStepDoIt " << G4endl;
  G4cout << "BDSXSBias::PostStepDoit  Getting pChange" << G4endl;
#endif
  G4VParticleChange* pChange = pRegProcess->PostStepDoIt( track, stepData );
  pChange->SetVerboseLevel(0);
#ifdef DEBUG
  G4cout << "BDSXSBias::PostStepDoit Choosing setsecondaryweightbyprocess" << G4endl;
#endif
  pChange->SetSecondaryWeightByProcess(true);
  pChange->SetParentWeightByProcess(true);
#ifdef DEBUG
  G4cout << "BDSXSBias::PostStepDoit Getting parent weight" << G4endl;
#endif
  G4double w =  pChange->GetParentWeight();
  G4double ws = w / eFactor;
  G4double survivalProb = w - ws;
  
#ifdef DEBUG
  G4cout << "BDSXSBias::PostStepDoit Getting number of secondaries" << G4endl;
#endif
  G4int iNSec = pChange->GetNumberOfSecondaries();

#ifdef DEBUG  
  G4cout << "BDSXSBias::PostStepDoit Setting secondary weights" << G4endl;
#endif

  G4bool pionEvent = false;
  G4bool gammaInPionEvent= false;
  for (G4int i = 0; i < iNSec; i++) {
    pChange->GetSecondary(i)->SetWeight(ws); 
    if(std::abs(pChange->GetSecondary(i)->GetDefinition()->GetPDGEncoding())==211) {
      pionEvent=true;
    }
    if(std::abs(pChange->GetSecondary(i)->GetDefinition()->GetPDGEncoding())==22){
      if (pionEvent==true){
        gammaInPionEvent = true;
      }
    }
  }
  
  if(pionEvent){
    G4cout << "Pion event" << G4endl;
    if(gammaInPionEvent){
#ifdef DEBUG      
      G4cout << "gammaInPionEvent" << G4endl;
#endif
    }
    else {
#ifdef DEBUG      
      G4cout << "NO gammaInPionEvent" << G4endl;
#endif
    }
  }
  
  if (pionEvent){
    G4Track* secTrack[100];
    for (G4int i = 0; i < iNSec; i++) {
#ifdef DEBUG      
      G4cout << "BDSXSBias::PostStepDoIt Correcting kinetic energy of pion" << G4endl;
#endif
      secTrack[i] = pChange->GetSecondary(i);
      G4double EkCorrected = secTrack[i]->GetDynamicParticle()->GetKineticEnergy()/1000;
      secTrack[i]->SetKineticEnergy(EkCorrected);
      secTrack[i]->SetWeight(ws);

    }
    pChange->Clear();
    pChange->SetNumberOfSecondaries(iNSec);  
    for(G4int i=0; i<iNSec; i++){
      pChange->AddSecondary(secTrack[i]);
    }
  }
  
#ifdef DEBUG   
  G4cout << "BDSXSBias::PostStepDoit Testing for primary survival" << G4endl;
#endif
  if(G4UniformRand()<survivalProb){
    pChange->ProposeParentWeight(survivalProb);
    pChange->ProposeTrackStatus(fAlive);
  }
  
#ifdef DEBUG  
  G4cout << "BDSXSBias::PostStepDoIt number of secondaries: " << pChange->GetNumberOfSecondaries() << G4endl;
#endif
  return pChange;
}

