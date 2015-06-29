//  
//   BDSIM, (C) 2001-2015
//   
//   version 0.6
//  
//
//
//   Main code
//
//
//   History
//     17 Jul 2007 by Malton v.0.4
//     26 Jan 2007 by Agapov v.0.3
//     28 Mar 2006 by Agapov v.0.2
//     
//

#include "BDSDebug.hh" 
#include "BDSExecOptions.hh"     // executable command line options 
#include "BDSGlobalConstants.hh" //  global parameters

#include "G4UIterminal.hh"
#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh"
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#ifdef G4VIS_USE
#include "G4UImanager.hh"        // G4 session managers
#endif
#include "G4UIExecutive.hh"
#endif

#include <cstdlib>      // standard headers 
#include <cstdio>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>

#include "G4EventManager.hh" // Geant4 includes
#include "G4GeometryManager.hh"
#include "G4TrackingManager.hh"
#include "G4SteppingManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4TrajectoryDrawByCharge.hh"

#include "BDSBeamline.hh"
#include "BDSBunch.hh"
#include "BDSDetectorConstruction.hh"   
#include "BDSEventAction.hh"
#include "BDSGeometryInterface.hh"
#include "BDSLogicalVolumeInfoRegistry.hh"
#include "BDSMaterials.hh"
#include "BDSOutputBase.hh" 
#include "BDSOutputFactory.hh"
#include "BDSPhysicsList.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSRunAction.hh"
#include "BDSSteppingAction.hh"
#include "BDSStackingAction.hh"
#include "BDSUserTrackingAction.hh"
#include "BDSRandom.hh" // for random number generator from CLHEP
#include "BDSRunManager.hh"
#include "BDSUtilities.hh"

#include "parser/gmad.h"  // GMAD parser
#include "parser/options.h"

//=======================================================
// Global variables 
BDSOutputBase* bdsOutput=NULL;         // output interface
//=======================================================

extern Options options;

int main(int argc,char** argv) {

  /* Initialize executable command line options reader object */
  const BDSExecOptions* execOptions = BDSExecOptions::Instance(argc,argv);
  execOptions->Print();
  
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> DEBUG mode is on." << G4endl;
#endif  

  //
  // Parse lattice file
  //
  G4cout << __FUNCTION__ << "> Using input file : "<< execOptions->GetInputFilename()<<G4endl;
  
  gmad_parser(execOptions->GetInputFilename());

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
  BDSRandom::WriteSeedState(); //write the current state once set / loaded

  // instantiate the specific type of bunch distribution (class),
  // get the corresponding parameters from the gmad parser info
  // and attach to the initialised random number generator
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> Instantiating chosen bunch distribution." << G4endl;
#endif
  BDSBunch* bdsBunch = new BDSBunch();
  bdsBunch->SetOptions(options);
  
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

  BDSPhysicsList* PhysList=new BDSPhysicsList;
  runManager->SetUserInitialization(PhysList);
  
#ifdef BDSDEBUG 
  G4cout<< __FUNCTION__ << "> User init phys list"<<G4endl;
#endif

  // Set the geometry tolerance
  static G4GeometryTolerance* theGeometryTolerance = G4GeometryTolerance::GetInstance();
  G4cout << __FUNCTION__ << "> Default geometry tolerances: surface " 
	 << theGeometryTolerance->GetSurfaceTolerance()/CLHEP::m << " m " 
	 << theGeometryTolerance->GetAngularTolerance() << " " 
	 << theGeometryTolerance->GetRadialTolerance()  << " " <<G4endl;
  G4double worldMaximumExtent=1000*CLHEP::m;
  // This sets the tolerances for the geometry (1e-11 times this value)
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldMaximumExtent); 
#ifdef BDSDEBUG
  G4cout<<__FUNCTION__<<"> Geometry Tolerances: " << G4endl;
  G4cout<<__FUNCTION__<<std::setw(23)<<" World Maximum Extent: "<<std::setw(15)<<worldMaximumExtent/CLHEP::m<<" m"<<G4endl;
  G4cout<<__FUNCTION__<<std::setw(23)<<" Surface: "             <<std::setw(15)<<theGeometryTolerance->GetSurfaceTolerance()/CLHEP::m<< " m"<<G4endl;
  G4cout<<__FUNCTION__<<std::setw(23)<<" Angular: "             <<std::setw(15)<<theGeometryTolerance->GetAngularTolerance()<<G4endl;
  G4cout<<__FUNCTION__<<std::setw(23)<<" Radial: "              <<std::setw(15)<<theGeometryTolerance->GetRadialTolerance()<<G4endl;
#endif
  
  G4cout << __FUNCTION__ << "> Constructing the accelerator"<<G4endl;
  BDSDetectorConstruction* detector = new BDSDetectorConstruction();
 
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> User init detector"<<G4endl;
#endif
  runManager->SetUserInitialization(detector);

  //
  // set user action classes
  //
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> User action - runaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSRunAction);

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> User action - eventaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSEventAction());

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> User action - steppingaction"<<G4endl;
#endif
  // Only add steppingaction if it is actually used, so do check here (for cpu reasons)
  if (globalConstants->GetThresholdCutPhotons() > 0 || globalConstants->GetThresholdCutCharged() > 0
      || execOptions->GetVerboseStep()) {
    runManager->SetUserAction(new BDSSteppingAction);
  }
  
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> User action - trackingaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSUserTrackingAction);

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> User action - stackingaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSStackingAction);

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> User action - primary generator"<<G4endl;
#endif
  runManager->SetUserAction(new BDSPrimaryGeneratorAction(bdsBunch));


  //
  // Initialize G4 kernel
  //
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Init kernel"<<G4endl;
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
  
  // set default output formats:
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> Setting up output." << G4endl;
#endif
  bdsOutput = BDSOutputFactory::createOutput(execOptions->GetOutputFormat());
  G4cout.precision(10);

  // catch aborts to close output stream/file. perhaps not all are needed.
  signal(SIGABRT, &BDS::HandleAborts); // aborts
  signal(SIGTERM, &BDS::HandleAborts); // termination requests
  signal(SIGSEGV, &BDS::HandleAborts); // segfaults
  signal(SIGINT,  &BDS::HandleAborts); // interrupts
  
  // Write survey file
  if(execOptions->GetOutline()) {
#ifdef BDSDEBUG 
    G4cout<<"contructing geometry interface"<<G4endl;
#endif
    BDSGeometryInterface* BDSGI = new BDSGeometryInterface(execOptions->GetOutlineFilename());

#ifdef BDSDEBUG 
    G4cout << __FUNCTION__ << "> Writing survey file"<<G4endl;
#endif
    if(execOptions->GetOutlineFormat()=="survey") BDSGI->Survey();
    else if(execOptions->GetOutlineFormat()=="optics") BDSGI->Optics();
    else {
      G4cout << __FUNCTION__ << "> Outlineformat " << execOptions->GetOutlineFormat() << "is not known! exiting." << G4endl;
      exit(1);
    }
  }

  if(!execOptions->GetBatch())   // Interactive mode
    {
      G4UIsession* session=0;
#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);
#else
      session = new G4UIterminal();
#endif

#ifdef G4VIS_USE
#ifdef BDSDEBUG 
      G4cout<< __FUNCTION__ << "> Initializing Visualisation Manager"<<G4endl;
#endif
      // Initialize visualisation
      G4VisManager* visManager = new G4VisExecutive;
      // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
      // G4VisManager* visManager = new G4VisExecutive("Quiet");
      visManager->Initialize();
      
      G4TrajectoryDrawByCharge* trajModel1 = new G4TrajectoryDrawByCharge("trajModel1");
      visManager->RegisterModel(trajModel1);
      visManager->SelectTrajectoryModel(trajModel1->Name());
#endif
 
#ifdef G4UI_USE
      G4UIExecutive* session2 = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
      // get the pointer to the User Interface manager 
      G4UImanager* UIManager = G4UImanager::GetUIpointer();

      std::string bdsimPath = BDS::GetBDSIMExecPath();
      // difference between local build and install build:
      std::string visPath;
      std::string localPath = bdsimPath + "vis/vis.mac";
      std::string installPath = bdsimPath + "../share/BDSIM/vis/vis.mac";
      
      if (FILE *file = fopen(localPath.c_str(), "r")) {
	fclose(file);
	visPath = bdsimPath + "vis/";
      } else if (FILE *file = fopen(installPath.c_str(), "r")) {
	fclose(file);
	visPath = bdsimPath + "../share/BDSIM/vis/";
      } else {
	G4cout << __FUNCTION__ << "> ERROR: default visualisation file could not be found!" << G4endl;
      }

      // check if visualisation file is present and readable
      std::string visMacroName = execOptions->GetVisMacroFilename();
      bool useDefault = false;
      // if not set use default visualisation file
      if (visMacroName.empty()) useDefault = true;
      G4String visMacroFilename = BDS::GetFullPath(visMacroName);
      if (!useDefault) {
	FILE* file = NULL;
	// first relative to main path:
	file = fopen(visMacroFilename.c_str(), "r");
	if (file) {
	  fclose(file);
	} else {
	  // if not present use a default one (OGLSQt or DAWNFILE)
	  G4cout << __FUNCTION__ << "> WARNING: visualisation file " << visMacroFilename <<  " file not present, using default!" << G4endl;
	  useDefault = true;
	}
      }
      if (useDefault) {
#ifdef G4VIS_USE_OPENGLQT
	visMacroFilename = visPath + "vis.mac";
#else
	visMacroFilename = visPath + "dawnfile.mac";
#endif
      }
      // execute visualisation file
      UIManager->ApplyCommand("/control/execute " + visMacroFilename);

      // add default gui
      if (session2->IsGUI()) {
	// Add icons
	std::string iconMacroFilename = visPath + "icons.mac";
	UIManager->ApplyCommand("/control/execute " + iconMacroFilename);
	// add menus
	std::string guiMacroFilename  = visPath + "gui.mac";
	UIManager->ApplyCommand("/control/execute " + guiMacroFilename);
	// add run icon:
	std::string runButtonFilename = visPath + "run.png";
	UIManager->ApplyCommand("/gui/addIcon \"Run beam on\" user_icon \"/run/beamOn 1\" " + runButtonFilename);
      }
#endif
      session2->SessionStart();
      delete session2;
#endif
      delete session;

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
  delete BDSLogicalVolumeInfoRegistry::Instance();
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
