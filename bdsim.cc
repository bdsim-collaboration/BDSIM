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
#include "BDSModularPhysicsList.hh"
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
#include "G4TransportationManager.hh"
#include "G4PropagatorInField.hh"

#include "BDSBeamline.hh"
#include "BDSBunch.hh"
#include "BDSGeometryInterface.hh"
#include "BDSMaterials.hh"
#include "BDSOutputBase.hh" 
#include "BDSOutputASCII.hh" 
#include "BDSOutputROOT.hh" 
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
  G4int nAtt = (G4int)BDSGlobalConstants::Instance()->nAbortAttempts();
  if(nAtt == 0){
    std::cout << "BDSIM is about to crash or was interrupted! " << std::endl;
    std::cout << "With signal: " << strsignal(signal_number) << std::endl;
    std::cout << "Trying to write and close output file" << std::endl;
    bdsOutput->Write();
    G4RunManager::GetRunManager()->AbortRun();
    std::cout << "Ave, Imperator, morituri te salutant!" << std::endl;
  } else if (nAtt==1){
    bdsOutput->Write();
  } else {
    std::cout << "Failed to write and close output file." << std::endl;
    exit(-1);  
  }
  BDSGlobalConstants::Instance()->incrementAbortAttempts();
}

int main(int argc,char** argv) {

  /* Initialize executable command line options reader object */
#ifdef BDSDEBUG
  BDSExecOptions::Instance(argc,argv)->Print();
#else
  BDSExecOptions::Instance(argc,argv);
#endif
  
  std::cout << "> Test cout." << std::endl;

#ifdef BDSDEBUG
  std::cout << __FUNCTION__ << "> DEBUG mode is on." << std::endl;
  std::cout << "> DEBUG mode is on." << std::endl;
#endif  

  //
  // Parse lattice file
  //
  G4String infile = BDSExecOptions::Instance()->GetInputFilename();
  std::cout << __FUNCTION__ << "> Using input file : "<< infile <<std::endl;
  G4int bparser = gmad_parser(infile);
  std::cout << __FUNCTION__ << "> Finished parsing input file." <<std::endl;
  std::cout << "> Finished parsing input file." <<std::endl;
  std::cout << "> Finished parsing input file." <<std::endl;

  //
  // initialize random number generator
  //
  std::cout << "Initializing random number generator." <<std::endl;
  BDS::CreateRandomNumberGenerator();
  std::cout << "Finished initializing random number generator." <<std::endl;
#ifdef BDSDEBUG
  std::cout << __FUNCTION__ << " Setting seed." << std::endl;
#endif
  std::cout << "Setting random seed." <<std::endl;
  BDS::SetSeed(); // set the seed from options or from exec options
  std::cout << "Finished setting random seed." <<std::endl;
  if (BDSExecOptions::Instance()->SetSeedState()) //optionally load the seed state from file
    {BDS::LoadSeedState(BDSExecOptions::Instance()->GetSeedStateFilename());}
  std::cout<< __FUNCTION__ << "> Writing seed state."<<std::endl;
  BDS::WriteSeedState(); //write the current state once set / loaded

  // instantiate the specific type of bunch distibution (class),
  // get the corresponding parameters from the gmad parser info
  // and attach to the initialised random number generator
#ifdef BDSDEBUG
  std::cout << __FUNCTION__ << "> Instantiating chosen bunch distribution." << std::endl;
#endif
  std::cout<< __FUNCTION__ << "> Setting bunch options."<<std::endl;
  bdsBunch.SetOptions(options);
  
  //
  // construct mandatory run manager (the G4 kernel) and
  // set mandatory initialization classes
  //

#ifdef BDSDEBUG 
  std::cout << __FUNCTION__ << "> Constructing run manager"<<std::endl;
#endif
  BDSRunManager * runManager = new BDSRunManager;
  // runManager->SetNumberOfAdditionalWaitingStacks(1);

  //For geometry sampling, phys list must be initialized before detector.
#ifdef BDSDEBUG 
  std::cout << __FUNCTION__ << "> Constructing phys list" << std::endl;
#endif

  BDSModularPhysicsList* PhysList=new BDSModularPhysicsList;
  runManager->SetUserInitialization(PhysList);
  
#ifdef BDSDEBUG 
  std::cout<< __FUNCTION__ << "> User init phys list"<<std::endl;
#endif

  // Set the geometry tolerance
  static G4GeometryTolerance* theGeometryTolerance = G4GeometryTolerance::GetInstance();
  std::cout << __FUNCTION__ << "> Default geometry tolerances: surface " 
	 << theGeometryTolerance->GetSurfaceTolerance()/CLHEP::m << " m " 
	 << theGeometryTolerance->GetAngularTolerance() << " " 
	 << theGeometryTolerance->GetRadialTolerance()  << " " <<std::endl;
  G4double worldMaximumExtent=1000*CLHEP::m;
  // This sets the tolerances for the geometry (1e-11 times this value)
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldMaximumExtent); 
#ifdef BDSDEBUG
  std::cout<<__FUNCTION__<<"> Geometry Tolerances: " << std::endl;
  std::cout<<__FUNCTION__<<std::setw(23)<<" World Maximum Extent: "<<std::setw(15)<<worldMaximumExtent/CLHEP::m<<" m"<<std::endl;
  std::cout<<__FUNCTION__<<std::setw(23)<<" Surface: "             <<std::setw(15)<<theGeometryTolerance->GetSurfaceTolerance()/CLHEP::m<< " m"<<std::endl;
  std::cout<<__FUNCTION__<<std::setw(23)<<" Angular: "             <<std::setw(15)<<theGeometryTolerance->GetAngularTolerance()<<std::endl;
  std::cout<<__FUNCTION__<<std::setw(23)<<" Radial: "              <<std::setw(15)<<theGeometryTolerance->GetRadialTolerance()<<std::endl;
#endif
  
  std::cout << __FUNCTION__ << "> Constructing the accelerator"<<std::endl;
  BDSDetectorConstruction* detector = new BDSDetectorConstruction();
 
#ifdef BDSDEBUG 
  std::cout << __FUNCTION__ << "> User init detector"<<std::endl;
#endif
  runManager->SetUserInitialization(detector);


  //
  // set user action classes
  //
#ifdef BDSDEBUG 
  std::cout << __FUNCTION__ << "> User action - runaction"<<std::endl;
#endif
  runManager->SetUserAction(new BDSRunAction);

#ifdef BDSDEBUG 
  std::cout << __FUNCTION__ << "> User action - eventaction"<<std::endl;
#endif
  runManager->SetUserAction(new BDSEventAction());

#ifdef BDSDEBUG 
  std::cout << __FUNCTION__ << "> User action - steppingaction"<<std::endl;
#endif
  
  if (BDSGlobalConstants::Instance()->GetThresholdCutPhotons() > 0 || BDSGlobalConstants::Instance()->GetThresholdCutCharged() > 0
      || BDSExecOptions::Instance()->GetVerboseStep() || BDSExecOptions::Instance()->GetVerboseEventNumber()>=0) {
  runManager->SetUserAction(new BDSSteppingAction);
}
  
#ifdef BDSDEBUG 
  std::cout << __FUNCTION__ << "> User action - trackingaction"<<std::endl;
#endif
  runManager->SetUserAction(new BDSUserTrackingAction);

#ifdef BDSDEBUG 
  std::cout << __FUNCTION__ << "> User action - stackingaction"<<std::endl;
#endif
  runManager->SetUserAction(new BDSStackingAction);

#ifdef BDSDEBUG 
  std::cout << __FUNCTION__ << "> User action - primary generator"<<std::endl;
#endif
  runManager->SetUserAction(new BDSPrimaryGeneratorAction());


  //
  // Initialize G4 kernel
  //
#ifdef BDSDEBUG 
  std::cout << __FUNCTION__ << "> Init kernel"<<std::endl;
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
    G4cerr << "bdsim.cc: error - geometry not closed." << std::endl;
    return 1;
  }

  //Set the maximum acceptable step size
    G4TransportationManager::GetTransportationManager()->GetPropagatorInField()->SetLargestAcceptableStep(10*CLHEP::cm);

  //
  // set default output formats:
  //
#ifdef BDSDEBUG
  std::cout << __FUNCTION__ << "> Setting up output." << std::endl;
#endif  

  if (BDSExecOptions::Instance()->GetOutputFormat() == BDSOutputFormat::_ASCII) {
    bdsOutput = new BDSOutputASCII();
  } else if (BDSExecOptions::Instance()->GetOutputFormat() == BDSOutputFormat::_ROOT) {
#ifdef USE_ROOT
    bdsOutput = new BDSOutputROOT();
#endif
  }
  std::cout.precision(10);

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
    std::cout<<"contructing geometry interface"<<std::endl;
#endif
    BDSGeometryInterface* BDSGI = new BDSGeometryInterface(BDSExecOptions::Instance()->GetOutlineFilename());

#ifdef BDSDEBUG 
    std::cout << __FUNCTION__ << "> Writing survey file"<<std::endl;
#endif
    if(BDSExecOptions::Instance()->GetOutlineFormat()=="survey") BDSGI->Survey();
    if(BDSExecOptions::Instance()->GetOutlineFormat()=="optics") BDSGI->Optics();
  }


  if(!BDSExecOptions::Instance()->GetBatch())   // Interactive mode
    {
      //Magnet field line visualisation
      std::cout<< __FUNCTION__ << "> Initializing Visualisation Manager"<<std::endl;
      G4UIsession* session=0;
      G4VisManager* visManager=0;
#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);
#else
      session = new G4UIterminal();
#endif    

#ifdef G4VIS_USE
#ifdef BDSDEBUG 
      std::cout<< __FUNCTION__ << "> Initializing Visualisation Manager"<<std::endl;
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
  std::cout << __FUNCTION__ << "> BDSOutput deleting..."<<std::endl;
#endif
  delete bdsOutput;
  
#ifdef BDSDEBUG
  std::cout << __FUNCTION__ << "> BDSBeamline deleting..."<<std::endl;
#endif
  delete BDSBeamline::Instance();

#ifdef BDSDEBUG 
  std::cout << __FUNCTION__ << "> instances deleting..."<<std::endl;
#endif
  delete BDSExecOptions::Instance();
  delete BDSGlobalConstants::Instance();
  delete BDSMaterials::Instance();

#ifdef BDSDEBUG 
  std::cout<< __FUNCTION__ << "> BDSRunManager deleting..."<<std::endl;
#endif
  delete runManager; 

  std::cout << __FUNCTION__ << "> End of Run, Thank you for using BDSIM!" << std::endl;

   
  return 0;
}
