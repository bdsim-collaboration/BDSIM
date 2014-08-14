#include "BDSParameterisationPhysics.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4ProcessManager.hh"

BDSParameterisationPhysics::BDSParameterisationPhysics():G4VPhysicsConstructor("BDSParameterisationPhysics"),_wasActivated(false),
							 _fastSimulationManagerProcess(NULL)
{;}

BDSParameterisationPhysics::~BDSParameterisationPhysics()
{;}

void BDSParameterisationPhysics::ConstructParticle(){
  G4Gamma::Gamma();
  G4Electron::Electron();
  G4Positron::Positron();    
  return;
}

void BDSParameterisationPhysics::ConstructProcess(){
  if(_wasActivated) return;
  _wasActivated=true;
  
  _fastSimulationManagerProcess =
    new G4FastSimulationManagerProcess();
  G4cout << "FastSimulationManagerProcess" <<G4endl;
  aParticleIterator->reset();
  //G4cout<<"---"<<G4endl;                                                                                                                                              
  while( (*aParticleIterator)() ){
    //G4cout<<"+++"<<G4endl;                                                                                                                                            
    G4ParticleDefinition* particle = aParticleIterator->value();
    // G4cout<<"--- particle "<<particle->GetParticleName()<<G4endl;                                                                                                    
    G4ProcessManager* pmanager = particle->GetProcessManager();
    // The fast simulation process becomes a discrete process only since 9.0:                                                                                                 
    pmanager->AddDiscreteProcess(_fastSimulationManagerProcess);
  }
  return;
}


