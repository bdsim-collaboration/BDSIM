//  
//   BDSIM, (C) 2001-2007 
//   
//   version 0.4
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



const int DEBUG = 0;

#include "BDSGlobalConstants.hh" // global parameters

#include "G4UImanager.hh"        // G4 session managers
#include "G4UIterminal.hh"
#include "G4UItcsh.hh"

#include "Randomize.hh"

#ifdef G4VIS_USE
#include "BDSVisManager.hh"
#endif


#include <cstdlib>      // standard headers 
#include <cstdio>
#include <unistd.h>
#include <getopt.h>


#include "BDSDetectorConstruction.hh"   // Geant4 includes
#include "BDSPhysicsList.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSRunAction.hh"
#include "BDSEventAction.hh"
#include "BDSSteppingAction.hh"
#include "BDSStackingAction.hh"
#include "BDSUserTrackingAction.hh"
#include "BDSSteppingVerbose.hh"
#include "BDSRunManager.hh"


#include "BDSGeometryInterface.hh"

#include "BDSOutput.hh" 
#include "BDSBunch.hh"
#include "BDSMaterials.hh"

#include "parser/gmad.h"  // GMAD parser




//=======================================================

// Print program usage
static void usage()
{
  G4cout<<"bdsim : version 0.4 build _UNKNOWN_BUILD_DATE_"<<G4endl;
  G4cout<<"        (C) 2001-2007 Royal Holloway University London"<<G4endl;
  G4cout<<"        http://ilc.pp.rhul.ac.uk/bdsim.html"<<G4endl;
  G4cout<<G4endl;

  G4cout<<"Usage: bdsim [options]"<<G4endl;
  G4cout<<"Options:"<<G4endl;
  G4cout<<"--file=<filename>    : specify the lattice file "<<G4endl
	<<"--output=<fmt>       : output format (root|ascii), default ascii"<<G4endl
	<<"--outfile=<file>     : output file name. Will be appended with _N"<<G4endl
        <<"                       where N = 0, 1, 2, 3... etc."<<G4endl
	<<"--vis_mac=<file>     : file with the visualization macro script, default vis.mac"<<G4endl
	<<"--help               : display this message"<<G4endl
	<<"--verbose            : display general parameters before run"<<G4endl
    	<<"--verbose_event      : display information for every event "<<G4endl
    	<<"--verbose_step=N     : display tracking information after each step"<<G4endl
	<<"--verbose_event_num  : display tracking information for event number N"<<G4endl
	<<"--batch              : batch mode - no graphics"<<G4endl
	<<"--outline=<file>     : print geometry info to <file>"<<G4endl
	<<"--outline_type=<fmt> : type of outline format"<<G4endl
	<<"                       where fmt = optics | survey"<<G4endl
	<<"--materials		: list materials included in bdsim by default"<<G4endl;

}


BDSGlobalConstants* BDSGlobals;  // global options instance
BDSOutput bdsOutput;                // output interface
BDSBunch theBunch;  // bunch information
G4int outputFormat=_ASCII;
G4String outputFilename="output";  //receives a .txt or .root in BDSOutput
G4String outlinefile="BDSOutline.dat";  
G4String outlineType="";
G4String inputFilename= "optics.gmad"; // input file with gmad lattice description
G4String visMacroFile="vis.mac"; // visualization macro file

G4bool outline = false;
G4bool verbose = false;  // run options
G4bool verboseStep = false;
G4bool verboseEvent = false;
G4int verboseEventNumber = -1;
G4bool isBatch = false;

BDSSamplerSD* BDSSamplerSensDet;

G4int nptwiss = 200; // number of particles for twiss parameters matching (by tracking)

//=======================================================


int main(int argc,char** argv) {

  // Parse the command line options 
  
   static struct option LongOptions[] = {
     { "help" , 0, 0, 0 },
     { "outline", 1, 0, 0 },
     { "outline_type", 1, 0, 0 },
     { "verbose", 0, 0, 0 },
     { "verbose_step", 0, 0, 0 },
     { "verbose_event", 0, 0, 0 },
     { "verbose_event_num", 1, 0, 0 },
     { "file", 1, 0, 0 },
     { "vis_mac", 1, 0, 0 },
     { "output", 1, 0, 0},
     { "outfile", 1, 0, 0},
     { "batch", 0, 0, 0 },  
     { "materials", 0, 0, 0 },
     { 0, 0, 0, 0 }
   };
  
  int OptionIndex = 0;
  int c;
  int ThisOptionId;
  
  for(;;)
    {
      
      ThisOptionId = optind ? optind : 1;
      OptionIndex = 0;
      
      c = getopt_long(argc, argv, "Vv",
		      LongOptions, &OptionIndex );
      
      if ( c == -1 ) // end of options list
  	break;
      
      switch (c) {
      case 0:
	
	if( !strcmp(LongOptions[OptionIndex].name , "help") )
	  {

	    usage();
	    return 1;
	  }
	
	if( !strcmp(LongOptions[OptionIndex].name , "batch") )
	  {
	    isBatch = true;
	  }
	if( !strcmp(LongOptions[OptionIndex].name , "verbose") )
	  {
	    verbose = true; 
	  }
	if( !strcmp(LongOptions[OptionIndex].name , "verbose_step") )
	  {
	    verboseStep = true; 
	  }
	if( !strcmp(LongOptions[OptionIndex].name , "verbose_event") )
	  {
	    verboseEvent = true; 
	  }
	if( !strcmp(LongOptions[OptionIndex].name , "verbose_event_num") )
	  {
	    if(optarg)
	      verboseEventNumber = atoi(optarg); 
	  }
	if( !strcmp(LongOptions[OptionIndex].name , "output") )
	  {
	    if(optarg) {
	      if(!strcmp(optarg,"ascii")) outputFormat=_ASCII;
	      else if (!strcmp(optarg,"root")) outputFormat=_ROOT;
	      else G4cerr<<"unknown output format "<<optarg<<G4endl;
	    }
	  }
	if( !strcmp(LongOptions[OptionIndex].name , "outfile") )
	  {
	    if(optarg) {
	      outputFilename=optarg;
	    }
	  }
	if( !strcmp(LongOptions[OptionIndex].name , "outline") )
	  {
	    if(optarg) outlinefile = optarg; 
	    outline=true;
	  }
	if( !strcmp(LongOptions[OptionIndex].name , "outline_type") )
	  {
	    if(optarg) outlineType = optarg; 
	    outline=true;  // can't have outline type without turning on outline!
	  }
	if( !strcmp(LongOptions[OptionIndex].name , "file") )
	  {
	    if(optarg) {
	      inputFilename=optarg;
	    }
	    else {
	      G4cout<<"please specify the lattice filename"<<G4endl;
	    }
	  }
	if( !strcmp(LongOptions[OptionIndex].name , "vis_mac") )
	  {
	    if(optarg) {
	      visMacroFile=optarg;
	    }
	    else {
	      G4cout<<"please specify the visualization macro file"<<G4endl;
	    }
	  }
	if( !strcmp(LongOptions[OptionIndex].name, "materials") )
	  {
	    BDSMaterials::ListMaterials();
	    return 1;
	  }
	break;
      case -1:
	break;
      default:
	break;
      }
      
    }
  
  
  G4cout<<"Using input file: "<<inputFilename<<G4endl;

  if( gmad_parser(inputFilename) == -1)   // parse lattice file
    {
      G4cout<<"can't open input file "<<inputFilename<<G4endl;
      exit(1);
    }

  // we shouldn't have verbose steps without verbose events etc.
  if(verboseStep) verboseEvent = true;

  BDSGlobals = new BDSGlobalConstants(options);

  theBunch.SetOptions(options);

  bdsOutput.SetFormat(outputFormat);


  // set default output formats:
  G4cout.precision(10);

  // choose the Random engine
  HepRandom::setTheEngine(new RanecuEngine);

  long seed;

  // get the seed from options if positive, else
  // user time as a seed

#include <ctime>

  if(BDSGlobals->GetRandomSeed()>=0)
    seed = BDSGlobals->GetRandomSeed();
  else
    seed = time(NULL);

  HepRandom::setTheSeed(seed);

  G4cout<<" seed from bdsglobals="<<BDSGlobals->GetRandomSeed()<<G4endl;
  G4cout<<"Random Number SEED ="<<HepRandom::getTheSeed()<<G4endl;

 
  BDSRunManager * runManager = new BDSRunManager;
//  runManager->SetNumberOfAdditionalWaitingStacks(1);

  // set mandatory initialization classes
 
  BDSDetectorConstruction* detector = new BDSDetectorConstruction;

  if(DEBUG) G4cout<<"detector construction done"<<G4endl;
 
  runManager->SetUserInitialization(detector);

  if(DEBUG) G4cout<<"constructing phys list"<<G4endl;
 
  BDSPhysicsList* PhysList=new BDSPhysicsList;
  
  if(DEBUG) G4cout<<"user init phys list"<<G4endl;
  runManager->SetUserInitialization(PhysList);
  if(DEBUG) G4cout<<"user init phys list done"<<G4endl;

  G4UIsession* session=0;

   if(!isBatch)
    {
#ifdef G4UI_USE_TCSH
      session = new G4UIterminal(new G4UItcsh);
#else
      session = new G4UIterminal();
#endif    
      
    }  


  if(DEBUG) G4cout<<"initializing visual manager"<<G4endl;


#ifdef G4VIS_USE  // visualization manager
  G4VisManager* visManager;
  if(!isBatch);
  {
    visManager = new BDSVisManager;
    visManager->Initialize();
  }
#endif
  
  
  // set user action classes
  if(DEBUG) G4cout<<"user action - detector"<<G4endl;
  runManager->SetUserAction(new BDSPrimaryGeneratorAction(detector));
  if(DEBUG) G4cout<<"user action - runaction"<<G4endl;

  runManager->SetUserAction(new BDSRunAction);

  
  //  BDSEventAction* theBDSEventAction = new BDSEventAction();
  if(DEBUG) G4cout<<"user action - eventaction"<<G4endl;
  runManager->SetUserAction(new BDSEventAction());

  if(DEBUG) G4cout<<"user action - steppingaction"<<G4endl;
  runManager->SetUserAction(new BDSSteppingAction);

  if(DEBUG) G4cout<<"user action - trackingaction"<<G4endl;
  runManager->SetUserAction(new BDSUserTrackingAction);

  if(DEBUG) G4cout<<"user action - stacking"<<G4endl;
  runManager->SetUserAction(new BDSStackingAction);
  

  if(DEBUG) G4cout<<"init kernel"<<G4endl;
  //Initialize G4 kernel
  runManager->Initialize();
  if(DEBUG) G4cout<<"init kernel done"<<G4endl;
  
  //  PhysList->BDSAddTransportation();
        
  bdsOutput.Init(0); // activate the output - setting the first filename to 
                     // be appended with _0

  BDSGeometryInterface* BDSGI = new BDSGeometryInterface(outlinefile);
  if(outline)
    {
      if(outlineType=="survey") BDSGI->Survey();
      if(outlineType=="optics") BDSGI->Optics();
    }


  // Track nptwiss particles for beta functions 
  // and SR Rescaling. SR rescaling is adjusting the magnet fields according to
  // k-values considering the beam energy loss due to SR

  if(BDSGlobals->DoTwiss())
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
	      G4cout<<"Disabling SR"<<G4endl;
	      pManager->SetProcessActivation(iProc, false);

	    } 	 

	  if(pName=="contSR")  	 
	    { 	 
	      G4cout<<"Enabling constSR"<<G4endl;
	      pManager->SetProcessActivation(iProc, true);
	      
	    } 	 
	}

      // do not need secondaries whatsoever
      BDSGlobals->SetStopTracks(true);
      
      runManager->BeamOn(nptwiss);

      // Clear Stack
      G4EventManager::GetEventManager()->GetStackManager()->ClearPostponeStack();
      
      // turn  SR back on
      BDSGlobals->SetSynchTrackPhotons(options.synchTrackPhotons);

      //restore the stoptracks flag
      BDSGlobals->SetStopTracks(options.stopTracks);

      for(G4int iProc=0;iProc<nProc;iProc++) 	 
	{ 	 
	  G4String pName=(*procVec)[iProc]->GetProcessName(); 	 
	  if(pName=="BDSSynchRad")  	 
	    { 	 
	      G4cout<<"Enabling SR"<<G4endl;
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
  BDSGlobals->SetDoTwiss(false);
  BDSGlobals->SetSynchRescale(false);

  if (!isBatch)   // Define UI session for interactive mode.
    {
      // get the pointer to the User Interface manager 
      G4UImanager* UI = G4UImanager::GetUIpointer();  
 
      UI->ApplyCommand("/control/execute " + visMacroFile);    
 
      session->SessionStart();
      delete session;
      
    }
  else           // Batch mode
    { 
      runManager->BeamOn(BDSGlobals->GetNumberToGenerate());
    }

  
  // job termination  
#ifdef G4VIS_USE
  if(!isBatch) delete visManager;
#endif
  
    
  //delete BDSGlobals;
  delete runManager;
  delete BDSGlobals;
     
  return 0;
}
