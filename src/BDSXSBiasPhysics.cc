#include "BDSXSBiasPhysics.hh"
#include "G4GammaConversionToMuons.hh"
#include "BDSVProcess.hh"

BDSXSBiasPhysics::BDSXSBiasPhysics(G4String name):G4VPhysicsConstructor(name),_wasActivated(false)
{;}

BDSXSBiasPhysics::~BDSXSBiasPhysics()
{;}

void BDSXSBiasPhysics::ConstructParticle(){;}

void BDSXSBiasPhysics::ConstructProcess(){;}


//Deactivates an old process with the same name as proc1 and inserts a new one, or just inserts the new one if the old one is not present.
void BDSXSBiasPhysics::ReplaceDiscreteProcess(G4ProcessManager* pmanager, G4VProcess* proc1, G4VProcess* proc2){
  G4ProcessVector* processVector = pmanager->GetProcessList();
  //  G4int processIndex=-1;
//Replace the existing gammaconversiontomuons process with our wrappered process;
  for(int i=0; i < processVector->entries(); ++i){
    G4VProcess* tempProc = processVector->removeAt(i);
    if(tempProc->GetProcessName() != (proc1)->GetProcessName()){
      processVector->insertAt(i,tempProc);
      //  pmanager->SetProcessActivation(processVector->index(processVector[i]),false);
    } else {
      pmanager->AddDiscreteProcess(proc2);
      return;
    }
  }
}
