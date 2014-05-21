#include "BDSAwakePhysicsList.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh"
#include "G4EmPenelopePhysics.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmStandardPhysics.hh"
#include "G4OpticalPhysics.hh"
#include "G4OpticalProcessIndex.hh"
#include "G4ParticleTable.hh"

#include "BDSGlobalConstants.hh"

BDSAwakePhysicsList::BDSAwakePhysicsList() 
: G4VModularPhysicsList(){
  SetVerboseLevel(1);

  ConstructPhysics();
  ConfigurePhysics();
  Register();

  // EM physics
  

  SetParticleDefinition();
}

void BDSAwakePhysicsList::ConstructPhysics(){
  _opticalPhysics = new G4OpticalPhysics();
  _emPhysics = new G4EmPenelopePhysics();
}

void BDSAwakePhysicsList::ConfigurePhysics(){
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  _opticalPhysics->Configure(kCerenkov,  globals->GetTurnOnCerenkov());///< Cerenkov process index                                   
  _opticalPhysics->Configure(kScintillation, true);///< Scintillation process index                              
  _opticalPhysics->Configure(kAbsorption, globals->GetTurnOnOpticalAbsorption());///< Absorption process index                                 
  _opticalPhysics->Configure(kRayleigh, globals->GetTurnOnRayleighScattering());///< Rayleigh scattering process index                        
  _opticalPhysics->Configure(kMieHG, globals->GetTurnOnMieScattering());///< Mie scattering process index                             
  _opticalPhysics->Configure(kBoundary, globals->GetTurnOnOpticalSurface());///< Boundary process index                                   
  _opticalPhysics->Configure(kWLS, true);///< Wave Length Shifting process index                       
    //    _opticalPhysics->Configure(kNoProcess,      globals->GetTurnOn< Number of processes, no selected process
}

void BDSAwakePhysicsList::Register(){
  RegisterPhysics(_opticalPhysics);
  RegisterPhysics(_emPhysics);
}

BDSAwakePhysicsList::~BDSAwakePhysicsList()
{ 
}


void BDSAwakePhysicsList::SetCuts()
{
  G4VUserPhysicsList::SetCuts();

  SetCutsWithDefault();   


  
    if(BDSGlobalConstants::Instance()->GetProdCutPhotons()>0)
      SetCutValue(BDSGlobalConstants::Instance()->GetProdCutPhotons(),G4ProductionCuts::GetIndex("gamma"));
  
   if(BDSGlobalConstants::Instance()->GetProdCutElectrons()>0)
     SetCutValue(BDSGlobalConstants::Instance()->GetProdCutElectrons(),G4ProductionCuts::GetIndex("e-"));
  
  if(BDSGlobalConstants::Instance()->GetProdCutPositrons()>0)
    SetCutValue(BDSGlobalConstants::Instance()->GetProdCutPositrons(),G4ProductionCuts::GetIndex("e+"));
  
  DumpCutValuesTable(); 
}  

void BDSAwakePhysicsList::SetParticleDefinition(){
  // set primary particle definition and kinetic beam parameters other than total energy
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  BDSGlobalConstants::Instance()->SetParticleDefinition(particleTable->
							FindParticle(BDSGlobalConstants::Instance()->GetParticleName()) );  
  
  if(!BDSGlobalConstants::Instance()->GetParticleDefinition()) 
    {
      G4Exception("Particle not found, quitting!", "-1", FatalException, "");
      exit(1);
    }
  
  // set kinetic beam parameters other than total energy
  BDSGlobalConstants::Instance()->SetBeamMomentum( sqrt(pow(BDSGlobalConstants::Instance()->GetBeamTotalEnergy(),2)-
							pow(BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass(),2)) );
  
  BDSGlobalConstants::Instance()->SetBeamKineticEnergy(BDSGlobalConstants::Instance()->GetBeamTotalEnergy() - 
						       BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass() );
  
  G4cout << __METHOD_NAME__ << "Beam properties:"<<G4endl;
  G4cout << __METHOD_NAME__ << "Particle : " 
	 << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetParticleName()<<G4endl;
  G4cout << __METHOD_NAME__ << "Mass : " 
	 << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGMass()/CLHEP::GeV<< " GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Charge : " 
	 << BDSGlobalConstants::Instance()->GetParticleDefinition()->GetPDGCharge()<< " e"<<G4endl;
  G4cout << __METHOD_NAME__ << "Total Energy : "
	 << BDSGlobalConstants::Instance()->GetBeamTotalEnergy()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Kinetic Energy : "
	 << BDSGlobalConstants::Instance()->GetBeamKineticEnergy()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout << __METHOD_NAME__ << "Momentum : "
	 << BDSGlobalConstants::Instance()->GetBeamMomentum()/CLHEP::GeV<<" GeV"<<G4endl;
}
