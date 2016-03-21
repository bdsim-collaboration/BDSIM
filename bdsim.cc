/**
 * @file bdsim.cc
 *
 * \mainpage
 * BDSIM © 2001-2016
 *
 * version 0.9.develop
 */

#include "BDSDebug.hh" 
#include "BDSExecOptions.hh"     // executable command line options 
#include "BDSGlobalConstants.hh" //  global parameters

#include <cstdlib>      // standard headers 
#include <cstdio>
#include <signal.h>

#include "G4EventManager.hh" // Geant4 includes
#include "G4GeometryManager.hh"
#include "G4ParallelWorldPhysics.hh"
#include "G4TrackingManager.hh"
#include "G4SteppingManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4GenericBiasingPhysics.hh"

#include "BDSAcceleratorModel.hh"
#include "BDSBunch.hh"
#include "BDSColours.hh"
#include "BDSDetectorConstruction.hh"   
#include "BDSEventAction.hh"
#include "BDSGeometryWriter.hh"
#include "BDSMaterials.hh"
#include "BDSModularPhysicsList.hh"
#include "BDSOutputBase.hh" 
#include "BDSOutputFactory.hh"
#include "BDSParallelWorldSampler.hh"
#include "BDSParser.hh" // Parser
#include "BDSPhysicsList.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSRandom.hh" // for random number generator from CLHEP
#include "BDSRunAction.hh"
#include "BDSRunManager.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSDManager.hh"
#include "BDSSteppingAction.hh"
#include "BDSStackingAction.hh"
#include "BDSTrackingAction.hh"
#include "BDSUtilities.hh"
#include "BDSVisManager.hh"

//=======================================================
// Global variables 
BDSOutputBase* bdsOutput=nullptr;     ///< output interface
//=======================================================

int main(int argc,char** argv)
{
  /// Print header & program information
  G4cout<<"bdsim : version 0.9.develop"<<G4endl;
  G4cout<<"        (C) 2001-2016 Royal Holloway University London"<<G4endl;
  G4cout<<"        http://www.pp.rhul.ac.uk/bdsim"<<G4endl;
  G4cout<<G4endl;

  /// Initialize executable command line options reader object
  const BDSExecOptions* execOptions = BDSExecOptions::Instance(argc,argv);
  execOptions->Print();
  
  /// Check geant4 exists in the current environment
  if (!BDS::Geant4EnvironmentIsSet())
    {G4cout << "No Geant4 environmental variables found - please source geant4.sh environment" << G4endl; exit(1);}

#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> DEBUG mode is on." << G4endl;
#endif

  /// Parse lattice file
  G4cout << __FUNCTION__ << "> Using input file : "<< execOptions->GetInputFilename()<<G4endl;
  
  BDSParser::Instance(execOptions->GetInputFilename());

  /// Parse options, explicitly initialise materials and global constants and construct required materials
  BDSMaterials::Instance()->PrepareRequiredMaterials();

  /// Force construction of global constants after parser has been initialised (requires materials first).
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();

  /// Initialize random number generator
  BDSRandom::CreateRandomNumberGenerator();
  BDSRandom::SetSeed(); // set the seed from options or from exec options
  if (execOptions->SetSeedState()) //optionally load the seed state from file (separate from seed)
    {BDSRandom::LoadSeedState(execOptions->GetSeedStateFilename());}
  if (BDSExecOptions::Instance()->GetOutputFormat() != BDSOutputFormat::none)
    {BDSRandom::WriteSeedState();} //write the current state once set / loaded

  /// Instantiate the specific type of bunch distribution (class),
  /// get the corresponding parameters from the gmad parser info
  /// and attach to the initialised random number generator.
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> Instantiating chosen bunch distribution." << G4endl;
#endif
  BDSBunch* bdsBunch = new BDSBunch();
  bdsBunch->SetOptions(BDSParser::Instance()->GetOptions());

  /// Optionally generate primaries only and exit
  if (execOptions->GeneratePrimariesOnly())
    {
      // output creation is duplicated below but with this if loop, we exit so ok.
      bdsOutput = BDSOutputFactory::CreateOutput(execOptions->GetOutputFormat());
      G4double x0=0.0, y0=0.0, z0=0.0, xp=0.0, yp=0.0, zp=0.0, t=0.0, E=0.0, weight=1.0;
      for (G4int i = 0; i < globalConstants->GetNumberToGenerate(); i++)
      {
        bdsBunch->GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight);
        bdsOutput->WritePrimary(E, x0, y0, z0, xp, yp, zp, t, weight, 1, i, 1);
      }
      delete bdsBunch;
      delete bdsOutput;
      exit(0);
    }

  /// Construct mandatory run manager (the G4 kernel) and
  /// register mandatory initialization classes.

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Constructing run manager"<<G4endl;
#endif
  BDSRunManager * runManager = new BDSRunManager;
  // runManager->SetNumberOfAdditionalWaitingStacks(1);
  // note this doesn't actually construct the accelerator - only instantiates the class.
  // the run manager later calls the construct method
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - detector construction"<<G4endl;
#endif
  /// Register the geometry and parallel world construction methods with run manager.
  BDSDetectorConstruction* realWorld = new BDSDetectorConstruction();
  BDSParallelWorldSampler* samplerWorld = new BDSParallelWorldSampler();
  realWorld->RegisterParallelWorld(samplerWorld);
  runManager->SetUserInitialization(realWorld);  

  //For geometry sampling, phys list must be initialized before detector.
  // BUT for samplers we use a parallel world and this HAS to be before the physcis
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Constructing phys list" << G4endl;
#endif
  G4String physicsListName = BDSParser::Instance()->GetOptions().physicsList;
  if(BDSParser::Instance()->GetOptions().modularPhysicsListsOn)
    {
      G4ParallelWorldPhysics* pWorld  = new G4ParallelWorldPhysics(samplerWorld->GetName());
      BDSModularPhysicsList* physList = new BDSModularPhysicsList(physicsListName);
      physList->RegisterPhysics(pWorld);
      /* Biasing */
#if G4VERSION_NUMBER > 999
      G4GenericBiasingPhysics* physBias = new G4GenericBiasingPhysics();
      physBias->Bias("e-");
      physBias->Bias("e+");
      physBias->Bias("gamma");
      physBias->Bias("proton");
      physBias->Bias("mu-");
      physBias->Bias("mu+");
      physBias->Bias("pi-");
      physBias->Bias("pi+");
      physList->RegisterPhysics(physBias);
#endif
      runManager->SetUserInitialization(physList);
    }
  else
    { 
      BDSPhysicsList* physList = new BDSPhysicsList(physicsListName);
      runManager->SetUserInitialization(physList);
    }

  /// Set the geometry tolerance
  G4GeometryTolerance* theGeometryTolerance = G4GeometryTolerance::GetInstance();
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> Default geometry tolerances: surface " 
	 << theGeometryTolerance->GetSurfaceTolerance()/CLHEP::m << " m " 
	 << theGeometryTolerance->GetAngularTolerance() << " rad " 
	 << theGeometryTolerance->GetRadialTolerance()/CLHEP::m  << " m" << G4endl;
#endif
  // This sets the tolerances for the geometry (1e-11 times this value)
  // Note, this doesn't actually have any affect on the size of the geometry,
  // and is only used to calculate the tolerance in geant4. This is really misleading
  // naming on the part of geant4. There is no way to just set a tolerance directly.
  G4double worldMaximumExtent=1000*CLHEP::m;
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldMaximumExtent); 
  G4cout << __FUNCTION__ << "> Geometry Tolerances: "     << G4endl;
  G4cout << __FUNCTION__ << ">" << std::setw(22) << "Surface: " << std::setw(10) << theGeometryTolerance->GetSurfaceTolerance()/CLHEP::m << " m"   << G4endl;
  G4cout << __FUNCTION__ << ">" << std::setw(22) << "Angular: " << std::setw(10) << theGeometryTolerance->GetAngularTolerance()          << " rad" << G4endl;
  G4cout << __FUNCTION__ << ">" << std::setw(22) << "Radial: "  << std::setw(10) << theGeometryTolerance->GetRadialTolerance()/CLHEP::m  << " m"   << G4endl;

  /// Set user action classes
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - Run Action"<<G4endl;
#endif
  runManager->SetUserAction(new BDSRunAction);

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - Event Action"<<G4endl;
#endif
  runManager->SetUserAction(new BDSEventAction());

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - Stepping Action"<<G4endl;
#endif
  // Only add steppingaction if it is actually used, so do check here (for cpu reasons)
  if (globalConstants->GetThresholdCutPhotons() > 0 || globalConstants->GetThresholdCutCharged() > 0
      || execOptions->GetVerboseStep()) {
    runManager->SetUserAction(new BDSSteppingAction);
  }
  
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - Tracking Action"<<G4endl;
#endif
  runManager->SetUserAction(new BDSTrackingAction);

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - Stacking Action"<<G4endl;
#endif
  runManager->SetUserAction(new BDSStackingAction);

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - Primary Generator"<<G4endl;
#endif
  runManager->SetUserAction(new BDSPrimaryGeneratorAction(bdsBunch));

  /// Initialize G4 kernel
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Initialising Geant4 kernel"<<G4endl;
#endif
  runManager->Initialize();

  /// Implement bias operations on all volumes only after G4RunManager::Initialize()
  if (BDSParser::Instance()->GetOptions().modularPhysicsListsOn)
    {realWorld->BuildPhysicsBias();}

#ifdef BDSDEBUG
  auto physics = runManager->GetUserPhysicsList();
  if (const BDSModularPhysicsList* modPhysics = dynamic_cast<const BDSModularPhysicsList*>(physics))
    {
      modPhysics->PrintPrimaryParticleProcesses();
      modPhysics->PrintDefinedParticles();
    }
#endif

  /// Set verbosity levels
  runManager->SetVerboseLevel(execOptions->GetVerboseRunLevel());
  G4EventManager::GetEventManager()->SetVerboseLevel(execOptions->GetVerboseEventLevel());
  G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(execOptions->GetVerboseTrackingLevel());
  G4EventManager::GetEventManager()->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(execOptions->GetVerboseSteppingLevel());
  
  /// Close the geometry in preparation for running - everything is now fixed.
  G4bool bCloseGeometry = G4GeometryManager::GetInstance()->CloseGeometry();
  if(!bCloseGeometry)
    { 
      G4cerr << "bdsim.cc: error - geometry not closed." << G4endl;
      return 1;
    }

  if (execOptions->ExportGeometry())
    {
      BDSGeometryWriter geometrywriter;
      geometrywriter.ExportGeometry(execOptions->GetExportType(),
				    execOptions->GetExportFileName());
    }
  else
    {
      /// Construct output
#ifdef BDSDEBUG
      G4cout << __FUNCTION__ << "> Setting up output." << G4endl;
#endif
      bdsOutput = BDSOutputFactory::CreateOutput(execOptions->GetOutputFormat());
      G4cout.precision(10);
      
      /// Catch aborts to close output stream/file. perhaps not all are needed.
      signal(SIGABRT, &BDS::HandleAborts); // aborts
      signal(SIGTERM, &BDS::HandleAborts); // termination requests
      signal(SIGSEGV, &BDS::HandleAborts); // segfaults
      // no interrupts since ctest sends an interrupt signal when interrupted
      // and then the BDSIM process somehow doesn't get killed
      // signal(SIGINT,  &BDS::HandleAborts); // interrupts

    /// Run in either interactive or batch mode
      if(!execOptions->GetBatch())   // Interactive mode
	{
	  BDSVisManager visManager;
	  visManager.StartSession(argc,argv);
	}
      else           // Batch mode
	{runManager->BeamOn(globalConstants->GetNumberToGenerate());}
    }

  /// Termination & clean up.
  G4GeometryManager::GetInstance()->OpenGeometry();

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> BDSOutput deleting..."<<G4endl;
#endif
  delete bdsOutput;
  
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> instances deleting..."<<G4endl;
#endif
  delete BDSAcceleratorModel::Instance();
  delete execOptions;
  delete globalConstants;
  delete BDSMaterials::Instance();
  delete BDSParser::Instance();

  // instances not used in this file, but no other good location for deletion
  delete BDSColours::Instance();
  delete BDSSDManager::Instance();
  delete BDSSamplerRegistry::Instance();
  
#ifdef BDSDEBUG 
  G4cout<< __FUNCTION__ << "> BDSRunManager deleting..."<<G4endl;
#endif
  delete runManager;
  delete samplerWorld;
  delete bdsBunch;

  G4cout << __FUNCTION__ << "> End of Run, Thank you for using BDSIM!" << G4endl;

  return 0;
}
