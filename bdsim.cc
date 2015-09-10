//  
//   BDSIM, (C) 2001-2015
//   
//   version 0.8

#include "BDSDebug.hh" 
#include "BDSExecOptions.hh"     // executable command line options 
#include "BDSGlobalConstants.hh" //  global parameters

#include <cstdlib>      // standard headers 
#include <cstdio>
#include <signal.h>

#include "G4EventManager.hh" // Geant4 includes
#include "G4GeometryManager.hh"
#include "G4TrackingManager.hh"
#include "G4SteppingManager.hh"
#include "G4GeometryTolerance.hh"

#include "G4Version.hh"
#if G4VERSION_NUMBER > 999
#include "G4GenericBiasingPhysics.hh"
#endif

#include "BDSAcceleratorModel.hh"
#include "BDSBunch.hh"
#include "BDSDetectorConstruction.hh"   
#include "BDSEventAction.hh"
#include "BDSGeometryWriter.hh"
#include "BDSMaterials.hh"
#include "BDSModularPhysicsList.hh"
#include "BDSOutputBase.hh" 
#include "BDSOutputFactory.hh"
#include "BDSPhysicsList.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSRandom.hh" // for random number generator from CLHEP
#include "BDSRunAction.hh"
#include "BDSRunManager.hh"
#include "BDSSteppingAction.hh"
#include "BDSStackingAction.hh"
#include "BDSSurvey.hh"
#include "BDSUserTrackingAction.hh"
#include "BDSUtilities.hh"
#include "BDSVisManager.hh"

#include "parser/gmad.h"  // GMAD parser
#include "parser/options.h"

//=======================================================
// Global variables 
BDSOutputBase* bdsOutput=nullptr;         // output interface
//=======================================================

namespace GMAD {
  extern Options options;
}

int main(int argc,char** argv)
{
  // print header
  G4cout<<"bdsim : version 0.8.develop"<<G4endl;
  G4cout<<"        (C) 2001-2015 Royal Holloway University London"<<G4endl;
  G4cout<<"        http://www.ph.rhul.ac.uk/twiki/bin/view/PP/JAI/BdSim"<<G4endl;
  G4cout<<G4endl;

  /* Initialize executable command line options reader object */
  const BDSExecOptions* execOptions = BDSExecOptions::Instance(argc,argv);
  execOptions->Print();
  
  // check geant4 exists in the current environment
  if (!BDS::Geant4EnvironmentIsSet())
    {G4cout << "No Geant4 environmental variables found - please source geant4.sh environment" << G4endl; exit(1);}

#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> DEBUG mode is on." << G4endl;
#endif  

  //
  // Parse lattice file
  //
  G4cout << __FUNCTION__ << "> Using input file : "<< execOptions->GetInputFilename()<<G4endl;
  
  GMAD::gmad_parser(execOptions->GetInputFilename());

  //
  // parse options and explicitly initialise materials and global constants
  //
  BDSMaterials::Instance();
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();
  
  //
  // initialize random number generator
  //

  BDSRandom::CreateRandomNumberGenerator();
  BDSRandom::SetSeed(); // set the seed from options or from exec options
  if (execOptions->SetSeedState()) //optionally load the seed state from file
    {BDSRandom::LoadSeedState(execOptions->GetSeedStateFilename());}
  if (BDSExecOptions::Instance()->GetOutputFormat() != BDSOutputFormat::none)
    {BDSRandom::WriteSeedState();} //write the current state once set / loaded

  // instantiate the specific type of bunch distribution (class),
  // get the corresponding parameters from the gmad parser info
  // and attach to the initialised random number generator
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> Instantiating chosen bunch distribution." << G4endl;
#endif
  BDSBunch* bdsBunch = new BDSBunch();
  bdsBunch->SetOptions(GMAD::options);
  
  //
  // construct mandatory run manager (the G4 kernel) and
  // set mandatory initialization classes
  //

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Constructing run manager"<<G4endl;
#endif
  BDSRunManager * runManager = new BDSRunManager;
  // runManager->SetNumberOfAdditionalWaitingStacks(1);

  //For geometry sampling, phys list must be initialized before detector.
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Constructing phys list" << G4endl;
#endif
  if(GMAD::options.modularPhysicsListsOn) {
    BDSModularPhysicsList *physList = new BDSModularPhysicsList;
    /* Biasing */
#if G4VERSION_NUMBER > 999
    G4GenericBiasingPhysics *physBias = new G4GenericBiasingPhysics();
    physBias->Bias("e-");
    physBias->Bias("e+");
    physBias->Bias("gamma");
    physBias->Bias("proton");
    physList->RegisterPhysics(physBias);
#endif
    runManager->SetUserInitialization(physList);
  }
  else { 
    BDSPhysicsList        *physList = new BDSPhysicsList;  
    runManager->SetUserInitialization(physList);
  }

  // Set the geometry tolerance
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

  // note this doesn't actually construct the accelerator - only instantiates the class.
  // the run manager later calls the construct method
  BDSDetectorConstruction* detector = new BDSDetectorConstruction();
 
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - detector construction"<<G4endl;
#endif
  runManager->SetUserInitialization(detector);

  //
  // set user action classes
  //
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - runaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSRunAction);

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - eventaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSEventAction());

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - steppingaction"<<G4endl;
#endif
  // Only add steppingaction if it is actually used, so do check here (for cpu reasons)
  if (globalConstants->GetThresholdCutPhotons() > 0 || globalConstants->GetThresholdCutCharged() > 0
      || execOptions->GetVerboseStep()) {
    runManager->SetUserAction(new BDSSteppingAction);
  }
  
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - trackingaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSUserTrackingAction);

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - stackingaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSStackingAction);

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - primary generator"<<G4endl;
#endif
  runManager->SetUserAction(new BDSPrimaryGeneratorAction(bdsBunch));
  
  //
  // Initialize G4 kernel
  //
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Initialising Geant4 kernel"<<G4endl;
#endif
  runManager->Initialize();

  //
  // set verbosity levels
  //
  runManager->SetVerboseLevel(execOptions->GetVerboseRunLevel());
  G4EventManager::GetEventManager()->SetVerboseLevel(execOptions->GetVerboseEventLevel());
  G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(execOptions->GetVerboseTrackingLevel());
  G4EventManager::GetEventManager()->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(execOptions->GetVerboseSteppingLevel());
  
  // Close the geometry
  G4bool bCloseGeometry = G4GeometryManager::GetInstance()->CloseGeometry();
  if(!bCloseGeometry) { 
    G4cerr << "bdsim.cc: error - geometry not closed." << G4endl;
    return 1;
  }

  if (execOptions->ExportGeometry())
    {
      BDSGeometryWriter geometrywriter;
      geometrywriter.ExportGeometry(execOptions->GetExportType(),
				    execOptions->GetExportFileName());
      // clean up before exiting
      G4GeometryManager::GetInstance()->OpenGeometry();
      delete BDSAcceleratorModel::Instance();
      delete execOptions;
      delete globalConstants;
      delete BDSMaterials::Instance();
      delete runManager;
      delete bdsBunch;
      return 0;
    }
  
  // set default output formats:
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> Setting up output." << G4endl;
#endif
  bdsOutput = BDSOutputFactory::CreateOutput(execOptions->GetOutputFormat());
  G4cout.precision(10);

  // catch aborts to close output stream/file. perhaps not all are needed.
  signal(SIGABRT, &BDS::HandleAborts); // aborts
  signal(SIGTERM, &BDS::HandleAborts); // termination requests
  signal(SIGSEGV, &BDS::HandleAborts); // segfaults
  // no interrupts since ctest sends an interrupt signal when interrupted
  // and then the BDSIM process somehow doesn't get killed
  // signal(SIGINT,  &BDS::HandleAborts); // interrupts
  
  // Write survey file
  if(execOptions->GetSurvey()) {
#ifdef BDSDEBUG 
    G4cout<<"contructing geometry interface"<<G4endl;
#endif
    BDSSurvey survey(execOptions->GetSurveyFilename());
  }

  if(!execOptions->GetBatch())   // Interactive mode
    {
      BDSVisManager visManager;
      visManager.StartSession(argc,argv);
    }
  else           // Batch mode
    { 
      runManager->BeamOn(globalConstants->GetNumberToGenerate());
    }

  //
  // job termination
  //
  G4GeometryManager::GetInstance()->OpenGeometry();

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> BDSOutput deleting..."<<G4endl;
#endif
  delete bdsOutput;
  
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> BDSBeamline deleting..."<<G4endl;
#endif

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> instances deleting..."<<G4endl;
#endif
  delete BDSAcceleratorModel::Instance();
  delete execOptions;
  delete globalConstants;
  delete BDSMaterials::Instance();

#ifdef BDSDEBUG 
  G4cout<< __FUNCTION__ << "> BDSRunManager deleting..."<<G4endl;
#endif
  delete runManager;
  delete bdsBunch;

  G4cout << __FUNCTION__ << "> End of Run, Thank you for using BDSIM!" << G4endl;

  return 0;
}
