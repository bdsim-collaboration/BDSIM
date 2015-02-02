//  
//   BDSIM, (C) 2001-2014 
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
#include "G4GeometryManager.hh"

#ifdef G4VIS_USE
#include "BDSVisManager.hh"
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

#include "BDSDetectorConstruction.hh"   
#include "BDSEventAction.hh"
#include "BDSPhysicsList.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSRunAction.hh"
#include "BDSSteppingAction.hh"
#include "BDSStackingAction.hh"
#include "BDSUserTrackingAction.hh"
#include "BDSRunManager.hh"
#include "G4EventManager.hh" // Geant4 includes
#include "G4TrackingManager.hh"
#include "G4SteppingManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4TrajectoryDrawByCharge.hh"

#include "BDSBeamline.hh"
#include "BDSBunch.hh"
#include "BDSGeometryInterface.hh"
#include "BDSMaterials.hh"
#include "BDSOutputBase.hh" 
#include "BDSOutputASCII.hh" 
#include "BDSOutputROOT.hh" 
#include "BDSOutputVector.hh" 
#include "BDSRandom.hh" // for random number generator from CLHEP
//#ifdef USE_ROOT
//#include "BDSScoreWriter.hh"
//#endif

#include "parser/gmad.h"  // GMAD parser
#include "parser/options.h"

//=======================================================
// Global variables 
BDSOutputBase* bdsOutput=NULL;         // output interface
BDSBunch       bdsBunch;               // bunch information 
//=======================================================

//=======================================================

extern Options options;

void BDS_handle_aborts(int signal_number) {
  /** 
      Try to catch abort signals. This is not guaranteed to work.
      Main goal is to close output stream / files.
  */
  // prevent recursive calling
  static int nrOfCalls=0;
  if (nrOfCalls>0) exit(1);
  nrOfCalls++;
  std::cout << "BDSIM is about to crash or was interrupted! " << std::endl;
  std::cout << "With signal: " << strsignal(signal_number) << std::endl;
  std::cout << "Trying to write and close output file" << std::endl;
  bdsOutput->Write();
  std::cout << "Abort Geant4 run" << std::endl;
  G4RunManager::GetRunManager()->AbortRun();
  std::cout << "Ave, Imperator, morituri te salutant!" << std::endl;
}

int main(int argc,char** argv) {

  /* Initialize executable command line options reader object */
  BDSExecOptions::Instance(argc,argv)->Print();
  
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> DEBUG mode is on." << G4endl;
#endif  

  //
  // Parse lattice file
  //
  G4cout << __FUNCTION__ << "> Using input file : "<< BDSExecOptions::Instance()->GetInputFilename()<<G4endl;
  
  gmad_parser(BDSExecOptions::Instance()->GetInputFilename());
  
  //
  // initialize random number generator
  //

  BDS::CreateRandomNumberGenerator();
  BDS::SetSeed(); // set the seed from options or from exec options
  if (BDSExecOptions::Instance()->SetSeedState()) //optionally load the seed state from file
    {BDS::LoadSeedState(BDSExecOptions::Instance()->GetSeedStateFilename());}
  BDS::WriteSeedState(); //write the current state once set / loaded

  // instantiate the specific type of bunch distribution (class),
  // get the corresponding parameters from the gmad parser info
  // and attach to the initialised random number generator
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> Instantiating chosen bunch distribution." << G4endl;
#endif
  bdsBunch.SetOptions(options);
  
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
  if (BDSGlobalConstants::Instance()->GetThresholdCutPhotons() > 0 || BDSGlobalConstants::Instance()->GetThresholdCutCharged() > 0
      || BDSExecOptions::Instance()->GetVerboseStep()) {
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
  runManager->SetUserAction(new BDSPrimaryGeneratorAction());


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
  runManager->SetVerboseLevel(BDSExecOptions::Instance()->GetVerboseRunLevel());
  G4EventManager::GetEventManager()->SetVerboseLevel(BDSExecOptions::Instance()->GetVerboseEventLevel());
  G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(BDSExecOptions::Instance()->GetVerboseTrackingLevel());
  G4EventManager::GetEventManager()->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(BDSExecOptions::Instance()->GetVerboseSteppingLevel());
  
  //
  // Close the geometry
  //
  G4bool bCloseGeometry = G4GeometryManager::GetInstance()->CloseGeometry();
  if(!bCloseGeometry) { 
    G4cerr << "bdsim.cc: error - geometry not closed." << G4endl;
    return 1;
  }

  //
  // set default output formats:
  //
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> Setting up output." << G4endl;
#endif  

  if (BDSExecOptions::Instance()->GetOutputFormat() == BDSOutputFormat::_ASCII) {
    bdsOutput = new BDSOutputASCII();
  } else if (BDSExecOptions::Instance()->GetOutputFormat() == BDSOutputFormat::_ROOT) {
#ifdef USE_ROOT
    bdsOutput = new BDSOutputROOT();
#endif
  } else if (BDSExecOptions::Instance()->GetOutputFormat() == BDSOutputFormat::_COMBINED) {
    BDSOutputVector* combinedOutput = new BDSOutputVector();
    combinedOutput->Add(new BDSOutputASCII());
#ifdef USE_ROOT
    combinedOutput->Add(new BDSOutputROOT());
#endif
    bdsOutput = combinedOutput;
  }

  // set output precision
  G4cout.precision(10);

  // catch aborts to close output stream/file. perhaps not all are needed.
  signal(SIGABRT, &BDS_handle_aborts); // aborts
  signal(SIGTERM, &BDS_handle_aborts); // termination requests
  signal(SIGSEGV, &BDS_handle_aborts); // segfaults
  signal(SIGINT, &BDS_handle_aborts); // interrupts

  //
  // Write survey file
  //

  if(BDSExecOptions::Instance()->GetOutline()) {
#ifdef BDSDEBUG 
    G4cout<<"contructing geometry interface"<<G4endl;
#endif
    BDSGeometryInterface* BDSGI = new BDSGeometryInterface(BDSExecOptions::Instance()->GetOutlineFilename());

#ifdef BDSDEBUG 
    G4cout << __FUNCTION__ << "> Writing survey file"<<G4endl;
#endif
    if(BDSExecOptions::Instance()->GetOutlineFormat()=="survey") BDSGI->Survey();
    else if(BDSExecOptions::Instance()->GetOutlineFormat()=="optics") BDSGI->Optics();
    else {
      G4cout << __FUNCTION__ << "> Outlineformat " << BDSExecOptions::Instance()->GetOutlineFormat() << "is not known! exiting." << G4endl;
      exit(1);
    }
  }


  if(!BDSExecOptions::Instance()->GetBatch())   // Interactive mode
    {
      G4UIsession* session=0;
      G4VisManager* visManager=0;
#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);
#else
      session = new G4UIterminal();
#endif    

#ifdef G4VIS_USE
#ifdef BDSDEBUG 
      G4cout<< __FUNCTION__ << "> Initializing Visualisation Manager"<<G4endl;
#endif
      visManager = new BDSVisManager;
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
      UIManager->ApplyCommand("/control/execute " + BDSExecOptions::Instance()->GetVisMacroFilename());    
#endif
      session2->SessionStart();
      delete session2;
#endif
      delete session;

#ifdef G4VIS_USE
    }
#endif
  else           // Batch mode
    { 
      runManager->BeamOn(BDSGlobalConstants::Instance()->GetNumberToGenerate());
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
  delete BDSBeamline::Instance();

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> instances deleting..."<<G4endl;
#endif
  delete BDSExecOptions::Instance();
  delete BDSGlobalConstants::Instance();
  delete BDSMaterials::Instance();

#ifdef BDSDEBUG 
  G4cout<< __FUNCTION__ << "> BDSRunManager deleting..."<<G4endl;
#endif
  delete runManager; 

  G4cout << __FUNCTION__ << "> End of Run, Thank you for using BDSIM!" << G4endl;

   
  return 0;
}
