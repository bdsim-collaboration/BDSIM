//  
//   BDSIM, (C) 2001-2008 
//   
//   version 0.5-dev
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

#include "G4UImanager.hh"        // G4 session managers
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"
#include "G4GeometryManager.hh"

#include "Randomize.hh"

#ifdef G4VIS_USE
#include "BDSVisManager.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif


#include <cstdlib>      // standard headers 
#include <cstdio>
#include <ctime>
#include <unistd.h>
#include <getopt.h>


#include "BDSDetectorConstruction.hh"   // Geant4 includes
#include "BDSPhysicsList.hh"
#include "QGSP_BERT.hh"
#include "QGSP_BERT_HP.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSRunAction.hh"
#include "BDSEventAction.hh"
#include "BDSSteppingAction.hh"
#include "BDSStackingAction.hh"
#include "BDSUserTrackingAction.hh"
#include "BDSRunManager.hh"
#include "G4EventManager.hh"
#include "G4TrackingManager.hh"
#include "G4SteppingManager.hh"
#include "G4GeometrySampler.hh"
#include "G4ImportanceAlgorithm.hh"
#include "G4GeometryTolerance.hh"
#include "G4ScoringManager.hh"

#include "BDSGeometryInterface.hh"

#include "BDSOutput.hh" 
#include "BDSBunch.hh"
#include "BDSMaterials.hh"
#include "BDSScoreWriter.hh"


#include "parser/gmad.h"  // GMAD parser




//=======================================================
// Global variables 
BDSOutput*    bdsOutput;         // output interface
BDSBunch      bdsBunch;          // bunch information 
BDSSamplerSD* BDSSamplerSensDet; // sampler???
//=======================================================

//=======================================================


int main(int argc,char** argv) {

  /* Executable command line options reader object */
  BDSExecOptions *bdsOptions = BDSExecOptions::Instance();
  bdsOptions->Parse(argc,argv);
  bdsOptions->Print();
  
#ifdef DEBUG
  G4cout << __FUNCTION__ << "> DEBUG mode is on." << G4endl;
#endif  

  //
  // Parse lattice file
  //
  G4cout << __FUNCTION__ << "> Using input file : "<< BDSExecOptions::Instance()->GetInputFilename()<<G4endl;
  if( gmad_parser(BDSExecOptions::Instance()->GetInputFilename()) == -1)
    {
      G4cout << __FUNCTION__ << "> Can't open input file "
	     << BDSExecOptions::Instance()->GetInputFilename()<<G4endl;
      exit(1);
    }


  //
  // pass the run control and beam options read from the lattice
  // file via the gmad parser to the BDSGlobalConstants and 
  // to the BDSBunch instances
  //

#ifdef DEBUG
  G4cout << __FUNCTION__ << "> Setting global constants." << G4endl;
#endif  

  //  BDSGlobals = new BDSGlobalConstants(options);
  if (BDSExecOptions::Instance()->GetFifoFilename()) {
    //    BDSGlobals->SetFifo(fifoName);
    BDSGlobalConstants::Instance()->SetFifo(BDSExecOptions::Instance()->GetFifoFilename());
  }

#ifdef DEBUG
  G4cout << __FUNCTION__ << "> Setting bunch options." << G4endl;
#endif  

  bdsBunch.SetOptions(options);

  //
  // set default output formats:
  //
#ifdef DEBUG
  G4cout << __FUNCTION__ << "> Setting up output." << G4endl;
#endif  

  bdsOutput = new BDSOutput();
  bdsOutput->SetFormat(BDSExecOptions::Instance()->GetOutputFormat());
  G4cout.precision(10);


  //
  // initialize random number generator
  //

  // choose the Random engine
#ifdef DEBUG
  G4cout << __FUNCTION__ << "> Initialising random number generator." << G4endl;
#endif  
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);

  long seed;

  // get the seed from options if positive, else
  // user time as a seed

  if(BDSGlobalConstants::Instance()->GetRandomSeed()>=0)
    seed = BDSGlobalConstants::Instance()->GetRandomSeed();
  else
    seed = time(NULL);

  // set the seed
  CLHEP::HepRandom::setTheSeed(seed);

  // Print generator full state to output 
  G4cout << __FUNCTION__ << "Random number generator's state: " << G4endl;
  CLHEP::HepRandom::saveFullState(G4cout);

#ifdef DEBUG
  G4cout << __FUNCTION__ << "> Seed from BDSGlobalConstants=" 
	 << BDSGlobalConstants::Instance()->GetRandomSeed() << G4endl;
#endif

  G4cout << __FUNCTION__ << "> Random number generator's seed=" 
	 << CLHEP::HepRandom::getTheSeed() << G4endl;


  //
  // construct mandatory run manager (the G4 kernel) and
  // set mandatory initialization classes
  //

#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> Constructing run manager"<<G4endl;
#endif
  BDSRunManager * runManager = new BDSRunManager;
  // runManager->SetNumberOfAdditionalWaitingStacks(1);

  //For geometry sampling, phys list must be initialized before detector.
#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> Constructing phys list" << G4endl;
#endif

#ifdef DEBUG 
  G4cout<<"constructing phys list"<<G4endl;
#endif 
  BDSPhysicsList* PhysList=new BDSPhysicsList;
  runManager->SetUserInitialization(PhysList);
  
#ifdef DEBUG 
  G4cout<< __FUNCTION__ << "> User init phys list"<<G4endl;
#endif

  // Set the geometry tolerance
  static G4GeometryTolerance* theGeometryTolerance = G4GeometryTolerance::GetInstance();
  G4cout << __FUNCTION__ << "> Default geometry tolerances: surface " 
	 << theGeometryTolerance->GetSurfaceTolerance() << " " 
	 << theGeometryTolerance->GetAngularTolerance() << " " 
	 << theGeometryTolerance->GetRadialTolerance()  << " " <<G4endl;
  G4double worldMaximumExtent=1000*m;
  // This sets the tolerances for the geometry (1e-11 times this value)
  G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldMaximumExtent); 
  G4cout << __FUNCTION__ << "> Geometry toleranceswith worldMaximumExtent=" 
	 << worldMaximumExtent/m << "m: surface: " 
	 << theGeometryTolerance->GetSurfaceTolerance() 
	 << " angular: " << theGeometryTolerance->GetAngularTolerance() 
	 << " radial: " << theGeometryTolerance->GetRadialTolerance() << " " <<G4endl;
  
  
#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> Constructing detector"<<G4endl;
#endif
  BDSDetectorConstruction* detector = new BDSDetectorConstruction();
 
#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> User init detector"<<G4endl;
#endif
  runManager->SetUserInitialization(detector);


  //
  // set user action classes
  //
#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> User action - runaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSRunAction);

#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> User action - eventaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSEventAction());

#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> User action - steppingaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSSteppingAction);

#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> User action - trackingaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSUserTrackingAction);

#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> User action - stackingaction"<<G4endl;
#endif
  runManager->SetUserAction(new BDSStackingAction);

#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> User action - detector"<<G4endl;
#endif
  runManager->SetUserAction(new BDSPrimaryGeneratorAction(detector));

  

  //
  // Initialize G4 kernel
  //
#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> Init kernel"<<G4endl;
#endif
  runManager->Initialize();

  //Create a geometric importance sampling store
  //  if(BDSGlobalConstants::Instance()->GetGeometryBias()){
  //    G4VIStore *aIstore = 0;
  //    aIstore = detector->CreateImportanceStore();
  //    G4GeometrySampler mgs(detector->GetWorldVolume(),"neutron");
  //    mgs.SetParallel(false);
  //    G4ImportanceAlgorithm* importanceAlgorithm = new G4ImportanceAlgorithm();
  //    mgs.PrepareImportanceSampling(aIstore, 0);//,0);
  //    mgs.Configure();
  //  }


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
  try {
    G4bool bCloseGeometry = G4GeometryManager::GetInstance()->CloseGeometry(true,true);
    if(!bCloseGeometry) throw "bdsim.cc: error - geometry not closed.";
  }
  catch (char* strng) {
    G4cerr << "Exception raised: " << strng << G4endl;
    return 1;
  }

  bdsOutput->Init(0); // activate the output - setting the first filename to 
                     // be appended with _0

  //
  // Write survey file
  //

  if(BDSExecOptions::Instance()->GetOutline()) {
#ifdef DEBUG 
    G4cout<<"contructing geometry interface"<<G4endl;
#endif
    BDSGeometryInterface* BDSGI = new BDSGeometryInterface(BDSExecOptions::Instance()->GetOutlineFilename());

#ifdef DEBUG 
    G4cout << __FUNCTION__ << "> Writing survey file"<<G4endl;
#endif
    if(BDSExecOptions::Instance()->GetOutlineFormat()=="survey") BDSGI->Survey();
    if(BDSExecOptions::Instance()->GetOutlineFormat()=="optics") BDSGI->Optics();

#ifdef DEBUG 
    G4cout<<"deleting geometry interface"<<G4endl;
#endif
    delete BDSGI;
  }


  // Track nptwiss particles for beta functions 
  // and SR Rescaling. SR rescaling is adjusting the magnet fields according to
  // k-values considering the beam energy loss due to SR
  //
  if(BDSGlobalConstants::Instance()->DoTwiss())
    {

      G4cout<<"do twiss"<<G4endl;
      
      // disable SR process if present - analytical formulae used in rescaling
      G4ProcessManager *pManager = G4Electron::Electron()->GetProcessManager(); 	 
      G4ProcessVector *procVec=pManager->GetProcessList(); 	 
      G4int nProc=pManager->GetProcessListLength(); 	 
      
      
      for(G4int iProc=0;iProc<nProc;iProc++) 	 
	{ 	 
	  G4String pName=(*procVec)[iProc]->GetProcessName(); 	 
	  if(pName=="BDSSynchRad")  	 
	    { 	 
	      G4cout << __FUNCTION__ << "> Disabling SR"<<G4endl;
	      pManager->SetProcessActivation(iProc, false);

	    } 	 

	  if(pName=="contSR")  	 
	    { 	 
	      G4cout << __FUNCTION__ << "> Enabling constSR"<<G4endl;
	      pManager->SetProcessActivation(iProc, true);
	      
	    } 	 
	}

      // do not need secondaries whatsoever
      BDSGlobalConstants::Instance()->SetStopTracks(true);

      runManager->BeamOn(BDSExecOptions::Instance()->GetNPTwiss());

      // Clear Stack
      G4EventManager::GetEventManager()->GetStackManager()->ClearPostponeStack();
      
      // turn  SR back on
      BDSGlobalConstants::Instance()->SetSynchTrackPhotons(options.synchTrackPhotons);

      //restore the stoptracks flag
      BDSGlobalConstants::Instance()->SetStopTracks(options.stopTracks);

      for(G4int iProc=0;iProc<nProc;iProc++) 	 
	{ 	 
	  G4String pName=(*procVec)[iProc]->GetProcessName(); 	 
	  if(pName=="BDSSynchRad")  	 
	    { 	 
	      G4cout<< __FUNCTION__ << "> Enabling SR"<<G4endl;
	      pManager->SetProcessActivation(iProc, true);
	      
	    } 	 

	  if(pName=="contSR")  	 
	    { 	 
	      G4cout<<"Disabling constSR"<<G4endl;
	      pManager->SetProcessActivation(iProc, false);
	      
	    } 	 

	}

      G4cout<<"done"<<G4endl;
    
    }
  
  // now turn off SR Rescaling 
  BDSGlobalConstants::Instance()->SetDoTwiss(false);
  BDSGlobalConstants::Instance()->SetSynchRescale(false);


  //
  // Start the simulation
  // If not running in batch:
  //   1) start interactive session
  //   2) if visualisation requested, initialise visual manager
  //   3) execute visualisation macro (defined with option --vis_mac)
  //   4) wait for user input
  // else 
  //   generate and track the particles of the bunch as 
  //   defined by the user in the gmad input file
  //

  G4UIsession* session=0;
  G4VisManager* visManager=0;
 
  if(!BDSExecOptions::Instance()->GetBatch())   // Interactive mode
    {
#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);
#else
      session = new G4UIterminal();
#endif    

#ifdef G4VIS_USE
#ifdef DEBUG 
      G4cout<< __FUNCTION__ << "> Initializing Visualisation Manager"<<G4endl;
#endif
      visManager = new BDSVisManager;
      visManager->Initialize();
#endif
  
      // get the pointer to the User Interface manager 
      G4UImanager* UIManager = G4UImanager::GetUIpointer();  

#ifdef G4UI_USE
      G4UIExecutive* session = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
      UIManager->ApplyCommand("/control/execute " + BDSExecOptions::Instance()->GetVisMacroFilename());    
#endif
      session->SessionStart();
      delete session;
#endif

#ifdef G4VIS_USE
#ifdef DEBUG 
      G4cout << __FUNCTION__ << "> Visualisation Manager deleting..." << G4endl;
#endif
      delete visManager;
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

#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> BDSOutput deleting..."<<G4endl;
#endif
  delete bdsOutput;

#ifdef DEBUG 
  G4cout << __FUNCTION__ << "> BDSGlobalConstants::Instance() deleting..."<<G4endl;
#endif
  delete BDSGlobalConstants::Instance();
  
#ifdef DEBUG 
  G4cout<< __FUNCTION__ << "> BDSRunManager deleting..."<<G4endl;
#endif
  delete runManager;
 
  G4cout << __FUNCTION__ << "> End of Run, Thank you for using BDSIM!" << G4endl;

   
  return 0;
}
