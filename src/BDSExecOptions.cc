#include "BDSExecOptions.hh"

#include <iomanip>
#include <unistd.h>

#include "globals.hh" // geant4's globals that is...

#include "BDSDebug.hh"
#include "BDSMaterials.hh"
#include "BDSOutputFormat.hh"

#include "parser/getEnv.h"

BDSExecOptions* BDSExecOptions::_instance=0;

BDSExecOptions* BDSExecOptions::Instance(int argc, char **argv){
  if(_instance==0) {
    _instance = new BDSExecOptions(argc, argv);
    return _instance;
  } else {
    G4Exception("BDSExecOptions::Instance is already initialized. Return pointer to singleton with BDSExecOptions::Instance()", "-1", FatalException, "");
    return NULL;
  }
}

BDSExecOptions* BDSExecOptions::Instance(){
  if(_instance==0) {
    G4Exception("BDSExecOptions::Instance was not initialised. Initialize first with BDSExecOptions::Instance(int argc, char **argv).", "-1", FatalException, "");
    return NULL;
  } else 
    return _instance;
}

BDSExecOptions::BDSExecOptions(int argc, char **argv){
  inputFilename       = "optics.mad";
  visMacroFilename    = "vis.mac";
  outputFilename      = "output";
  outputFormat        = BDSOutputFormat::_ASCII;
  outline             = false;
  outlineFilename     = "outline.dat";
  outlineFormat       = "";

  gflash      = 0;
  gflashemax  = 10000;
  gflashemin  = 0.1;

  verbose       = false;
  verboseEvent  = false;
  verboseStep   = false;
  verboseEventNumber   = -1;
  batch         = false; 
  listMaterials = false;
  
  verboseRunLevel      = 0;
  verboseEventLevel    = 0;
  verboseTrackingLevel = 0;
  verboseSteppingLevel = 0;
  
  circular      = false;
  
  seed              = -1;
  setSeed           = false;
  seedStateFilename = "";
  setSeedState      = false;

  Parse(argc, argv);
  SetBDSIMPATH();
}

BDSExecOptions::~BDSExecOptions() {
  _instance = 0;
}

/** <Parse the command line options>
 * @param command line input number of variables
 * @param array of char* of the input parameters
 */
void BDSExecOptions::Parse(int argc, char **argv) {
  static struct option LongOptions[] = {{ "help" , 0, 0, 0 },
					{ "outline", 1, 0, 0 },
					{ "outline_type", 1, 0, 0 },
					{ "verbose", 0, 0, 0 },
					{ "verbose_step", 0, 0, 0 },
					{ "verbose_event", 0, 0, 0 },
					{ "verbose_event_num", 1, 0, 0 },
					{ "verbose_G4run", 1, 0, 0 },
					{ "verbose_G4event", 1, 0, 0 },
					{ "verbose_G4tracking", 1, 0, 0 },
					{ "verbose_G4stepping", 1, 0, 0 },
					{ "file", 1, 0, 0 },
					{ "vis_mac", 1, 0, 0 },
					{ "gflash", 1, 0, 0 },
					{ "gflashemax", 1, 0, 0 },
					{ "gflashemin", 1, 0, 0 },
					{ "output", 1, 0, 0 },
					{ "outfile", 1, 0, 0 },
					{ "batch", 0, 0, 0 },
					{ "materials", 0, 0, 0 },
					{ "circular", 0, 0, 0},
					{ "seed", 1, 0, 0},
					{ "seedstate",1,0,0},
					{ 0, 0, 0, 0 }};
  
  int OptionIndex = 0;
  int c;
 
  for(;;) {
    OptionIndex = 0;
  
    // see e.g. http://linux.die.net/man/3/getopt
    c = getopt_long(argc, argv, "Vv",
		    LongOptions, &OptionIndex );
    
    if ( c == -1 ) // end of options list
      break;
    
    switch (c) {
    case '?': // unrecognised option
      G4cout << "invalid option for command " << argv[0] << G4endl << G4endl << G4endl;
      Usage();
      exit(1);
      break;
    case 0:
      if( !strcmp(LongOptions[OptionIndex].name , "help") ) {
	Usage();
	exit(0);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "batch") ) {
	batch = true;
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose") ) {
	verbose = true; 
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_step") ) {
	verboseStep = true; 
	// we shouldn't have verbose steps without verbose events etc.
	verboseEvent = true;
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_event") ) {
	verboseEvent = true; 
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_event_num") ){
	if(optarg)
	  verboseEventNumber = atoi(optarg); 
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_G4run") ) {
	if(optarg)
	  verboseRunLevel = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_G4event") ) {
	if(optarg)
	  verboseEventLevel = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_G4tracking") )  {
	if(optarg)
	  verboseTrackingLevel = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "verbose_G4stepping") ) {
	if(optarg)
	  verboseSteppingLevel = atoi(optarg);
      }
      if( !strcmp(LongOptions[OptionIndex].name , "output") ) {
	if(optarg) {
	  if(!strcmp(optarg,"ascii") || !strcmp(optarg,"ASCII")) outputFormat=BDSOutputFormat::_ASCII;
	  else if (!strcmp(optarg,"root") || !strcmp(optarg,"ROOT")) outputFormat=BDSOutputFormat::_ROOT;
	  else if (!strcmp(optarg,"combined") || !strcmp(optarg,"COMBINED")) outputFormat=BDSOutputFormat::_COMBINED;
	  else {
	    G4cerr<<"unknown output format "<<optarg<<G4endl;
	    exit(1);
	  }
#ifndef USE_ROOT
	  if (outputFormat == BDSOutputFormat::_ROOT || outputFormat == BDSOutputFormat::_COMBINED) {
	    G4cerr << "ERROR outputFormat root, but BDSIM not configured with ROOT support!" << G4endl;
	    G4cerr << "Use ascii instead, or recompile with ROOT!" << G4endl;
	    exit(1);
	  }
#endif
	}
      }
      if( !strcmp(LongOptions[OptionIndex].name , "outfile") ) {
	if(optarg) {
	  outputFilename=optarg;
	}
      }
      if( !strcmp(LongOptions[OptionIndex].name , "outline") ) {
	if(optarg) outlineFilename = optarg; 
	outline=true;
      }
      if( !strcmp(LongOptions[OptionIndex].name , "outline_type") ) {
	if(optarg) outlineFormat = optarg; 
	outline=true;  // can't have outline type without turning on outline!
      }
      if( !strcmp(LongOptions[OptionIndex].name , "file") ) {
	if(optarg) {
	  inputFilename=optarg;
	}
	else {
	  G4cout<<"please specify the lattice filename"<<G4endl;
	}
      }
      if( !strcmp(LongOptions[OptionIndex].name , "vis_mac") ) {
	if(optarg) {
	  visMacroFilename=optarg;
	}
	else {
	  G4cout<<"please specify the visualization macro file"<<G4endl;
	}
      }
      if( !strcmp(LongOptions[OptionIndex].name , "gflash") ) {
	if(optarg)
	  gflash = atoi(optarg); 
      }
      if( !strcmp(LongOptions[OptionIndex].name , "gflashemax") ) {
	if(optarg)
	  gflashemax = atof(optarg); 
      }
      if( !strcmp(LongOptions[OptionIndex].name , "gflashemin") ) {
	if(optarg)
	  gflashemin = atof(optarg); 
      }
      if( !strcmp(LongOptions[OptionIndex].name, "materials") ) {
	BDSMaterials::ListMaterials();
	exit(0);
      }
      if( !strcmp(LongOptions[OptionIndex].name, "circular")  ) {
	circular = true;
      }
      if( !strcmp(LongOptions[OptionIndex].name, "seed")  ){
	seed = atoi(optarg);
	setSeed = true;
      }
      if( !strcmp(LongOptions[OptionIndex].name, "seedstate") ){
	if(optarg) {
	  seedStateFilename = optarg;
	}
	setSeedState = true;
      }
      break;
      
    default:
      break;
    }
  } 
}

void BDSExecOptions::Usage() {
  G4cout<<"bdsim : version 0.6.develop"<<G4endl;
  G4cout<<"        (C) 2001-2015 Royal Holloway University London"<<G4endl;
  G4cout<<"        http://www.ph.rhul.ac.uk/twiki/bin/view/PP/JAI/BdSim"<<G4endl;
  G4cout<<G4endl;

  G4cout<<"Usage: bdsim [options]"<<G4endl;
  G4cout<<"Options:"<<G4endl;
  G4cout<<"--file=<filename>      : specify the lattice and options file "<<G4endl
	<<"--batch                : batch mode - no graphics"<<G4endl
	<<"--circular             : assume circular machine - turn control"<<G4endl
	<<"--gflash=N             : whether or not to turn on gFlash fast shower parameterisation. Default 0."<<G4endl
	<<"--gflashemax=N         : maximum energy for gflash shower parameterisation in GeV. Default 10000."<<G4endl
	<<"--gflashemin=N         : minimum energy for gflash shower parameterisation in GeV. Default 0.1."<<G4endl
	<<"--help                 : display this message"<<G4endl
	<<"--materials            : list materials included in bdsim by default"<<G4endl
	<<"--outline=<file>       : print geometry info to <file>"<<G4endl
	<<"--outline_type=<fmt>   : type of outline format"<<G4endl
	<<"                         where fmt = optics | survey"<<G4endl
	<<"--output=<fmt>         : output format (root|ascii|combined), default ascii"<<G4endl
	<<"--outfile=<file>       : output file name. Will be appended with _N"<<G4endl
        <<"                         where N = 0, 1, 2, 3... etc."<<G4endl
        <<"--seed=N               : the seed to use for the random number generator" <<G4endl
	<<"--seedstate=<file>     : file containing CLHEP::Random seed state - overrides other seed options"<<G4endl
	<<"--verbose              : display general parameters before run"<<G4endl
	<<"--verbose_event        : display information for every event "<<G4endl
	<<"--verbose_event_num=N  : display tracking information for event number N"<<G4endl
	<<"--verbose_step         : display tracking information after each step"<<G4endl
	<<"--verbose_G4event=N    : set Geant4 Event manager verbosity level"<<G4endl
	<<"--verbose_G4run=N      : set Geant4 verbosity level (see Geant4 manual for details)"<<G4endl
	<<"--verbose_G4stepping=N : set Geant4 Stepping manager verbosity level"<<G4endl
	<<"--verbose_G4tracking=N : set Geant4 Tracking manager verbosity level [-1:5]"<<G4endl
	<<"--vis_mac=<file>       : file with the visualization macro script, default vis.mac"<<G4endl;
}

void BDSExecOptions::SetBDSIMPATH(){
  //Set itsBDSIMPATH to mirror what is done in parser.l (i.e. if no environment varible set, assume base filename path is that of the gmad file).
  itsBDSIMPATH = getEnv("BDSIMPATH");
  if(itsBDSIMPATH.length()<=0){
    G4String inputFilepath = "";
    // get the path part of the supplied path to the main input file
    G4String::size_type found = inputFilename.rfind("/"); // find the last '/'
    if (found != G4String::npos){
      inputFilepath = inputFilename.substr(0,found); // the path is the bit before that
    } // else remains empty string
    // need to know whether it's an absolute or relative path
    if ((inputFilename.substr(0,1)) == "/"){
      // the main file has an absolute path
      itsBDSIMPATH = inputFilepath;
    } else {
      // the main file has a relative path
      char cwdchars[200]; //filepath up to 200 characters
      G4String cwd = (G4String)getcwd(cwdchars, sizeof(cwdchars)) + "/";
      itsBDSIMPATH = cwd + inputFilepath;
    
    }
  }
  itsBDSIMPATH += "/";
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " BDSIMPATH set to: " << itsBDSIMPATH << G4endl;
#endif

}

void BDSExecOptions::Print() {
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " inputFilename: "       << std::setw(15) << inputFilename       << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " visMacroFilename: "    << std::setw(15) << visMacroFilename    << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " outputFilename: "      << std::setw(15) << outputFilename      << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " outputFormat: "        << std::setw(15) << outputFormat        << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " outlineFilename: "     << std::setw(15) << outlineFilename     << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " gflash: "              << std::setw(15) << gflash              << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " gflashemin: "          << std::setw(15) << gflashemin          << G4endl;  
  G4cout << __METHOD_NAME__ << std::setw(23) << " gflashemax: "          << std::setw(15) << gflashemax          << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " verbose: "             << std::setw(15) << verbose             << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " verboseEvent: "        << std::setw(15) << verboseEvent        << G4endl;  
  G4cout << __METHOD_NAME__ << std::setw(23) << " verboseStep: "         << std::setw(15) << verboseStep         << G4endl;  
  G4cout << __METHOD_NAME__ << std::setw(23) << " batch: "               << std::setw(15) << batch               << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " outline: "             << std::setw(15) << outline             << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " listMaterials: "       << std::setw(15) << listMaterials       << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " verboseRunLevel: "     << std::setw(15) << verboseRunLevel     << G4endl;  
  G4cout << __METHOD_NAME__ << std::setw(23) << " verboseEventLevel: "   << std::setw(15) << verboseEventLevel   << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " verboseTrackingLevel: "<< std::setw(15) << verboseTrackingLevel<< G4endl;  
  G4cout << __METHOD_NAME__ << std::setw(23) << " verboseSteppingLevel: "<< std::setw(15) << verboseSteppingLevel<< G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " circular: "            << std::setw(15) << circular            << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " seed: "                << std::setw(15) << seed                << G4endl;
  G4cout << __METHOD_NAME__ << std::setw(23) << " seedStateFilename: "   << std::setw(15) << seedStateFilename   << G4endl;
  return;
}
