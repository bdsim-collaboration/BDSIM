//   BDSIM, (C) 2001-2007 
//    
//   version 0.4 
//   last modified : 10 Sept 2007 by malton@pp.rhul.ac.uk
//  

//
//    Physics lists
//

#include <iomanip>   

#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSPhysicsList.hh"

#include "globals.hh"
#include "G4ParticleDefinition.hh"
#include "G4ParticleWithCuts.hh"
#include "G4ProcessManager.hh"
#include "G4ProcessVector.hh"
#include "G4ParticleTypes.hh"
#include "G4ParticleTable.hh"
#include "G4FastSimulationManagerProcess.hh"

#include "G4Version.hh"

#include "G4Material.hh"
#include "G4MaterialTable.hh"
#include "G4ios.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BERT_HP.hh"
#if G4VERSION_NUMBER < 1000
#include "HadronPhysicsQGSP_BERT.hh"
#include "HadronPhysicsQGSP_BERT_HP.hh"
#include "HadronPhysicsFTFP_BERT.hh"
#include "HadronPhysicsLHEP.hh"
#else
#include "G4HadronPhysicsQGSP_BERT.hh"
#include "G4HadronPhysicsQGSP_BERT_HP.hh"
#include "G4HadronPhysicsQGSP_BIC_HP.hh"
#include "G4HadronPhysicsFTFP_BERT.hh"
#endif
#include "G4Decay.hh"
#include "G4eeToHadrons.hh"

#include "G4EmStandardPhysics.hh"
#include "G4EmLivermorePhysics.hh"
#include "G4EmPenelopePhysics.hh"

// physics processes

//
// EM
//

// gamma
#include "G4ComptonScattering.hh"
#include "G4GammaConversion.hh"
#include "G4GammaConversionToMuons.hh"
#include "G4PhotoElectricEffect.hh"
#include "BDSXSBias.hh" //added by L.D. 16/11/09
#include "BDSGammaConversion_LPB.hh" //added by M.D. Salt, R.B. Appleby, 15/10/09

// charged particles
#include "G4eMultipleScattering.hh"
#include "G4MuMultipleScattering.hh"
#include "G4hMultipleScattering.hh"

//Optical processes
#include "G4Cerenkov.hh"
#include "G4Scintillation.hh"
#include "G4OpAbsorption.hh"
#include "G4OpRayleigh.hh"
#include "G4OpMieHG.hh"
#include "G4OpBoundaryProcess.hh"
#include "G4CoulombScattering.hh"

// e
#include "G4eIonisation.hh"
#include "G4eBremsstrahlung.hh"
#include "G4eplusAnnihilation.hh"
#include "BDSeBremsstrahlung_LPB.hh" //added by M.D. Salt, R.B.Appleby,15/10/09

// mu
#include "G4MuIonisation.hh"
#include "G4MuBremsstrahlung.hh"
#include "G4MuPairProduction.hh"
#if G4VERSION_NUMBER < 950
#include "G4MuonNucleusProcess.hh"
#elif G4VERSION_NUMBER < 953
#include "G4MuNuclearInteraction.hh"
#else 
#include "G4MuonVDNuclearModel.hh"
#endif

//ions
#include "G4hIonisation.hh"
#include "G4ionIonisation.hh"


//Decay of unstable particles
#include "G4Decay.hh"

//
// Low EM
//

//gamma
#if G4VERSION_NUMBER < 950
#include "G4LowEnergyRayleigh.hh"
#include "G4LowEnergyPhotoElectric.hh"
#include "G4LowEnergyCompton.hh"
#include "G4LowEnergyGammaConversion.hh"
#else
#include "G4RayleighScattering.hh"
#include "G4LivermoreRayleighModel.hh"
#include "G4PhotoElectricEffect.hh"
#include "G4LivermorePhotoElectricModel.hh"
#include "G4ComptonScattering.hh"
#include "G4LivermoreComptonModel.hh"
#include "G4GammaConversion.hh"
#include "G4LivermoreGammaConversionModel.hh"
#endif

//e
#if G4VERSION_NUMBER < 950
#include "G4LowEnergyIonisation.hh"
#include "G4LowEnergyBremsstrahlung.hh"
#else
#include "G4eIonisation.hh"
#include "G4LivermoreIonisationModel.hh"
#include "G4UniversalFluctuation.hh"

#include "G4eBremsstrahlung.hh"
#include "G4LivermoreBremsstrahlungModel.hh"
#endif

#include "G4AnnihiToMuPair.hh"

//ions
#if G4VERSION_NUMBER < 950
#include "G4hLowEnergyIonisation.hh"
#endif

#include "BDSLaserCompton.hh"
#include "BDSSynchrotronRadiation.hh"
#include "BDSContinuousSR.hh"
#include "G4StepLimiter.hh"
#include "G4UserSpecialCuts.hh"

#include "G4OpticalPhysics.hh"

//
// Hadronic
//
#include "G4TheoFSGenerator.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4QGSModel.hh"
#include "G4GammaParticipants.hh"
#include "G4QGSMFragmentation.hh"
#include "G4ExcitedStringDecay.hh"

#if G4VERSION_NUMBER < 1000
#include "G4GammaNuclearReaction.hh"
#include "G4ElectroNuclearReaction.hh"
#endif
#include "G4PhotoNuclearProcess.hh"
#include "G4ElectronNuclearProcess.hh"
#include "G4PositronNuclearProcess.hh"

//Planck scattering
#include "BDSPlanckScatterBuilder.hh"

//
// particle definition
//

// Bosons
#include "G4ChargedGeantino.hh"
#include "G4Geantino.hh"
#include "G4Gamma.hh"
#include "G4OpticalPhoton.hh"

// leptons
#include "G4MuonPlus.hh"
#include "G4MuonMinus.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoMu.hh"

#include "G4TauPlus.hh"
#include "G4TauMinus.hh"
#include "G4NeutrinoTau.hh"
#include "G4AntiNeutrinoTau.hh"

#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4NeutrinoE.hh"
#include "G4AntiNeutrinoE.hh"

// Hadrons
#include "G4MesonConstructor.hh"
#include "G4BaryonConstructor.hh"
#include "G4IonConstructor.hh"

//ShortLived
#include "G4ShortLivedConstructor.hh"




BDSPhysicsList::BDSPhysicsList():  G4VUserPhysicsList()
{
  theCerenkovProcess           = NULL;
  theScintillationProcess      = NULL;
  theAbsorptionProcess         = NULL;
  theRayleighScatteringProcess = NULL;
  theMieHGScatteringProcess    = NULL;
  theBoundaryProcess           = NULL;

  theStandardPhysList          = NULL;
  theBDSIMPhysList             = NULL;
  theHadPhysList1              = NULL;
  theHadPhysList2              = NULL;

  verbose = BDSExecOptions::Instance()->GetVerbose();

  // construct particles

  //defaultCutValue = 0.7*CLHEP::mm;  
  defaultCutValue = BDSGlobalConstants::Instance()->GetDefaultRangeCut()*CLHEP::m;  
  SetDefaultCutValue(BDSGlobalConstants::Instance()->GetDefaultRangeCut()*CLHEP::m);

  G4cout  << __METHOD_NAME__ << "Charged Thresholdcut=" 
	  << BDSGlobalConstants::Instance()->GetThresholdCutCharged()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout  << __METHOD_NAME__ << "Photon Thresholdcut=" 
	  << BDSGlobalConstants::Instance()->GetThresholdCutPhotons()/CLHEP::GeV<<" GeV"<<G4endl;
  G4cout  << __METHOD_NAME__ << "Default range cut=" 
	  << BDSGlobalConstants::Instance()->GetDefaultRangeCut()/CLHEP::m<<" m"<<G4endl;

  //This is the GEANT4 physics list verbose level.
  SetVerboseLevel(1);   
}

BDSPhysicsList::~BDSPhysicsList()
{
  delete theStandardPhysList;
  delete theBDSIMPhysList;
  delete theHadPhysList1;
  delete theHadPhysList2;
}

void BDSPhysicsList::ConstructProcess()
{ 
#if BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif 

  bool plistFound=false;
  //standard physics lists
  if(BDSGlobalConstants::Instance()->GetPhysListName() == "QGSP_BERT_HP"){
#if G4VERSION_NUMBER < 1000
    theStandardPhysList = new HadronPhysicsQGSP_BERT_HP();
#else
    theStandardPhysList = new G4HadronPhysicsQGSP_BERT_HP();
#endif
    theStandardPhysList->ConstructProcess();
    plistFound=true;
  } else if(BDSGlobalConstants::Instance()->GetPhysListName() == "QGSP_BERT"){
#if G4VERSION_NUMBER < 1000
    theStandardPhysList = new HadronPhysicsQGSP_BERT();
#else
    theStandardPhysList = new G4HadronPhysicsQGSP_BERT();
#endif
    theStandardPhysList->ConstructProcess();
    plistFound=true;
  } else if (BDSGlobalConstants::Instance()->GetPhysListName() == "QGSP_BERT_HP_muon"){ //Modified standard physics lists
#if G4VERSION_NUMBER < 1000
    theStandardPhysList = new HadronPhysicsQGSP_BERT_HP();
#else
    theStandardPhysList = new G4HadronPhysicsQGSP_BERT_HP();
#endif
    theStandardPhysList->ConstructProcess();
    ConstructMuon();
    plistFound=true;
  } else if (BDSGlobalConstants::Instance()->GetPhysListName() == "QGSP_BERT_muon"){
#if G4VERSION_NUMBER < 1000
    theStandardPhysList = new HadronPhysicsQGSP_BERT();
#else
    theStandardPhysList = new G4HadronPhysicsQGSP_BERT();
#endif
    theStandardPhysList->ConstructProcess();
    ConstructMuon();
    plistFound=true;
  } else if(BDSGlobalConstants::Instance()->GetPhysListName() == "QGSP_BERT_HP_muon_em_low"){
#if G4VERSION_NUMBER < 1000
    theStandardPhysList = new HadronPhysicsQGSP_BERT_HP();
#else
    theStandardPhysList = new G4HadronPhysicsQGSP_BERT_HP();
#endif
    theStandardPhysList->ConstructProcess();
    ConstructMuon();
    ConstructEM_Low_Energy();
    plistFound=true;
  } else if(BDSGlobalConstants::Instance()->GetPhysListName() == "livermore"){
    G4EmLivermorePhysics* physList = new G4EmLivermorePhysics;
    physList->ConstructProcess();
    plistFound=true;
  }else if(BDSGlobalConstants::Instance()->GetPhysListName() == "penelope"){
    G4EmPenelopePhysics* physList = new G4EmPenelopePhysics;
    physList->ConstructProcess();
    plistFound=true;
  }else if(BDSGlobalConstants::Instance()->GetPhysListName() == "G4EmStandard"){
    G4EmStandardPhysics* physList = new G4EmStandardPhysics;
    physList->ConstructProcess();
    plistFound=true;
  }


  if(!plistFound){
    //Need to add transportation if non-standard physics list
    AddTransportation();
  }
  
  //Apply the following in all cases - step limiter etc.
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    if((particle->GetParticleName()=="gamma")||(particle->GetParticleName()=="e-")||(particle->GetParticleName()=="e+")){
      particle->SetApplyCutsFlag(true);
    }
    G4ProcessManager *pmanager = particle->GetProcessManager();
    pmanager->AddProcess(new G4StepLimiter,-1,-1,1);
#ifndef NOUSERSPECIALCUTS
    pmanager->AddDiscreteProcess(new G4UserSpecialCuts);
#endif
  }
  
  //===========================================
  //Some options
  //-------------------------------------------
  //Build planck scattering if option is set
  if(BDSGlobalConstants::Instance()->GetDoPlanckScattering()){
    BDSPlanckScatterBuilder* psbuild = new BDSPlanckScatterBuilder();
    psbuild->Build();
    // psbuild is leaked. This can't be deleted as its BDSPlanckScatterProcess is registered
  }
  //A flag to switch on hadronic lead particle biasing
  if (BDSGlobalConstants::Instance()->GetUseHadLPB() ){
    setenv("SwitchLeadBiasOn","1",1); 
  }
  //Synchrotron radiation
  if(BDSGlobalConstants::Instance()->GetSynchRadOn()) {
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "synch. rad. is turned on" << G4endl;
#endif
    ConstructSR();
  } else {
#ifdef BDSDEBUG
    G4cout << __METHOD_NAME__ << "synch. rad. is turned OFF!" << G4endl;
#endif
  }
  //Particle decay
  if(BDSGlobalConstants::Instance()->GetDecayOn()) ConstructDecay();
  //Optical processes
  ConstructOptical();
  //============================================
  
  if(!plistFound) return;
  //Search BDSIM physics lists
  
  if (BDSGlobalConstants::Instance()->GetPhysListName() == "standard") return;

  // register physics processes here
      
  //Always add parameterisation
  AddParameterisation();
      
  // standard e+/e-/gamma electromagnetic interactions
  if(BDSGlobalConstants::Instance()->GetPhysListName() == "em_standard") 
    {
      ConstructEM();
    }
  else if(BDSGlobalConstants::Instance()->GetPhysListName() == "em_single_scatter") 
    {
      ConstructEMSingleScatter();
    }
  else if(BDSGlobalConstants::Instance()->GetPhysListName() == "merlin") 
    {
      ConstructMerlin();
    }
      
  // low energy em processes
  else if(BDSGlobalConstants::Instance()->GetPhysListName() == "em_low") 
    {
      ConstructEM_Low_Energy();
    }
      
  // standard electromagnetic + muon
  else if(BDSGlobalConstants::Instance()->GetPhysListName() == "em_muon") 
    {
      ConstructEM();
      ConstructMuon();
    }
  // standard hadronic - photo-nuclear etc.
  else if(BDSGlobalConstants::Instance()->GetPhysListName() == "hadronic_standard") 
    {
      ConstructEM();
      ConstructHadronic();
    }
      
  // standard electromagnetic + muon + hadronic
  else if(BDSGlobalConstants::Instance()->GetPhysListName() == "hadronic_muon") 
    {
      ConstructEM();
      ConstructMuon();
      ConstructHadronic();
    }
  
  else if(BDSGlobalConstants::Instance()->GetPhysListName() == "hadronic_QGSP_BERT") {
    ConstructEM();
#if G4VERSION_NUMBER < 1000
    theBDSIMPhysList = new HadronPhysicsQGSP_BERT("hadron");
#else
    theBDSIMPhysList = new G4HadronPhysicsQGSP_BERT("hadron");
#endif
    theBDSIMPhysList->ConstructProcess();
  }
  
  else if(BDSGlobalConstants::Instance()->GetPhysListName() == "hadronic_QGSP_BERT_muon") {
    ConstructEM();
    ConstructMuon();
#if G4VERSION_NUMBER < 1000
    theBDSIMPhysList = new HadronPhysicsQGSP_BERT("hadron");
#else
    theBDSIMPhysList = new G4HadronPhysicsQGSP_BERT("hadron");
#endif
    theBDSIMPhysList->ConstructProcess();
  }
  
  else if(BDSGlobalConstants::Instance()->GetPhysListName() == "hadronic_FTFP_BERT"){
    ConstructEM();
#if G4VERSION_NUMBER < 1000
    theBDSIMPhysList = new HadronPhysicsFTFP_BERT;
#else
    theBDSIMPhysList = new G4HadronPhysicsFTFP_BERT;
#endif
    theBDSIMPhysList->ConstructProcess();
  }
  
  else if(BDSGlobalConstants::Instance()->GetPhysListName() == "hadronic_QGSP_BERT_HP_muon"){
    ConstructEM();
    ConstructMuon();
    ConstructHadronic();
    //      ConstructPhotolepton_Hadron();
#if G4VERSION_NUMBER < 1000
    theBDSIMPhysList = new HadronPhysicsQGSP_BERT_HP;
#else
    theBDSIMPhysList = new G4HadronPhysicsQGSP_BERT_HP;
#endif
    theBDSIMPhysList->ConstructProcess();
  }
      
  else if(BDSGlobalConstants::Instance()->GetPhysListName() == "hadronic_FTFP_BERT_muon"){
    G4cout << __METHOD_NAME__ << "Using hadronic_FTFP_BERT_muon" << G4endl;
    ConstructEM();
    ConstructMuon();
#if G4VERSION_NUMBER < 1000
    theBDSIMPhysList = new HadronPhysicsFTFP_BERT;
#else
    theBDSIMPhysList = new G4HadronPhysicsFTFP_BERT;
#endif
    theBDSIMPhysList->ConstructProcess();
  }
  // physics list for laser wire - standard em stuff +
  // weighted compton scattering from laser wire
  else if(BDSGlobalConstants::Instance()->GetPhysListName() == "lw") {
    ConstructEM();
    ConstructLaserWire();
  }

  //default - standard (only transportation)
  G4cerr<<"WARNING : Unknown physics list "<<BDSGlobalConstants::Instance()->GetPhysListName()<<
    "  using transportation only (standard) "<<G4endl;
}

void BDSPhysicsList::ConstructParticle()
{
  //standard physics lists
  if (theStandardPhysList) {
    theStandardPhysList->ConstructParticle();
  } else {
    // pseudo-particles
    G4Geantino::GeantinoDefinition();
    G4ChargedGeantino::ChargedGeantinoDefinition();
    
    // gamma
    G4Gamma::GammaDefinition();
    
    // optical photon
    G4OpticalPhoton::OpticalPhotonDefinition();
    
    // leptons
    G4Electron::ElectronDefinition();
    G4Positron::PositronDefinition();
    G4MuonPlus::MuonPlusDefinition();
    G4MuonMinus::MuonMinusDefinition();
    
    G4NeutrinoE::NeutrinoEDefinition();
    G4AntiNeutrinoE::AntiNeutrinoEDefinition();
    G4NeutrinoMu::NeutrinoMuDefinition();
    G4AntiNeutrinoMu::AntiNeutrinoMuDefinition();  
    
    // mesons
    G4MesonConstructor mConstructor;
    mConstructor.ConstructParticle();
    
    // baryons
    G4BaryonConstructor bConstructor;
    bConstructor.ConstructParticle();
    
    // ions
    G4IonConstructor iConstructor;
    iConstructor.ConstructParticle();
    
    //  Construct resonances and quarks
    G4ShortLivedConstructor pShortLivedConstructor;
    pShortLivedConstructor.ConstructParticle();
  }
  
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

#include "G4Region.hh"
#include "G4ProductionCuts.hh"
void BDSPhysicsList::SetCuts()
{
  if (verbose){
    G4cout << __METHOD_NAME__ << " setting cuts\n";
    
  }
  
  SetCutsWithDefault();   


  
    if(BDSGlobalConstants::Instance()->GetProdCutPhotons()>0)
      SetCutValue(BDSGlobalConstants::Instance()->GetProdCutPhotons(),G4ProductionCuts::GetIndex("gamma"));
  
   if(BDSGlobalConstants::Instance()->GetProdCutElectrons()>0)
     SetCutValue(BDSGlobalConstants::Instance()->GetProdCutElectrons(),G4ProductionCuts::GetIndex("e-"));
  
  if(BDSGlobalConstants::Instance()->GetProdCutPositrons()>0)
    SetCutValue(BDSGlobalConstants::Instance()->GetProdCutPositrons(),G4ProductionCuts::GetIndex("e+"));
  

    
  if(1)
    DumpCutValuesTable(); 

}


// particular physics process constructors

void BDSPhysicsList::ConstructEM(){
  ConstructEMMisc();
  ConstructMultipleScattering();
}

void BDSPhysicsList::ConstructEMSingleScatter(){
  ConstructEMMisc();
  ConstructCoulombScattering();
}

void BDSPhysicsList::ConstructEMMisc()
{
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (particleName == "gamma") {
      // gamma         
      pmanager->AddDiscreteProcess(new G4PhotoElectricEffect);
      pmanager->AddDiscreteProcess(new G4ComptonScattering);

      if(0){
	G4GammaConversion* gammaconversion = new G4GammaConversion();
	gammaconversion->SetLambdaFactor(1/1.0e-20);
	BDSXSBias* gammaconversion_xsbias = new BDSXSBias();
	gammaconversion_xsbias->RegisterProcess(gammaconversion);
	gammaconversion_xsbias->SetEnhanceFactor(1e-20);
	pmanager->AddDiscreteProcess(gammaconversion_xsbias);
	
      } else if (BDSGlobalConstants::Instance()->GetUseEMLPB()){ //added by M.D. Salt, R.B. Appleby, 15/10/09
	  G4GammaConversion* gammaconversion = new G4GammaConversion();
	  GammaConversion_LPB* gammaconversion_lpb = new GammaConversion_LPB();
	  gammaconversion_lpb->RegisterProcess(gammaconversion);
	  pmanager->AddDiscreteProcess(gammaconversion_lpb);
      } else {
	pmanager->AddDiscreteProcess(new G4GammaConversion);
      }
    
      
    } else if (particleName == "e-") {
      pmanager->AddProcess(new G4eIonisation,       -1, 2,2);
      if(0){
	G4eBremsstrahlung* ebremsstrahlung = new G4eBremsstrahlung();
	ebremsstrahlung->SetLambdaFactor(1/1.0e-20);
	BDSXSBias* ebremsstrahlung_xsbias = new BDSXSBias();
	ebremsstrahlung_xsbias->RegisterProcess(ebremsstrahlung);
	ebremsstrahlung_xsbias->SetEnhanceFactor(1e-20);
	pmanager->AddDiscreteProcess(ebremsstrahlung_xsbias);     
      }	else if(BDSGlobalConstants::Instance()->GetUseEMLPB()){ //added by M.D. Salt, R.B. Appleby, 15/10/09
	  
        G4eBremsstrahlung* ebremsstrahlung = new G4eBremsstrahlung();
        eBremsstrahlung_LPB* ebremsstrahlung_lpb = new eBremsstrahlung_LPB();
        ebremsstrahlung_lpb->RegisterProcess(ebremsstrahlung);
        pmanager->AddProcess(ebremsstrahlung_lpb,     -1,-1,3);
      } else {
	G4eBremsstrahlung* ebremsstrahlung = new G4eBremsstrahlung();
        pmanager->AddProcess(ebremsstrahlung,   -1, 3,3);     
      }
            
      if(BDSGlobalConstants::Instance()->GetTurnOnCerenkov()){
        G4Cerenkov* theCerenkovProcess = new G4Cerenkov;
        pmanager->AddProcess(theCerenkovProcess);
        pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
      }
      
    } else if (particleName == "e+") {
      //positron
      pmanager->AddProcess(new G4eIonisation,       -1, 2,2);
      if(0){
	G4eBremsstrahlung* ebremsstrahlung = new G4eBremsstrahlung();
	ebremsstrahlung->SetLambdaFactor(1/1.0e-20);
	BDSXSBias* ebremsstrahlung_xsbias = new BDSXSBias();
	ebremsstrahlung_xsbias->RegisterProcess(ebremsstrahlung);
	ebremsstrahlung_xsbias->SetEnhanceFactor(1e-20);
	pmanager->AddDiscreteProcess(ebremsstrahlung_xsbias);      
      } else if (BDSGlobalConstants::Instance()->GetUseEMLPB()){
	G4eBremsstrahlung* ebremsstrahlung = new G4eBremsstrahlung();
	eBremsstrahlung_LPB* ebremsstrahlung_lpb = new eBremsstrahlung_LPB();
        ebremsstrahlung_lpb->RegisterProcess(ebremsstrahlung);
        pmanager->AddProcess(ebremsstrahlung_lpb,     -1,-1,3);
      } else {
        pmanager->AddProcess(new G4eBremsstrahlung,   -1, 3,3);
      }
      pmanager->AddProcess(new G4eplusAnnihilation,  0,-1,4);
      if(BDSGlobalConstants::Instance()->GetTurnOnCerenkov()){      
        G4Cerenkov* theCerenkovProcess = new G4Cerenkov;
        pmanager->AddProcess(theCerenkovProcess);
        pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
      }
    } else if ((!particle->IsShortLived()) &&
	       (particle->GetPDGCharge() != 0.0) && 
	       (particle->GetParticleName() != "chargedgeantino")) {
      //all others charged particles except geantino
      pmanager->AddProcess(new G4hIonisation,       -1, 2,2);
           if(BDSGlobalConstants::Instance()->GetTurnOnCerenkov()){
        G4Cerenkov* theCerenkovProcess = new G4Cerenkov;
        pmanager->AddProcess(theCerenkovProcess);
        pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
      }
    }
  }
}

void BDSPhysicsList::ConstructMultipleScattering(){
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if (particleName == "e-") {
      //electron
      pmanager->AddProcess(new G4eMultipleScattering,-1, 1,1);

      
    } else if (particleName == "e+") {
      
      pmanager->AddProcess(new G4eMultipleScattering,-1, 1,1);
    } else if ((!particle->IsShortLived()) &&
	       (particle->GetPDGCharge() != 0.0) && 
	       (particle->GetParticleName() != "chargedgeantino")) {
      
      //all others charged particles except geantino
      pmanager->AddProcess(new G4hMultipleScattering,-1, 1,1);
    }
  }
}

void BDSPhysicsList::ConstructCoulombScattering(){
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName(); 
    if (particleName == "e-") {
      pmanager->AddDiscreteProcess(new G4CoulombScattering);
    } else if (particleName == "e+") {
      pmanager->AddDiscreteProcess(new G4CoulombScattering);
    }else if ((!particle->IsShortLived()) &&
	      (particle->GetPDGCharge() != 0.0) && 
	      (particle->GetParticleName() != "chargedgeantino")) {
      //all others charged particles except geantino
      pmanager->AddDiscreteProcess(new G4CoulombScattering);
    } 
  }
}

// particular physics process constructors
void BDSPhysicsList::ConstructMuon()
{
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();

    if (particleName == "gamma") {
      // gamma         
      G4GammaConversionToMuons* gammaconversiontomuons = new G4GammaConversionToMuons();
      BDSXSBias* gammaconversiontomuon_xsbias = new BDSXSBias();
      gammaconversiontomuons->SetCrossSecFactor(BDSGlobalConstants::Instance()->GetGammaToMuFe());
      gammaconversiontomuon_xsbias->RegisterProcess(gammaconversiontomuons);
      gammaconversiontomuon_xsbias->SetEnhanceFactor(BDSGlobalConstants::Instance()->GetGammaToMuFe());
      pmanager->AddDiscreteProcess(gammaconversiontomuon_xsbias);
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "GammaToMuFe = " << BDSGlobalConstants::Instance()->GetGammaToMuFe() << G4endl;
#endif
    } else if (particleName == "e+") {
      //positron
      //===========ee to hadrons in development================
      G4eeToHadrons* eetohadrons = new G4eeToHadrons();
      // BDSXSBias* eetohadrons_xsbias = new BDSXSBias();
      // G4cout << "eeToHadronsXSBias = " << BDSGlobalConstants::Instance()->GetEeToHadronsFe() << G4endl;
      eetohadrons->SetCrossSecFactor(BDSGlobalConstants::Instance()->GetEeToHadronsFe());
      //eetohadrons_xsbias->RegisterProcess(eetohadrons);
      //eetohadrons_xsbias->SetEnhanceFactor(BDSGlobalConstants::Instance()->GetEeToHadronsFe());
      //pmanager->AddDiscreteProcess(eetohadrons_xsbias);
      pmanager->AddDiscreteProcess(eetohadrons);
      //-------------------------------------------------------
      G4AnnihiToMuPair* annihitomupair = new G4AnnihiToMuPair();
      BDSXSBias* annihitomupair_xsbias = new BDSXSBias();
      annihitomupair->SetCrossSecFactor(BDSGlobalConstants::Instance()->GetAnnihiToMuFe());
      annihitomupair_xsbias->RegisterProcess(annihitomupair);
      annihitomupair_xsbias->SetEnhanceFactor(BDSGlobalConstants::Instance()->GetAnnihiToMuFe());
      pmanager->AddDiscreteProcess(annihitomupair_xsbias); 
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "AnnihiToMuFe = " << BDSGlobalConstants::Instance()->GetAnnihiToMuFe() << G4endl;
#endif    
    } else if( particleName == "mu+" || 
               particleName == "mu-"    ) {
      //muon  
      pmanager->AddProcess(new G4MuMultipleScattering,-1, 1,1);
      pmanager->AddProcess(new G4MuIonisation,      -1, 2,2);
      pmanager->AddProcess(new G4MuBremsstrahlung,  -1, 3,3);
      pmanager->AddProcess(new G4MuPairProduction,  -1, 4,4);
      if(BDSGlobalConstants::Instance()->GetTurnOnCerenkov()){
        G4Cerenkov* theCerenkovProcess = new G4Cerenkov;
        pmanager->AddProcess(theCerenkovProcess);
        pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
#if G4VERSION_NUMBER < 950
        pmanager->AddDiscreteProcess(new G4MuonNucleusProcess);     
#elif G4VERSION_NUMBER < 953
	pmanager->AddDiscreteProcess(new G4MuNuclearInteraction);     
#else 
	/*	pmanager->AddDiscreteProcess(new G4MuonVDNuclearModel); */
#endif
      }
    }
  }
}
   

void BDSPhysicsList::ConstructDecay()
{
  theParticleIterator->reset();
  G4Decay* theDecayProcess = new G4Decay();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if (theDecayProcess->IsApplicable(*particle)) { 
      pmanager -> AddProcess(theDecayProcess);
      pmanager -> SetProcessOrdering(theDecayProcess, idxPostStep);
      pmanager -> SetProcessOrdering(theDecayProcess, idxAtRest);
    }
  }
}


void BDSPhysicsList::ConstructOptical()
{
  bool bCerOn=BDSGlobalConstants::Instance()->GetTurnOnCerenkov();
  bool bBirksOn=BDSGlobalConstants::Instance()->GetTurnOnBirksSaturation();

//  theCerenkovProcess->DumpPhysicsTable();
//  theScintillationProcess->DumpPhysicsTable();
//  theRayleighScatteringProcess->DumpPhysicsTable();

  if(bCerOn){
    if(!theCerenkovProcess){
      theCerenkovProcess = new G4Cerenkov("Cerenkov");
    }
  }
  
  theScintillationProcess      = new G4Scintillation("Scintillation");
  if(BDSGlobalConstants::Instance()->GetTurnOnOpticalAbsorption()){
    theAbsorptionProcess         = new G4OpAbsorption();
  }
  if(BDSGlobalConstants::Instance()->GetTurnOnRayleighScattering()){
    theRayleighScatteringProcess = new G4OpRayleigh();
  }
  if(BDSGlobalConstants::Instance()->GetTurnOnMieScattering()){
    theMieHGScatteringProcess    = new G4OpMieHG();
  }
  if(BDSGlobalConstants::Instance()->GetTurnOnOpticalSurface()){
    theBoundaryProcess           = new G4OpBoundaryProcess();
#if G4VERSION_NUMBER < 960
    G4OpticalSurfaceModel themodel = unified;
    theBoundaryProcess->SetModel(themodel);
#endif
  }

  SetVerboseLevel(1);
  theScintillationProcess->SetScintillationYieldFactor(BDSGlobalConstants::Instance()->GetScintYieldFactor());
  theScintillationProcess->SetTrackSecondariesFirst(true);

  // Use Birks Correction in the Scintillation process

  if(bBirksOn){
    G4EmSaturation* emSaturation = G4LossTableManager::Instance()->EmSaturation();
    theScintillationProcess->AddSaturation(emSaturation);
  }

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    if(bCerOn){
      if (theCerenkovProcess->IsApplicable(*particle)) {
	pmanager->AddProcess(theCerenkovProcess);
	pmanager->SetProcessOrdering(theCerenkovProcess,idxPostStep);
	theCerenkovProcess->SetMaxNumPhotonsPerStep(20);
	theCerenkovProcess->SetMaxBetaChangePerStep(10.0);
	theCerenkovProcess->SetTrackSecondariesFirst(true);
      }
    }
    if (theScintillationProcess->IsApplicable(*particle)) {
      pmanager->AddProcess(theScintillationProcess);
      pmanager->SetProcessOrderingToLast(theScintillationProcess, idxAtRest);
      pmanager->SetProcessOrderingToLast(theScintillationProcess, idxPostStep);
    }
    if (particleName == "opticalphoton") {
#ifdef DEBUG
      G4cout << " AddDiscreteProcess to OpticalPhoton " << G4endl;
#endif
      if(BDSGlobalConstants::Instance()->GetTurnOnOpticalAbsorption()){
	pmanager->AddDiscreteProcess(theAbsorptionProcess);
      }
      if(BDSGlobalConstants::Instance()->GetTurnOnRayleighScattering()){
	pmanager->AddDiscreteProcess(theRayleighScatteringProcess);
      }
      if(BDSGlobalConstants::Instance()->GetTurnOnMieScattering()){
	pmanager->AddDiscreteProcess(theMieHGScatteringProcess);
      }
      if(BDSGlobalConstants::Instance()->GetTurnOnOpticalSurface()){
	pmanager->AddDiscreteProcess(theBoundaryProcess);
      }
    }
  }
}


void BDSPhysicsList::ConstructMerlin()
{
  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if (particleName == "e-") {
      //electron
      pmanager->AddProcess(new G4eMultipleScattering,-1, 1,1);
      pmanager->AddProcess(new G4eIonisation,       -1, 2,2);
      pmanager->AddProcess(new G4eBremsstrahlung,   -1, 3,3);      
    } 
  }
}


void BDSPhysicsList::ConstructEM_Low_Energy()
{
#if G4VERSION_NUMBER > 949
  //Applicability range for Livermore models                                                                                                                                
  //for higher energies, the Standard models are used                                                                                                                       
  G4double highEnergyLimit = 1*CLHEP::GeV;
#endif

  theParticleIterator->reset();
  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
     
    if (particleName == "gamma") {
#if G4VERSION_NUMBER < 950
      pmanager->AddDiscreteProcess(new G4LowEnergyRayleigh());
      pmanager->AddDiscreteProcess(new G4LowEnergyPhotoElectric);
      pmanager->AddDiscreteProcess(new G4LowEnergyCompton);
      pmanager->AddDiscreteProcess(new G4LowEnergyGammaConversion);
#else
      G4RayleighScattering* rayl = new G4RayleighScattering();
      G4LivermoreRayleighModel*
	raylModel = new G4LivermoreRayleighModel();
      raylModel->SetHighEnergyLimit(highEnergyLimit);
      rayl->AddEmModel(0, raylModel);
      pmanager->AddDiscreteProcess(rayl);

      G4PhotoElectricEffect* phot = new G4PhotoElectricEffect();
      G4LivermorePhotoElectricModel*
	photModel = new G4LivermorePhotoElectricModel();
      photModel->SetHighEnergyLimit(highEnergyLimit);
      phot->AddEmModel(0, photModel);
      pmanager->AddDiscreteProcess(phot);

      G4ComptonScattering* compt = new G4ComptonScattering();
      G4LivermoreComptonModel*
	comptModel = new G4LivermoreComptonModel();
      comptModel->SetHighEnergyLimit(highEnergyLimit);
      compt->AddEmModel(0, comptModel);
      pmanager->AddDiscreteProcess(compt);
      
      G4GammaConversion* conv = new G4GammaConversion();
      G4LivermoreGammaConversionModel*
	convModel = new G4LivermoreGammaConversionModel();
      convModel->SetHighEnergyLimit(highEnergyLimit);
      conv->AddEmModel(0, convModel);
      pmanager->AddDiscreteProcess(conv);
#endif
      
    } else if (particleName == "e-") {
        pmanager->AddProcess(new G4eMultipleScattering,-1, 1,1);
#if G4VERSION_NUMBER < 950
	pmanager->AddProcess(new G4LowEnergyIonisation,        -1, 2,2);
	pmanager->AddProcess(new G4LowEnergyBremsstrahlung,    -1, 3,3);
#else
	G4eIonisation* eIoni = new G4eIonisation();
	G4LivermoreIonisationModel*
	  eIoniModel = new G4LivermoreIonisationModel();
	eIoniModel->SetHighEnergyLimit(highEnergyLimit);
	eIoni->AddEmModel(0, eIoniModel, new G4UniversalFluctuation() );
	pmanager->AddProcess(eIoni,                   -1,-1, 1);
	
	G4eBremsstrahlung* eBrem = new G4eBremsstrahlung();
	G4LivermoreBremsstrahlungModel*
	  eBremModel = new G4LivermoreBremsstrahlungModel();
	eBremModel->SetHighEnergyLimit(highEnergyLimit);
	eBrem->AddEmModel(0, eBremModel);
	pmanager->AddProcess(eBrem,                   -1,-1, 2);
#endif
	    
    } else if (particleName == "e+") {
        pmanager->AddProcess(new G4eMultipleScattering,-1, 1,1);
	pmanager->AddProcess(new G4eIonisation,        -1, 2,2);
	pmanager->AddProcess(new G4eBremsstrahlung,    -1, 3,3);
	pmanager->AddProcess(new G4eplusAnnihilation,   0,-1,4);
    } else if( particleName == "mu+" || 
               particleName == "mu-"    ) {
        pmanager->AddProcess(new G4MuMultipleScattering,-1, 1,1);
        pmanager->AddProcess(new G4MuIonisation,      -1, 2,2);
        pmanager->AddProcess(new G4MuBremsstrahlung,  -1, 3,3);
        pmanager->AddProcess(new G4MuPairProduction,  -1, 4,4);       

    } else if (particleName == "GenericIon") {
        pmanager->AddProcess(new G4hMultipleScattering,-1, 1,1);
#if G4VERSION_NUMBER < 950
	pmanager->AddProcess(new G4hLowEnergyIonisation,       -1,2,2);
      //      pmanager->AddProcess(new G4ionIonisation,      -1, 2,2);
      // it dose not work here
#else
	pmanager->AddProcess(new G4ionIonisation,     -1,-1, 1);
#endif

    } else if ((!particle->IsShortLived()) &&
	       (particle->GetPDGCharge() != 0.0) && 
	       (particle->GetParticleName() != "chargedgeantino")) {

        pmanager->AddProcess(new G4hMultipleScattering,-1, 1,1);
#if G4VERSION_NUMBER < 950
      pmanager->AddProcess(new G4hLowEnergyIonisation,       -1,2,2);
#else
      pmanager->AddProcess(new G4hIonisation,       -1,-1, 1);
#endif
    }
  }
}

void BDSPhysicsList::ConstructLaserWire()
{
  G4cout << "Constructing laser-wire" << G4endl;

  theParticleIterator->reset();

  BDSLaserCompton* lwProcess = new BDSLaserCompton;

  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if (particleName == "e-") {
      pmanager->AddProcess(lwProcess);
      pmanager->SetProcessOrderingToLast(lwProcess,idxPostStep);
    }

    if (particleName == "e+") {
      pmanager->AddProcess(lwProcess);
      pmanager->SetProcessOrderingToLast(lwProcess,idxPostStep);
    }

  }

}


// Hadron Processes

#include "G4HadronElasticProcess.hh"
#include "G4HadronFissionProcess.hh"
#include "G4HadronCaptureProcess.hh"

#include "G4PionPlusInelasticProcess.hh"
#include "G4PionMinusInelasticProcess.hh"
#include "G4KaonPlusInelasticProcess.hh"
#include "G4KaonZeroSInelasticProcess.hh"
#include "G4KaonZeroLInelasticProcess.hh"
#include "G4KaonMinusInelasticProcess.hh"
#include "G4ProtonInelasticProcess.hh"
#include "G4AntiProtonInelasticProcess.hh"
#include "G4NeutronInelasticProcess.hh"
#include "G4AntiNeutronInelasticProcess.hh"
#include "G4LambdaInelasticProcess.hh"
#include "G4AntiLambdaInelasticProcess.hh"
#include "G4SigmaPlusInelasticProcess.hh"
#include "G4SigmaMinusInelasticProcess.hh"
#include "G4AntiSigmaPlusInelasticProcess.hh"
#include "G4AntiSigmaMinusInelasticProcess.hh"
#include "G4XiZeroInelasticProcess.hh"
#include "G4XiMinusInelasticProcess.hh"
#include "G4AntiXiZeroInelasticProcess.hh"
#include "G4AntiXiMinusInelasticProcess.hh"
#include "G4OmegaMinusInelasticProcess.hh"
#include "G4AntiOmegaMinusInelasticProcess.hh"

// Low-energy Models
#if G4VERSION_NUMBER < 1000
#include "G4LElastic.hh"
#include "G4LCapture.hh"
#else
#include "G4HadronElastic.hh"
#include "G4NeutronRadCapture.hh"
#endif
#include "G4LFission.hh"

#if G4VERSION_NUMBER < 1000
#include "G4LEPionPlusInelastic.hh"
#include "G4LEPionMinusInelastic.hh"
#include "G4LEKaonPlusInelastic.hh"
#include "G4LEKaonZeroSInelastic.hh"
#include "G4LEKaonZeroLInelastic.hh"
#include "G4LEKaonMinusInelastic.hh"
#include "G4LEProtonInelastic.hh"
#include "G4LEAntiProtonInelastic.hh"
#include "G4LENeutronInelastic.hh"
#include "G4LEAntiNeutronInelastic.hh"
#include "G4LELambdaInelastic.hh"
#include "G4LEAntiLambdaInelastic.hh"
#include "G4LESigmaPlusInelastic.hh"
#include "G4LESigmaMinusInelastic.hh"
#include "G4LEAntiSigmaPlusInelastic.hh"
#include "G4LEAntiSigmaMinusInelastic.hh"
#include "G4LEXiZeroInelastic.hh"
#include "G4LEXiMinusInelastic.hh"
#include "G4LEAntiXiZeroInelastic.hh"
#include "G4LEAntiXiMinusInelastic.hh"
#include "G4LEOmegaMinusInelastic.hh"
#include "G4LEAntiOmegaMinusInelastic.hh"
#else
#include "G4CascadeInterface.hh"
#include "G4BinaryLightIonReaction.hh"
#endif

// -- generator models
#include "G4TheoFSGenerator.hh"
#include "G4ExcitationHandler.hh"
#include "G4GeneratorPrecompoundInterface.hh"
#include "G4StringModel.hh"
#include "G4PreCompoundModel.hh"
#include "G4QGSMFragmentation.hh"
#include "G4ExcitedStringDecay.hh"

void BDSPhysicsList::ConstructHadronic()
{
  
#if G4VERSION_NUMBER < 1000
  G4NeutronBuilder* theNeutrons=new G4NeutronBuilder;
  theNeutrons->RegisterMe(new G4LHEPNeutronBuilder);

  G4ProtonBuilder * thePro=new G4ProtonBuilder;
  thePro->RegisterMe(new G4LHEPProtonBuilder);

  G4PiKBuilder * thePiK=new G4PiKBuilder;
  thePiK->RegisterMe(new G4LHEPPiKBuilder);

  theNeutrons->Build();
  thePro->Build();
  thePiK->Build();

  // Photonuclear processes

  G4PhotoNuclearProcess * thePhotoNuclearProcess = new G4PhotoNuclearProcess;
  G4GammaNuclearReaction * theGammaReaction = new G4GammaNuclearReaction;
  G4ElectronNuclearProcess * theElectronNuclearProcess = new G4ElectronNuclearProcess;
  G4PositronNuclearProcess * thePositronNuclearProcess = new G4PositronNuclearProcess;
  G4ElectroNuclearReaction * theElectroReaction = new G4ElectroNuclearReaction;
  G4TheoFSGenerator * theModel = new G4TheoFSGenerator;
  
  G4QGSModel< G4GammaParticipants > * theStringModel = new G4QGSModel< G4GammaParticipants >;
  G4ExcitedStringDecay * theStringDecay = new G4ExcitedStringDecay(/*theFragmentation=*/new G4QGSMFragmentation);
  theStringModel->SetFragmentationModel(theStringDecay);
  
  G4GeneratorPrecompoundInterface * theCascade = new G4GeneratorPrecompoundInterface;
  
  theModel->SetTransport(theCascade);
  theModel->SetHighEnergyGenerator(theStringModel);

  G4ProcessManager * aProcMan = 0;
  
  aProcMan = G4Gamma::Gamma()->GetProcessManager();
  theGammaReaction->SetMaxEnergy(3.5*CLHEP::GeV);
  thePhotoNuclearProcess->RegisterMe(theGammaReaction);
  theModel->SetMinEnergy(3.*CLHEP::GeV);
  theModel->SetMaxEnergy(100*CLHEP::TeV);
  thePhotoNuclearProcess->RegisterMe(theModel);
  aProcMan->AddDiscreteProcess(thePhotoNuclearProcess);

  aProcMan = G4Electron::Electron()->GetProcessManager();
  theElectronNuclearProcess->RegisterMe(theElectroReaction);
  aProcMan->AddDiscreteProcess(theElectronNuclearProcess);
  
  aProcMan = G4Positron::Positron()->GetProcessManager();
  thePositronNuclearProcess->RegisterMe(theElectroReaction);
  aProcMan->AddDiscreteProcess(thePositronNuclearProcess);
#else
    /*
    From (14-7-14)
    http://geant4.cern.ch/support/proc_mod_catalog/physics_lists/useCases.shtml  

    LHC neutron fluxes

    The Simulation of neutron fluxes needs, in addition to the simulation of hadronic showers, a transport for low energy neutron down to thermal energies. Recommended: QGSP_BERT_HP, QGSP_BIC_HP
    
    Linear collider neutron fluxes

    Recommended: QGSP_BERT_HP, QGSP_BIC_HP

    Shielding applications (all energies)

    Recommended: QGSP_BERT_HP, QGSP_BIC_HP, QGSP_INCLXX, Shielding
  */

  /* based on these recommendations QGSP_BERT_HP, QGSP_BIC_HP are chosen 
     with QGSP_INCLXX and Shielding perhaps optional for certain studies
   */

  theHadPhysList1 = new G4HadronPhysicsQGSP_BERT_HP();
  theHadPhysList1->ConstructProcess();

  theHadPhysList2 = new G4HadronPhysicsQGSP_BIC_HP();
  theHadPhysList2->ConstructProcess();

#endif
}

void BDSPhysicsList::ConstructSR()
{
  // BDSIM's version of Synchrotron Radiation
  BDSSynchrotronRadiation* srProcess = new BDSSynchrotronRadiation;
  
  BDSContinuousSR *contSR = new BDSContinuousSR(); // contin. energy loss process

  // G4's version of Synchrotron Radiation - not used because does not have
  // Multiplicity or MeanFreeFactor capability
  // G4SynchrotronRadiation* srProcess = new G4SynchrotronRadiation;

  theParticleIterator->reset();

  while( (*theParticleIterator)() ){
    G4ParticleDefinition* particle = theParticleIterator->value();
    G4ProcessManager* pmanager = particle->GetProcessManager();
    G4String particleName = particle->GetParticleName();
    
    if (particleName == "e-") {
      pmanager->AddProcess(srProcess);
      pmanager->SetProcessOrderingToLast(srProcess,idxPostStep);

      G4int idx = pmanager->AddProcess(contSR);
      pmanager->SetProcessOrderingToLast(contSR,idxPostStep);
      pmanager->SetProcessActivation(idx, false);
    }
    
    if (particleName == "e+") {
      pmanager->AddProcess(srProcess);
      pmanager->SetProcessOrderingToLast(srProcess,idxPostStep);

      //G4int idx = pmanager->AddProcess(contSR);
      //      pmanager->SetProcessOrderingToLast(contSR,idxPostStep);
      //      pmanager->SetProcessActivation(idx, false);
    }
    
  }
  return; 
}

void BDSPhysicsList::AddParameterisation()
{
  G4FastSimulationManagerProcess*
    theFastSimulationManagerProcess =
    new G4FastSimulationManagerProcess();
  G4cout << "FastSimulationManagerProcess" <<G4endl;
  theParticleIterator->reset();
  //G4cout<<"---"<<G4endl;                                                                                                                                              
  while( (*theParticleIterator)() ){
    //G4cout<<"+++"<<G4endl;                                                                                                                                            

    G4ParticleDefinition* particle = theParticleIterator->value();
    // G4cout<<"--- particle "<<particle->GetParticleName()<<G4endl;                                                                                                    
    G4ProcessManager* pmanager = particle->GetProcessManager();
    // The fast simulation process becomes a discrete process only since 9.0:                                                                                                 
    pmanager->AddDiscreteProcess(theFastSimulationManagerProcess);
  }
}
